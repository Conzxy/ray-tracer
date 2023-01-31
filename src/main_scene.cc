#include "main_scene.hh"

#include <memory>

#include "accelerate/bvh_node.hh"
#include "material/dielectric.hh"
#include "material/diffuse_light.hh"
#include "material/lambertian.hh"
#include "material/matal.hh"
#include "shape/box.hh"
#include "shape/flip_face.hh"
#include "shape/rect.hh"
#include "shape/rotate.hh"
#include "shape/sphere.hh"
#include "shape/translate.hh"
#include "shape/constant_medium.hh"
#include "texture/checker_texture.hh"
#include "texture/image_texture.hh"
#include "texture/solid_texture.hh"

using namespace util;
using namespace img;
using namespace rt;
using namespace gm;
using namespace std;

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

void setup_cornellbox(ShapeList &world, ShapeSPtr &lights)
{
  auto red = make_shared<Lambertian>(rt::Color(.65, .05, .05));
  auto green = make_shared<Lambertian>(rt::Color(.12, .65, .45));
  auto white = make_shared<Lambertian>(rt::Color(.75, .75, .75));
  auto light = make_shared<DiffuseLight>(rt::Color(15, 15, 15));

  world.add(make_shared<YzRect>(0, 556, -556, 0, -278, green));   // left
  world.add(make_shared<YzRect>(0, 556, -556, 0, 278, red));      // right
  world.add(make_shared<XyRect>(-278, 278, 0, 556, -556, white)); // front face
  world.add(make_shared<XzRect>(-278, 278, -556, 0, 0, white));   // bottom
  world.add(make_shared<XzRect>(-278, 278, -556, 0, 556, white)); // top
  auto light_rect = make_shared<FlipFace>(
      XzRect::create_based_mid(0, 200, -278, 200, 554, light));
  world.add(light_rect);
  
  MaterialSPtr box1_mat = nullptr;
#if 1
  box1_mat = white;
#else
  auto mirror = make_shared<Matal>(rt::Color(1, 1, 1), 0);
  box1_mat = mirror;
#endif
  ShapeSPtr box1 = make_shared<Box>(Point3F(0, 0, 0), Point3F(165, 330, 165),
                                    box1_mat); // left

  ShapeSPtr box2 = make_shared<Box>(Point3F(0, 0, 0), Point3F(165, 165, 165),
                                    white); // right

  auto light_list = make_shared<ShapeList>();
  light_list->add(light_rect);

  box1 = make_shared<Rotate>(std::move(box1), Degree{.y = 18});
  box2 = make_shared<Rotate>(std::move(box2), Degree{.y = -15});
  box1 = make_shared<Translate>(std::move(box1), Vec3F{-152, 0, -460});
  box2 = make_shared<Translate>(std::move(box2), Vec3F{17, 0, -230});
#define CORNELLBOX_SCENE 1
#if CORNELLBOX_SCENE == 0
  world.add(make_shared<ConstantMedium>(std::move(box1), 0.01, Color{0, 0, 0}));
  world.add(make_shared<ConstantMedium>(std::move(box2), 0.01, Color{1, 1, 1}));
#elif CORNELLBOX_SCENE == 1
  auto glass = make_shared<Dielectric>(1.5);
  auto sphere = make_shared<Sphere>(Point3F(107, 90, -230), 90, glass);
  world.add(sphere);
  light_list->add(sphere);
  world.add(box1);
#elif CORNELLBOX_SCENE == 2
  world.add(box1);
  world.add(box2);
#endif

  lights = std::move(light_list);

}

void setup_cornellbox2(ShapeList &world, ShapeSPtr &lights)
{
  auto red = make_shared<Lambertian>(rt::Color(.65, .05, .05));
  auto white = make_shared<Lambertian>(rt::Color(.73, .73, .73));
  auto green = make_shared<Lambertian>(rt::Color(.12, .45, .15));
  auto light = make_shared<DiffuseLight>(rt::Color(15, 15, 15));

  world.add(make_shared<YzRect>(0, 555, 0, 555, 555, green));
  world.add(make_shared<YzRect>(0, 555, 0, 555, 0, red));
  world.add(make_shared<FlipFace>(
      make_shared<XzRect>(213, 343, 227, 332, 554, light)));
  world.add(make_shared<XzRect>(0, 555, 0, 555, 555, white));
  world.add(make_shared<XzRect>(0, 555, 0, 555, 0, white));
  world.add(make_shared<XyRect>(0, 555, 0, 555, 555, white));

  // shared_ptr<Material> aluminum =
  //     make_shared<Matal>(rt::Color(0.8, 0.85, 0.88), 0.0);
  shared_ptr<Shape> box1 =
      make_shared<Box>(Point3F(0, 0, 0), Point3F(165, 330, 165), white);
  box1 = make_shared<Rotate>(box1, Degree{.y = 15});
  box1 = make_shared<Translate>(box1, Vec3F(265, 0, 295));
  world.add(box1);

  auto glass = make_shared<Dielectric>(1.5);
  world.add(make_shared<Sphere>(Point3F(190, 90, 190), 90, glass));

  auto light_list = make_shared<ShapeList>();
  light_list->add(make_shared<XzRect>(213, 343, 227, 332, 554, nullptr));
  light_list->add(make_shared<Sphere>(Point3F(190, 90, 190), 90, nullptr));
  lights = std::move(light_list);
}
