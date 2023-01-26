#include <cstdio>

#include <mutex>
#include <string_view>
#include <thread>

#include "accelerate/bvh_node.hh"
#include "gm/util.hh"
#include "img/color.hh"
#include "img/tga_image.hh"
#include "material/dielectric.hh"
#include "material/diffuse_light.hh"
#include "material/lambertian.hh"
#include "material/matal.hh"
#include "rt/camera.hh"
#include "rt/hit_record.hh"
#include "rt/ray.hh"
#include "rt/scatter_record.hh"
#include "shape/box.hh"
#include "shape/rect.hh"
#include "shape/rotate.hh"
#include "shape/translate.hh"
#include "shape/shape_list.hh"
#include "shape/sphere.hh"
#include "shape/flip_face.hh"
#include "texture/checker_texture.hh"
#include "texture/image_texture.hh"
#include "texture/solid_texture.hh"
#include "util/atomic_counter.h"
#include "util/progress_bar.hh"
#include "util/random.hh"
#include "util/str_cvt.hh"

#define MAX_DEPTH 50

using namespace util;
using namespace img;
using namespace rt;
using namespace gm;
using namespace std;

namespace ktm = std::chrono;

Vec3F ray_color(Ray const &ray, rt::Color const &background, Shape const &shape,
                int max_depth);

img::Color compute_color(Vec3F const &c, int sample_per_pixel,
                         double gamma_exp);

struct Option {
  char const *path = nullptr;
  int sample_per_pixel = 100;
  int thread_num = 8;
  int gamma = 2;
  int image_height = 400;
  int scene_id = -1;
  void DebugPrint() const;
};

bool parse_options(int argc, char *argv[], Option *option);
void setup_scene(ShapeList &world);
void setup_random_scene(ShapeList &world);
void setup_light_scene(ShapeList &world);
void setup_image_scene(ShapeList &world);
void setup_cornellbox(ShapeList &world);

int main(int argc, char *argv[])
{
  Option option;
  if (!parse_options(argc, argv, &option)) {
    return EXIT_FAILURE;
  }

  option.DebugPrint();
  double gamma_exp = 1. / option.gamma;

  // Setup camera
  double aspect_ratio = 16. / 9.;

  // Setup Scene
  ShapeList world;

  rt::Color background(0, 0, 0);
  Point3F lookfrom(0, 0, 0);
  Point3F lookat(0, 0, -1);
  double fov = 90;

  switch (option.scene_id) {
    case 0: {
      setup_scene(world);
    } break;
    case 1: {
      background = rt::Color(0.8, 0.4, 0.3);
      lookfrom = Point3F(13., 2., 3.);
      // Point3F lookfrom(0, 0, 0);
      lookat = Point3F(0, 0, 0);
      // auto focus_dist = (lookat-lookfrom).length();
      fov = 30;
      setup_random_scene(world);
    } break;

    case 2: {
      background = rt::Color(0, 0, 0);
      lookfrom = Point3F(26, 3, 6);
      lookat = Point3F(0, 2, 0);
      fov = 20;
      setup_light_scene(world);
    } break;

    case 3: {
      background = rt::Color(1, 1, 1);
      lookfrom = Point3F(13, 2, 3);
      lookat = Point3F(0, 0, 0);
      fov = 20;
      setup_image_scene(world);
    } break;

    default:
    case 4: {
      // background = rt::Color(1, 1, 1);
      lookfrom = Point3F(0, 278, 800);
      lookat = Point3F(0, 278, 0);
      fov = 40;
      setup_cornellbox(world);
    } break;
  }

  Camera camera(lookfrom, lookat, aspect_ratio, fov, 1);
  camera.set_aperture(0.0);
  camera.DebugPrint();
  // Setup image
  int image_width = aspect_ratio * option.image_height;
  TgaImage image(image_width, option.image_height);

  std::vector<std::thread> thrs;
  thrs.reserve(option.thread_num);

  int start = image.height() - 1;
  int slice = (image.height() - 1) / option.thread_num;
  printf("init start = %d\n", start);
  printf("slice = %d\n", slice);

  const size_t total_sample =
      image.height() * image.width() * option.sample_per_pixel;
  AtomicCounter64 current_complete_sample(0);

  auto start_of_render = ktm::steady_clock::now();

  for (int ti = 0; ti < option.thread_num; ++ti) {
    int end = start - slice;
    if (ti == option.thread_num - 1 && end != 0) {
      end = 0;
    }

    // printf("slice [%d, %d]\n", start, end);
    thrs.emplace_back(
        std::thread([start, end, &option, gamma_exp, &background, &world,
                     &camera, &image, &current_complete_sample]() {
          for (int j = start; j >= end; --j) {
            for (int i = 0; i < image.width(); ++i) {
              // propertion
              Vec3F color_prop(0, 0, 0);
              for (int k = 0; k < option.sample_per_pixel; ++k) {
                auto offset = double(k) / option.sample_per_pixel;
                auto u = double(i + offset) / (image.width() - 1);
                auto v = double(j + offset) / (image.height() - 1);

                auto ray = camera.ray(u, v);
                color_prop += ray_color(ray, background, world, MAX_DEPTH);
                current_complete_sample++;
              }
              auto color =
                  compute_color(color_prop, option.sample_per_pixel, gamma_exp);
              image.SetPixel(i, j, color);
            }
          }
        }));

    start -= slice + 1;
  }

  while (1) {
    auto current_value = current_complete_sample.GetValue();
    if (current_value >= total_sample) break;
    update_progress_bar('#', current_value / double(total_sample) * 100);
    std::this_thread::sleep_for(1s);
  }
  update_progress_bar('#', 100);

  for (auto &thr : thrs) {
    thr.join();
  }

  auto end_of_render = ktm::steady_clock::now();
  ktm::duration<double> cost_time_of_render = end_of_render - start_of_render;
  printf("\nThe consume time of render is %.3lf sec\n",
         cost_time_of_render.count());
  fflush(stdout);
  image.WriteTo(option.path);

  return EXIT_SUCCESS;
}

#define PROGRAM_USAGE                                                          \
  "Usage: %s [image path] "                                                    \
  "[--sample_per_pixel/-spp integer] "                                         \
  "[--threads/-t integer]"                                                     \
  "[--gamma/-g integer]"                                                       \
  "[--height/-h integer]\n",                                                   \
      argv[0]

inline bool check_option(std::string_view opt, char const *lopt,
                         char const *sopt) noexcept
{
  return opt == lopt || opt == sopt;
}

bool parse_options(int argc, char *argv[], Option *option)
{
  if (2 > argc) {
    fprintf(stderr, PROGRAM_USAGE);
    return false;
  }

  if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) {
    printf(PROGRAM_USAGE);
    return true;
  }

  option->path = argv[1];

  argc -= 2;
  argv += 2;

  while (argc > 0) {
    auto eq_pos = strchr(*argv, '=');
    if (!eq_pos) {
      fprintf(stderr, "This is not a valid option format: <option> = value\n");
      return false;
    }

    std::string_view opt(*argv, eq_pos - *argv);
    char const *arg = eq_pos + 1;

    if (opt == "-spp" || opt == "--sample-per-pixel") {
      auto ret = util::str2int(arg);
      if (!ret) {
        fprintf(stderr, "The argument of --sample-per-pixel/-spp is invalid\n");
        return false;
      }
      option->sample_per_pixel = *ret;
    } else if (opt == "--threads" || opt == "-t") {
      auto ret = util::str2int(arg);
      if (!ret) {
        fprintf(stderr, "The argument of --threads/-t is invalid\n");
        return false;
      }
      option->thread_num = *ret;
    } else if (opt == "--height" || opt == "-h") {
      auto ret = util::str2int(arg);
      if (!ret) {
        fprintf(stderr, "The argument of --height/-h is invalid\n");
        return false;
      }
      option->image_height = *ret;
    } else if (opt == "--gamma" || opt == "-g") {
      auto ret = util::str2int(arg);
      if (!ret) {
        fprintf(stderr, "The argument of --gamma/-g is invalid\n");
        return false;
      }
      option->gamma = *ret;

    } else if (check_option(opt, "--scene", "-s")) {
      auto ret = util::str2int(arg);
      if (!ret) {
        fprintf(stderr, "The argument of --scene/-s is invalid\n");
        return false;
      }
      option->scene_id = *ret;
    } else {
      fprintf(stderr, "Unknown option: %s\n", *argv);
      return false;
    }

    argc--;
    argv++;
  }

  return true;
}

Vec3F ray_color(Ray const &ray, rt::Color const &background, Shape const &world,
                int max_depth)
{
  if (max_depth <= 0) return {0, 0, 0};

  HitRecord record;
  if (world.hit(ray, 0.001, inf, record)) {
    rt::Color attenuation;
    Ray out_ray;
    ScatterRecord s_rec;
    auto emitted = record.material->emitted(record, record.u, record.v, record.p);
    if (record.material->scatter(ray, record, &attenuation, &out_ray, &s_rec)) {
      auto area = 200 * 200;
      auto light_surface_p =
          Point3F(random_double(-100, 100), 554, random_double(-378, -178));
      auto hit_to_light = light_surface_p - record.p;
      auto distance_squared = hit_to_light.length_squared();
      auto unit_hit_to_light = hit_to_light.normalize();
      if (dot(unit_hit_to_light, record.normal) < 0) return emitted;
      auto cos_theta = fabs(dot(Vec3F(0, -1, 0), -unit_hit_to_light));
      if (cos_theta < epsilon) return emitted;
      auto pdf = distance_squared / (cos_theta * area);
      s_rec.pdf = pdf;
      out_ray.set_direction(unit_hit_to_light);
#if 1
      return (attenuation / pi) *
                 ray_color(out_ray, background, world, max_depth - 1) *
                 dot(out_ray.direction().normalize(), record.normal) / s_rec.pdf +
             emitted;
#else
      return attenuation *
             ray_color(out_ray, background, world, max_depth - 1) + emitted;
#endif
    }
    return emitted;
  }

#if 0
  auto unit_direction = ray.direction().normalize();
  // y可能是负值
  // [-1, 1] ==> [0, 1]
  auto t = 0.5 * (unit_direction.y + 1.0);
  return lerp(Vec3F(1.0, 1.0, 1.0), Vec3F(0.5, 0.7, 1.0), t);
#endif
  return background;
}

img::Color compute_color(Vec3F const &c, int sample_per_pixel, double gamma_exp)
{
  auto scale = 1. / sample_per_pixel;
  auto rgb = c * scale;
  rgb.x = std::pow(rgb.x, gamma_exp);
  rgb.y = std::pow(rgb.y, gamma_exp);
  rgb.z = std::pow(rgb.z, gamma_exp);

  img::Color color(rgb);
  return color;
}

void Option::DebugPrint() const
{
  // Log the command line arguments for debugging
  printf("path = %s\n", path);
  printf("sample_per_pixel = %d\n", sample_per_pixel);
  printf("thread_num = %d\n", thread_num);
  printf("image_height = %d\n", image_height);
  printf("gamma = %d\n", gamma);
}

void setup_scene(ShapeList &world)
{
  // setup textures
  auto checker_texture =
      std::make_shared<CheckerTexture>(rt::Color(0, 0, 0), rt::Color(1, 1, 1));
  // Setup materia1, 1, 1l
  auto material_ground = std::make_shared<Lambertian>(checker_texture);
  auto material_center = std::make_shared<Lambertian>(rt::Color(0.7, 0.3, 0.3));
  // auto material_center = std::make_unique<Dielectric>(1.5);
  // auto material_left = std::make_unique<Matal>(rt::Color(0.8, 0.8, 0.8),
  // 0.0);
  auto material_left = std::make_shared<Dielectric>(1.5);
  auto material_right = std::make_shared<Matal>(rt::Color(0.8, 0.6, 0.2), 0.0);

  // Setup shapes
  world.add(std::make_shared<Sphere>(Point3F(0, 0, -1), 0.5, material_center));
  world.add(
      std::make_shared<Sphere>(Point3F(0, -100.5, -1), 100, material_ground));
  world.add(std::make_shared<Sphere>(Point3F(-1.0, 0, -1), 0.5, material_left));
  world.add(std::make_shared<Sphere>(Point3F(1.0, 0, -1), 0.5, material_right));
}

void setup_random_scene(ShapeList &world)
{
  auto material_ground = make_shared<Lambertian>(rt::Color(0.5, 0.8, 0.5));
  world.add(make_shared<Sphere>(Point3F(0, -1000, 0), 1000, material_ground));

  for (int a = -11; a < 110; ++a) {
    for (int b = -11; b < 11; ++b) {
      auto choose_mat = random_double();
      Point3F center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

      if ((center - Point3F(4, 0.2, 0)).length() > 0.9) {
        MaterialSPtr material;
        if (choose_mat < 0.8) {
          auto albedo = rt::Color::random() * rt::Color::random();
          material = std::make_shared<Lambertian>(albedo);
        } else if (choose_mat < 0.95) {
          auto albedo = rt::Color::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          material = std::make_shared<Matal>(albedo, fuzz);
        } else {
          material = std::make_shared<Dielectric>(1.5);
        }

        world.add(make_shared<Sphere>(center, 0.2, std::move(material)));
      }
    }
  }

  world.add(
      make_shared<Sphere>(Point3F(0, 1, 0), 1.0, make_shared<Dielectric>(1.5)));
  world.add(
      make_shared<Sphere>(Point3F(-4, 1, 0), 1.0,
                          make_shared<Lambertian>(rt::Color(0.4, 0.2, 0.1))));
  world.add(
      make_shared<Sphere>(Point3F(4, 1, 0), 1.0,
                          make_shared<Matal>(rt::Color(0.7, 0.6, 0.5), 0.0)));
}

void setup_light_scene(ShapeList &world)
{
  auto light_texture = make_shared<SolidTexture>(rt::Color(16, 4, 4));
  auto diffuse_light_material = make_shared<DiffuseLight>(light_texture);
  auto rect_light = make_shared<XyRect>(3, 5, 1, 3, -2, diffuse_light_material);
  world.add(std::move(rect_light));
  world.add(make_shared<Sphere>(Point3F(0, 7, 0), 1, diffuse_light_material));
  world.add(make_shared<Sphere>(
      Point3F(0, 2, 0), 2, make_shared<Lambertian>(rt::Color(0.4, 0.6, 0.9))));
  world.add(
      make_shared<Sphere>(Point3F(0, -1000, 0), 1000,
                          make_shared<Lambertian>(rt::Color(0.3, 0.2, 0.8))));
}

void setup_image_scene(ShapeList &world)
{
  auto earth_texture = make_shared<ImageTexture>("img/earthmap.jpg");
  std::cout << *earth_texture << "\n";
  world.add(make_shared<Sphere>(Point3F(0, 0, 0), 2,
                                make_shared<Lambertian>(earth_texture)));
}

void setup_cornellbox(ShapeList &world)
{
  auto red = make_shared<Lambertian>(rt::Color(.65, .05, .05));
  auto green = make_shared<Lambertian>(rt::Color(.12, .65, .45));
  auto white = make_shared<Lambertian>(rt::Color(.75, .75, .75));
  auto light = make_shared<DiffuseLight>(rt::Color(16, 16, 16));

  world.add(make_shared<YzRect>(0, 556, -556, 0, -278, green));   // left
  world.add(make_shared<YzRect>(0, 556, -556, 0, 278, red));      // right
  world.add(make_shared<XyRect>(-278, 278, 0, 556, -556, white)); // front face
  world.add(make_shared<XzRect>(-278, 278, -556, 0, 0, white));   // bottom
  world.add(make_shared<XzRect>(-278, 278, -556, 0, 556, white)); // top
  world.add(make_shared<FlipFace>(XzRect::create_based_mid(0, 200, -278, 200, 554, light)));

  ShapeSPtr box1 = 
      make_shared<Box>(Point3F(0, 0, 0),
                       Point3F(165, 330, 165), white); // left

  ShapeSPtr box2 =
      make_shared<Box>(Point3F(0, 0, 0),
                       Point3F(165, 165, 165), white); // right
  
  box1 = make_shared<Rotate>(std::move(box1), Degree{.y = 18});
  box2 = make_shared<Rotate>(std::move(box2), Degree{.y = -15});
  box1 = make_shared<Translate>(std::move(box1), Vec3F{-152, 0, -460});
  box2 = make_shared<Translate>(std::move(box2), Vec3F{17, 0, -230});
  world.add(box1);
  world.add(box2);
}
