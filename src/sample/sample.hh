#ifndef SAMPLE_SAMPLE_HH__
#define SAMPLE_SAMPLE_HH__

#include "../gm/vec.hh"

namespace rt {

Vec3F cosine_direction_sample();

Vec3F sphere_direction_sample(double radius, double distance_squared);

}

#endif