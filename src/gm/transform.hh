#ifndef GM_TRANSFORM_HH__
#define GM_TRANSFORM_HH__

#include "matrix.hh"

namespace gm {

Matrix3x3F get_x_rotation_matrix(double angle);
Matrix3x3F get_y_rotation_matrix(double angle);
Matrix3x3F get_z_rotation_matrix(double angle);

}

#endif