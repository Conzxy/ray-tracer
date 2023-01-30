#ifndef RT_MAIN_SCENE_HH__
#define RT_MAIN_SCENE_HH__

#include "shape/shape_list.hh"

void setup_scene(rt::ShapeList &world);
void setup_random_scene(rt::ShapeList &world);
void setup_light_scene(rt::ShapeList &world);
void setup_image_scene(rt::ShapeList &world);
void setup_cornellbox(rt::ShapeList &world, rt::ShapeSPtr &lights);
void setup_cornellbox2(rt::ShapeList &world, rt::ShapeSPtr &lights);

#endif
