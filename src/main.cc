#include <cstdio>

#include <string_view>
#include <thread>

#include "gm/util.hh"
#include "img/color.hh"
#include "img/tga_image.hh"
#include "material/dielectric.hh"
#include "material/lambertian.hh"
#include "material/matal.hh"
#include "rt/camera.hh"
#include "rt/hit_record.hh"
#include "rt/ray.hh"
#include "shape/shape_list.hh"
#include "shape/sphere.hh"
#include "util/str_cvt.hh"
#include "util/random.hh"

#define MAX_DEPTH 50

using namespace util;
using namespace img;
using namespace rt;
using namespace gm;
using namespace std;

namespace ktm = std::chrono;

using MaterialVector = std::vector<std::unique_ptr<Material>>;

Vec3F ray_color(Ray const &ray, Shape const &shape, int max_depth);
img::Color compute_color(Vec3F const &c, int sample_per_pixel,
                         double gamma_exp);

struct Option {
  char const *path = nullptr;
  int sample_per_pixel = 100;
  int thread_num = 8;
  int gamma = 2;
  int image_height = 400;

  void DebugPrint() const;
};

bool parse_options(int argc, char *argv[], Option *option);
void setup_scene(ShapeList &world,
                 std::vector<std::unique_ptr<Material>> &materials);
void setup_random_scene(ShapeList &world, MaterialVector &mats);

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
  Point3F lookfrom(-2., 2., 0.);
  // Point3F lookfrom(0, 0, 0);
  Point3F lookat(0., 0., -1.);
  auto focus_dist = (lookat-lookfrom).length();
  Camera camera(lookfrom, lookat, aspect_ratio, 50, focus_dist);
  camera.set_aperture(5.0);
  camera.DebugPrint();

  // Setup image
  int image_width = aspect_ratio * option.image_height;
  TgaImage image(image_width, option.image_height);

  // Setup Scene
  ShapeList world;
  std::vector<std::unique_ptr<Material>> materials;  
  setup_scene(world, materials);
  // setup_random_scene(world, materials);

  std::vector<std::thread> thrs;
  thrs.reserve(option.thread_num);

  int start = image.height() - 1;
  int slice = (image.height() - 1) / option.thread_num;
  printf("init start = %d\n", start);
  printf("slice = %d\n", slice);

  auto start_of_render = ktm::steady_clock::now();

  for (int ti = 0; ti < option.thread_num; ++ti) {
    int end = start - slice;
    if (ti == option.thread_num - 1 && end != 0) {
      end = 0;
    }

    printf("slice [%d, %d]\n", start, end);
    thrs.emplace_back(std::thread(
        [start, end, &option, gamma_exp, &world, &camera, &image]() {
          for (int j = start; j >= end; --j) {
            for (int i = 0; i < image.width(); ++i) {
              // propertion
              Vec3F color_prop(0, 0, 0);
              for (int k = 0; k < option.sample_per_pixel; ++k) {
                auto offset = double(k) / option.sample_per_pixel;
                auto u = double(i + offset) / (image.width() - 1);
                auto v = double(j + offset) / (image.height() - 1);

                auto ray = camera.ray(u, v);
                color_prop += ray_color(ray, world, MAX_DEPTH);
              }
              auto color =
                  compute_color(color_prop, option.sample_per_pixel, gamma_exp);
              image.SetPixel(i, j, color);
            }
          }
        }));

    start -= slice + 1;
  }

  for (auto &thr : thrs) {
    thr.join();
  }

  auto end_of_render = ktm::steady_clock::now();
  ktm::duration<double> cost_time_of_render = end_of_render - start_of_render;
  printf("The consume time of render is %.3lfsec\n",
         cost_time_of_render.count());

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
    } else {
      fprintf(stderr, "Unknown option: %s\n", *argv);
      return false;
    }

    argc--;
    argv++;
  }

  return true;
}

Vec3F ray_color(Ray const &ray, Shape const &shape, int max_depth)
{
  if (max_depth <= 0) return {0, 0, 0};

  HitRecord record;
  if (shape.hit(ray, 0.001, inf, record)) {
    rt::Color attenuation;
    Ray out_ray;
    if (record.material->scatter(ray, record, &attenuation, &out_ray)) {
      return attenuation * ray_color(out_ray, shape, max_depth - 1);
    } else {
      return {0, 0, 0};
    }
  }

  auto unit_direction = ray.direction().normalize();
  // y可能是负值
  // [-1, 1] ==> [0, 1]
  auto t = 0.5 * (unit_direction.y + 1.0);
  return lerp(Vec3F(1.0, 1.0, 1.0), Vec3F(0.5, 0.7, 1.0), t);
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

void setup_scene(ShapeList &world,
                 std::vector<std::unique_ptr<Material>> &materials)
{
  // Setup material
  auto material_ground = std::make_unique<Lambertian>(rt::Color(0.8, 0.8, 0));
  auto material_center = std::make_unique<Lambertian>(rt::Color(0.7, 0.3, 0.3));
  // auto material_center = std::make_unique<Dielectric>(1.5);
  // auto material_left = std::make_unique<Matal>(rt::Color(0.8, 0.8, 0.8),
  // 0.0);
  auto material_left = std::make_unique<Dielectric>(1.5);
  auto material_right = std::make_unique<Matal>(rt::Color(0.8, 0.6, 0.2), 0.0);

  // Setup shapes
  world.add(
      std::make_shared<Sphere>(Point3F(0, 0, -1), 0.5, material_center.get()));
  world.add(std::make_shared<Sphere>(Point3F(0, -100.5, -1), 100,
                                     material_ground.get()));
  world.add(
      std::make_shared<Sphere>(Point3F(-1.0, 0, -1), 0.5, material_left.get()));
  world.add(
      std::make_shared<Sphere>(Point3F(1.0, 0, -1), 0.5, material_right.get()));

  materials.push_back(std::move(material_ground));
  materials.push_back(std::move(material_center));
  materials.push_back(std::move(material_left));
  materials.push_back(std::move(material_right));
}

void setup_random_scene(ShapeList &world, MaterialVector &mats)
{
  auto material_ground = make_unique<Lambertian>(rt::Color(0.5, 0.8, 0.5));
  world.add(make_shared<Sphere>(Point3F(0, -1000, 0), 1000, material_ground.get()));
  mats.push_back(std::move(material_ground));

  for (int a = -11; a < 11; ++a) {
    for (int b = -11; b < 11; ++b) {
      auto choose_mat = random_double();
      Point3F center(a + 0.9 * random_double(), 0.2, b + 0.9*random_double()); 
      
      if ((center - Point3F(4, 0.2, 0)).length() > 0.9) {
          if (choose_mat < 0.8) {
            auto albedo = rt::Color::random() * rt::Color::random();
            mats.push_back(std::make_unique<Lambertian>(albedo));
          } else if (choose_mat < 0.95) {
            auto albedo = rt::Color::random(0.5, 1);
            auto fuzz = random_double(0, 0.5);
            mats.push_back(std::make_unique<Matal>(albedo, fuzz));
          } else {
            mats.push_back(std::make_unique<Dielectric>(1.5));
          }

          world.add(make_shared<Sphere>(center, 0.2, mats.back().get()));
      }
    }
  }

  mats.push_back(make_unique<Dielectric>(1.5));
  world.add(make_shared<Sphere>(Point3F(0,1,0), 1.0, mats.back().get()));
  mats.push_back(make_unique<Lambertian>(rt::Color(0.4, 0.2, 0.1)));
  world.add(make_shared<Sphere>(Point3F(-4, 1, 0), 1.0, mats.back().get()));
  mats.push_back(make_unique<Matal>(rt::Color(0.7, 0.6, 0.5), 0.0));
  world.add(make_shared<Sphere>(Point3F(4, 1, 0), 1.0, mats.back().get()));
}