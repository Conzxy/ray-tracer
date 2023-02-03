#include <cstdio>
#include <string_view>
#include <thread>
#include <random>

#define USE_STB_IMAGE_WRITE 0

#if USE_STB_IMAGE_WRITE
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#endif

#include "main_scene.hh"
#include "option.hh"
#include "gm/util.hh"
#include "rt/ray.hh"
#include "rt/hit_record.hh"
#include "rt/scatter_record.hh"
#include "rt/camera.hh"
#include "img/color.hh"
#include "img/tga_image.hh"
#include "material/material.hh"
#include "sample/cosine_pdf.hh"
#include "sample/mixture_pdf.hh"
#include "sample/shape_pdf.hh"
#include "util/atomic_counter.h"
#include "util/progress_bar.hh"
#include "util/random.hh"

#define MAX_DEPTH 50

using namespace rt;
using namespace std;
using namespace util;
using namespace gm;
using namespace img;

namespace ktm = std::chrono;

Vec3F ray_color(Ray const &ray, rt::Color const &background, Shape const &shape,
                ShapeSPtr const &lights, int max_depth);

img::Color compute_color(Vec3F const &c, int sample_per_pixel,
                         double gamma_exp);

#if USE_STB_IMAGE_WRITE
bool write_tga_by_stb(TgaImage const& image, char const *path)
{
  stbi_flip_vertically_on_write(true);
  if (!stbi_write_tga(path,
    image.width(), image.height(), image.bytes_per_pixel(), image.data())) {
    return false;
  }
  return true;
}
#endif

int main(int argc, char *argv[])
{
  /****** Setting Options *****/
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
  ShapeSPtr lights = nullptr;
  
  // Setup Scene
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
      aspect_ratio = 1;
      lookfrom = Point3F(0, 278, 800);
      lookat = Point3F(0, 278, 0);
      fov = 40;
      setup_cornellbox(world, lights);
    } break;

    case 5: {
      aspect_ratio = 1.0 / 1.0;
      lookfrom = Point3F(278, 278, -800);
      lookat = Point3F(278, 278, 0);
      fov = 40;
      setup_cornellbox2(world, lights);
      // background = { 187 / 255., 1, 1 };
    } break;
  }

  Camera camera(lookfrom, lookat, aspect_ratio, fov, 1);
  camera.set_aperture(0.0);
  camera.DebugPrint();

  // Setup image
  int image_width = aspect_ratio * option.image_height;
  TgaImage image(image_width, option.image_height);
  
  // Setup thread and slice
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
  
    // Setup main render loop
    thrs.emplace_back(std::thread(
      [start, end, &option, gamma_exp, &background,
      &world, &camera, &image,
      &current_complete_sample, &lights]() {
        for (int j = start; j >= end; --j) {
          for (int i = 0; i < image.width(); ++i) {
            // propertion
            Vec3F color_prop(0, 0, 0);
            for (int k = 0; k < option.sample_per_pixel; ++k) {
              auto offset = double(k) / option.sample_per_pixel;
              auto u = double(i + offset) / (image.width() - 1);
              auto v = double(j + offset) / (image.height() - 1);

              auto ray = camera.ray(u, v);
              color_prop += ray_color(ray, background, world, lights, MAX_DEPTH);
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
  
  // Set and Update progress bar indicator
  while (1) {
    auto current_value = current_complete_sample.GetValue();
    if (current_value >= total_sample) break;
    update_progress_bar('#', current_value / double(total_sample) * 100);
    std::this_thread::sleep_for(1s);
  }
  update_progress_bar('#', 100);

  for (auto& thr : thrs) {
    thr.join();
  }

  auto end_of_render = ktm::steady_clock::now();
  ktm::duration<double> cost_time_of_render = end_of_render - start_of_render;
  printf("\nThe consume time of render is %.3lf sec\n",
    cost_time_of_render.count());
  fflush(stdout);
  
#if USE_STB_IMAGE_WRITE
  if (write_tga_by_stb(image, option.path)) {
    return EXIT_FAILURE;
  }
#else
  if (!image.WriteTo(option.path)) {
    return EXIT_FAILURE;
  }
#endif
  return EXIT_SUCCESS;
}

double RR_probability = 0.9999;

#define RR_COLOR(c) (c) / RR_probability

rt::Color ray_color(Ray const &ray, rt::Color const &background, Shape const &world,
                ShapeSPtr const &lights, int max_depth)
{
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_real_distribution<> urd(0., 1.);
  
#if 0
  RR_probability = 1.;
  if (max_depth <= 0) return {0, 0, 0};
#else
  if (urd(gen) > RR_probability || max_depth <= 0) return {0, 0, 0};
#endif

  HitRecord record;
  if (!world.hit(ray, 0.001, inf, record)) {
    return RR_COLOR(background);
  }

  ScatterRecord scatter_rec;
  auto emitted = record.material->emitted(record, record.u, record.v, record.p);

  if (!record.material->scatter(ray, record, scatter_rec)) {
    return RR_COLOR(emitted);
  }

  if (scatter_rec.is_specular) {
    return RR_COLOR(scatter_rec.attenuation *
           ray_color(scatter_rec.specular_ray, background, world, lights,
                     max_depth - 1));
  }
  ShapePdf light_pdf(lights.get(), record.p);
  MixturePdf mixture_pdf(scatter_rec.pdf.get(), &light_pdf);
  Pdf *used_pdf = nullptr;
#define PDF_TEST 3
#if PDF_TEST == 1
  used_pdf = scatter_rec.pdf.get();
#elif PDF_TEST == 2
  used_pdf = &light_pdf;
#elif PDF_TEST == 3
  used_pdf = &mixture_pdf;
#else
  used_pdf = scatter_rec.pdf.get();
#endif
  Ray out_ray(record.p, used_pdf->generate());

  double pdf_value = used_pdf->value(out_ray.direction());
  if (pdf_value < epsilon) return RR_COLOR(emitted);
  const auto cosine_theta_i =
      std::max(dot(out_ray.direction().normalize(), record.normal), 0.);
  return RR_COLOR((scatter_rec.attenuation / pi) *
              ray_color(out_ray, background, world, lights, max_depth - 1) *
              cosine_theta_i / pdf_value +
          emitted);
}

img::Color compute_color(Vec3F const &c, int sample_per_pixel, double gamma_exp)
{
  auto scale = 1. / sample_per_pixel;
  auto rgb = c * scale;
  if (isnan(rgb.x)) rgb.x = 0;
  if (isnan(rgb.y)) rgb.y = 0;
  if (isnan(rgb.z)) rgb.z = 0;

  rgb.x = std::pow(rgb.x, gamma_exp);
  rgb.y = std::pow(rgb.y, gamma_exp);
  rgb.z = std::pow(rgb.z, gamma_exp);

  /*
   * 由于light的强度不限制于[0, 1]，因此rgb的比例可能大于1
   * 对于这种情况，当作1处理
   * 使用clamp()约束
   */
  img::Color color(int(256 * clamp(rgb.x, 0, 0.99999)),
                   int(256 * clamp(rgb.y, 0, 0.99999)),
                   int(256 * clamp(rgb.z, 0, 0.99999)));
#if USE_STB_IMAGE_WRITE
  std::swap(color.r, color.b);
#endif
  return color;
}

