/**
  @file ArrayUtilities.hpp
  @brief Array manipulation methods and other utility functions.
  Created December 2017
  @author Pablo Hernandez-Cerdan
  @license MPL
  @version 1.0
  */

#ifndef ARRAY_UTILITIES_HPP_
#define ARRAY_UTILITIES_HPP_

#include <array>
#include <cmath>
#include <numeric>
#include <limits>
#include <sstream>

namespace ArrayUtilities {
using Array3D = std::array<double, 3>;

/**
 * CrossProduct axb
 *
 * @param a lhs input array
 * @param b rhs input array
 *
 * @return array with cross_product axb
 */
inline Array3D cross_product(const Array3D &a, const Array3D &b) {
    Array3D::value_type s0, s1, s2;
    s0 = a[1] * b[2] - a[2] * b[1];
    s1 = a[2] * b[0] - a[0] * b[2];
    s2 = a[0] * b[1] - a[1] * b[0];
    return Array3D{{s0, s1, s2}};
}

/**
 * dot_product of array
 *
 * @param a
 * @param b
 *
 * @return dot_product of a b
 */
inline Array3D::value_type dot_product(const Array3D &a, const Array3D &b) {
    return std::inner_product(a.begin(), a.end(), b.begin(), 0.0);
}

/**
 * L2-norm of input array ||a|| = sqrt(ai * ai)
 * @param a input array
 * @return norm value
 */
inline Array3D::value_type norm(const Array3D &a) {
    return sqrt(dot_product(a, a));
}

/**
 * Angle between a,b using atan2.
 * Faster and more reliable than acos.Also avoid to test for non zero.
 * @param a
 * @param b
 *
 * From cpp std:
 * If no errors occur, the arc tangent of y/x (atan( y, x)) in the range [-pi ;
 * +pi] radians, is returned.
 * @return angle
 */
inline Array3D::value_type angle(const Array3D &a, const Array3D &b) {
    // return std::acos(NetworkUtilities::aprox_if_necessary<double>(a||b,1.));
    return std::atan2(norm(cross_product(a, b)), dot_product(a, b));
}

/**
 * Projection of array "a" into array "into"
 *
 * @param a
 * @param into
 *
 * @return projection
 */
inline Array3D projection(const Array3D &a, const Array3D &into) {
    const auto into_squared = dot_product(into, into);
    if( into_squared > std::numeric_limits<double>::epsilon() ) {
        const auto term = dot_product( into, a ) / into_squared;
        return Array3D{ into[0] * term,
                        into[1] * term,
                        into[2] * term };
    } else {
        return Array3D{0,0,0};
   }
}

/**
 * Orthogonal_component of array "a" into array "into"
 * The orthogonal component is the array: a - projection(a,into)
 *
 * @param a
 * @param into
 *
 * @return orthogonal_component
 */
inline Array3D orthogonal_component(const Array3D &a, const Array3D &into) {
    const auto into_squared = dot_product(into, into);
    if( into_squared > std::numeric_limits<double>::epsilon() ) {
        const auto term = dot_product( into, a ) / into_squared;
        return Array3D { a[0] - into[0] * term,
                         a[1] - into[1] * term,
                         a[2] - into[2] * term };
    } else {
        return Array3D{0,0,0};
   }
}

/**
 * Sum of arrays (each dimension)
 *
 * @param lhs left
 * @param rhs right
 *
 * @return array with a+b
 */
inline Array3D plus(const Array3D &lhs, const Array3D &rhs) {
    return {{lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]}};
}

/**
 * Sum a scalar to array.
 *
 * @param lhs array
 * @param scalar double
 *
 * @return array + scalar
 */
inline Array3D plus_scalar(const Array3D &lhs,
                           const Array3D::value_type &scalar) {
    return plus(lhs, Array3D{{scalar, scalar, scalar}});
}

/**
 * negation of array: -lhs
 *
 * @param lhs input array
 *
 * @return negation of array
 */
inline Array3D negate(const Array3D &lhs) {
    return {{-lhs[0], -lhs[1], -lhs[2]}};
}
inline void negate_in_place(Array3D &lhs) {
   lhs[0] = -lhs[0]; lhs[1] = -lhs[1]; lhs[2] = -lhs[2];
}

/**
 * Difference between arrays: lhs - rhs
 * minus(a,b) = a - b;
 *
 * @param lhs left
 * @param rhs right
 *
 * @return array with lhs - rhs
 */
inline Array3D minus(const Array3D &lhs, const Array3D &rhs) {
    return {{lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2]}};
}

/**
 * Array minus a scalar
 *
 * @param lhs array
 * @param scalar number
 *
 * @return array - scalar
 */
inline Array3D minus_scalar(const Array3D &lhs,
                            const Array3D::value_type &scalar) {
    return plus_scalar(lhs, -scalar);
}

inline Array3D product_scalar(const Array3D &lhs,
                              const Array3D::value_type &scalar) {
    return {{lhs[0] * scalar, lhs[1] * scalar, lhs[2] * scalar}};
}

/**
 * Distance between two arrays.
 * @param lhs
 * @param rhs
 * @return double distance
 */
inline double distance(const Array3D &lhs, const Array3D &rhs) {
    return norm(minus(lhs, rhs));
}

inline Array3D::value_type cos_director(const Array3D &lhs,
                                        const Array3D &rhs) {
    return std::cos(angle(lhs, rhs));
}

/**
 * Helper to transform array to string, values are separated by space.
 * array<T,3> a{1,2,3}
 * std::string s = to_string(a)
 * std::cout << s << std::endl;
 * 1 2 3
 *
 * @param a input array
 *
 * @return string with array content, separated by space
 */
inline std::string to_string(const Array3D &a, bool comma_separated = false) {
    std::ostringstream ss;
    auto a_size = a.size();
    for (size_t i = 0; i < a_size; ++i) {
        ss << a[i];
        if (i != a_size - 1) {
           if(comma_separated) ss << ", ";
           else ss << " ";
        }
    }
    return ss.str();
}
} // namespace ArrayUtilities
#endif
