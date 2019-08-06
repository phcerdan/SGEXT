#ifndef SG_BOUNDARY_CONDITIONS_HPP
#define SG_BOUNDARY_CONDITIONS_HPP

#include "array_utilities.hpp"

namespace ArrayUtilities {
enum class boundary_condition {
    NONE,
    PERIODIC /// PBCs
             // REFLECTIVE_WALL
};

inline std::string boundary_condition_to_string(const boundary_condition &bc) {
    if (bc == boundary_condition::NONE) {
        return "NONE";
    } else if (bc == boundary_condition::PERIODIC) {
        return "PERIODIC";
    } else {
        throw std::domain_error(
                "boundary_condition_to_string error: unrecognized "
                "boundary_condition.");
    }
}

inline boundary_condition
string_to_boundary_condition(const std::string &bc_string) {
    if (bc_string == "NONE") {
        return boundary_condition::NONE;
    } else if (bc_string == "PERIODIC") {
        return boundary_condition::PERIODIC;
    } else {
        throw std::domain_error(
                "string_to_boundary_condition error: unrecognized "
                "string: " +
                bc_string);
    }
}

constexpr Array3D zeros3d = Array3D();
constexpr Array3D ones3d = Array3D{1.0, 1.0, 1.0};
constexpr Array3D ex = Array3D{1.0, 0.0, 0.0};
constexpr Array3D ey = Array3D{0.0, 1.0, 0.0};
constexpr Array3D ez = Array3D{0.0, 0.0, 1.0};

/** All functions/operators involving a pair of vectors must check if
 * they are closer to the particle in one of neighbor images.
 * This can be done: dx = o - x
 * if dx > size/2, then compute dx = o'' - x
 *              --------
 *  |o'      x'|o      x|o''     x''|
 *              --------
 *
 * Refs:
 * https://en.wikipedia.org/wiki/Periodic_boundary_conditions
 * https://homepage.univie.ac.at/Franz.Vesely/simsp/dx/node9.html
 */

/**************************************/
/******* MINUS ********/
/**************************************/

inline Array3D
minus_with_boundary_condition_periodic(const Array3D &lhs,
                                       const Array3D &rhs,
                                       const Array3D &size_box,
                                       const Array3D &size_box_inverse) {
    Array3D result;
    for (int i = 0; i != 3; i++) {
        result[i] = lhs[i] - rhs[i];
        result[i] -= size_box[i] * nearbyint(result[i] * size_box_inverse[i]);
    }
    return result;
}
/**
 * Difference with box of size 1.0
 */
inline Array3D minus_with_boundary_condition_periodic(const Array3D &lhs,
                                                      const Array3D &rhs) {
    Array3D result;
    for (int i = 0; i != 3; i++) {
        result[i] = lhs[i] - rhs[i];
        result[i] -= nearbyint(result[i]);
    }
    return result;
}

/*************************************************/
/* CLOSEST IMAGE IN PERIODIC BOUNDARY CONDITIONS */
/*************************************************/

/**
 * Returns input (rhs) or an image of input, whichever is closer to reference
 * (lhs).
 *
 * This comply with the minimum image criterion needed for Periodic Boundary
 * Conditions
 *
 * @param reference
 * @param input
 *
 * @return input_image
 */
inline Array3D closest_image_from_reference(const Array3D &reference,
                                            const Array3D &input) {
    return minus(reference,
                 minus_with_boundary_condition_periodic(reference, input));
}

/**************************************/
/******* SUM ********/
/**************************************/

inline Array3D
plus_with_boundary_condition_periodic(const Array3D &lhs,
                                      const Array3D &rhs,
                                      const Array3D &size_box,
                                      const Array3D &size_box_inverse) {
    Array3D result;
    for (int i = 0; i != 3; i++) {
        result[i] = lhs[i] + rhs[i];
        result[i] -= std::floor(result[i] * size_box_inverse[i]) * size_box[i];
    }
    return result;
}

/**
 * Sum with box of size 1.0
 */
inline Array3D plus_with_boundary_condition_periodic(const Array3D &lhs,
                                                     const Array3D &rhs) {
    Array3D result;
    for (int i = 0; i != 3; i++) {
        result[i] = lhs[i] + rhs[i];
        result[i] -= std::floor(result[i]);
    }
    return result;
}

/**************************************/
/******* DISTANCE ********/
/**************************************/
/** closest_image_from_reference can be used to convert x1 to x1_image, and use
 * the regular distance function between x0 and x1_image.
 */

inline Array3D::value_type
distance_with_boundary_condition_periodic(const Array3D &lhs,
                                          const Array3D &rhs,
                                          const Array3D &size_box,
                                          const Array3D &size_box_inverse) {
    return norm(minus_with_boundary_condition_periodic(lhs, rhs, size_box,
                                                       size_box_inverse));
}
inline Array3D::value_type
distance_with_boundary_condition_periodic(const Array3D &lhs,
                                          const Array3D &rhs) {
    return norm(minus_with_boundary_condition_periodic(lhs, rhs));
}

/**************************************/
/******* CROSS PRODUCT ********/
/**************************************/
/**************************************/
/******* DOT PRODUCT ********/
/**************************************/
/** Use closest_image_from_reference to conver x1 into x1_image, and use the
 * regular cross_product and dot_products between x0 and x1_image.
 */

} // namespace ArrayUtilities
#endif
