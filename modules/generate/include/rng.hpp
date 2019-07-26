/**
  @file RNG.hpp
  @brief Namespace containing all random methods, including the engine and
distributions. if you want to share between threads. Check the thread_local
attribute, or a vector of seeds. Seed is generated using random_device (some
kind of external source of seed). STATUS: NO multi-thread safety tested.
 http://www.open-std.org/JTC1/sc22/wg21/docs/papers/2013/n3551.pdf
 http://www.cplusplus.com/reference/random/
  @author Pablo Hernandez-Cerdan
  @version 1.0
**/

#ifndef RNG_UTILS_HPP
#define RNG_UTILS_HPP

#include <array>
#include <boost/math/constants/constants.hpp> // for pi definition
#include <cmath>                              // for std::acos();
#include <random>
/**
 * Random Number Generator (RNG) namespace.
 * Static functions, not instance required. @ref RNG::engine()
 * @sa AboutRNG
 */
namespace RNG {

// double const static pi=4.0*std::atan(1.); ///< Custom pi definition.
/**  Boost definition -simple double. You can template it with long double. */
double const static pi = boost::math::constants::pi<double>();
/**Engine (uniform distribution random generator), using Marsenne generator.
 * Engine is static thread_local because it returns that kind of variable.
 * Do you want to tag static and thread_local the uniform_real_distribution?
 * @return engine e
 */
inline std::mt19937 &engine() {
    /// seed generation
    static thread_local std::random_device rdev{};
    /// engine instantiation: e
    static thread_local std::mt19937 e{rdev()};
    // static thread_local std::mt19937 e{4342};
    return e;
}

/// Randomize reseed the input engine with a random generate seed.
inline void randomize_engine(std::mt19937 &eng) {
    std::random_device rd{};
    eng.seed(rd());
}

/// Uniform random distribution from double [0.0,1)
inline double rand01() {
    static thread_local std::uniform_real_distribution<double> uid(0.0, 1.0);
    return uid(engine());
}
/**
 * Return 1 with probability p
 * @param p must be lesser or equal than 1
 * @return 1 with probability p, 0 if not.
 */
inline bool random_bool(const double p) { return (rand01() < p) ? 1 : 0; }
/**
 * Uniform random distribution from double [min,max)
 * @param min
 * @param max
 * @return double between [min,max)
 */
inline double rand_range(const double &min, const double &max) {
    // note that inside function static variables doesn't interfer if they have
    // the same name
    std::uniform_real_distribution<double> uid(min, max);
    return uid(engine());
}
/**
 *  Uniform random distribution from int [min,max]
 * @param min
 * @param max
 * @return int from [min,max]
 */
inline int rand_range_int(const int &min, const int &max) {
    // note that inside function static variables doesn't interfer if they have
    // the same name
    std::uniform_int_distribution<int> uid(min, max);
    return uid(engine());
}
/**
 * return random position from (0,0,0) to domain
 * @param domain :array<double,3>
 * @return array from 0,0,0 to domain
 */
inline std::array<double, 3> random_pos(const std::array<double, 3> &domain) {
    return {rand_range(0., domain[0]), rand_range(0., domain[1]),
            rand_range(0., domain[2])};
}
/**
 * Random number from [0, pi)
 * @return double from [0, pi)
 */
inline double rand_pi() {
    static thread_local std::uniform_real_distribution<double> uid(0.0, pi);
    return uid(engine());
}
/**
 * Random number from [0, 2pi)
 * @return double from [0, 2pi)
 */
inline double rand_2pi() {
    static thread_local std::uniform_real_distribution<double> uid(0.0, 2 * pi);
    return uid(engine());
}
/** @brief randompos generation; domain: surface of sphere of radius r.
 * @param r: max modulus available
 * @return a 3darray v with |v| = r, and random orientation
 */
inline std::array<double, 3> random_orientation(const double &r) {
    // Math notation: phi=[0,pi], theta=[0,2pi]
    double phi = rand_pi(), theta = rand_2pi();
    return {r * sin(phi) * cos(theta), r * sin(phi) * sin(theta), r * cos(phi)};
}
/**
 * Lognormal distribution variable.
 * @param mean log of variable
 * @param s variance of that log.
 * @return double from lognormal distribution characterized by mean and s.
 */
inline double random_lognormal(const double &mean, const double &s) {
    std::lognormal_distribution<double> lnd(mean, s);
    return lnd(engine());
}

/**
 * Random generator for a geometric distribution
 *
 * @param q
 *
 * @return integer
 */
inline int random_geometric(const double &q) {
    std::geometric_distribution<int> gd(q);
    return gd(engine());
}
/**
 * Random generator from a Geometric shifted (=3) distribution.
 * /f$ f(p)=q(1-q)^(p-shift) /f$
 * @param q = 1 /(Z-2), where Z is the average valence.
 * @return integer (p:valence) + shift=3, Shift represent the min valence
 * available.
 */
inline int random_geometric_shifted3(const double &q) {
    // f(p)=q(1-q)^(p-shift)
    // p: valence = [3,inf)
    std::geometric_distribution<int> gd(q);
    return gd(engine()) + 3;
}
/**
 * Random generator from a Geometric shifted distribution.
 * /f$ f(p)=q(1-q)^(p-shift) /f$
 * @param q = 1 /(Z-2), where Z is the average valence.
 * @param shift  integer representing the min valence availabl
 * @return integer (p:valence) + shift.
 */
inline int random_geometric_shifted(const double &q, const int &shift) {
    // p: valence = [3,inf)
    std::geometric_distribution<int> gd(q);
    return gd(engine()) + shift;
}
/**
 * Homemade random generator from a Geometric shifted distribution. The
 * converstion to int is truncated. Test that this is equivalent to the other
 * definition.
 * @param p
 * @param shift
 * @return int from the geometric shifted distribution
 */
inline int random_geometric_shifted_custom(const double &p, const int &shift) {
    return static_cast<int>(log(1. - rand01()) / log(1 - p)) + shift;
}

} // namespace RNG

#endif // RNG_UTILS_HPP

/** @page AboutRNG Random Number Generator Notes
 * Random Number Generator (RNG)
 *
 * Typically, you never create any RNG instance in your
 * program. One instance will be created automatically, and
 * that instance will be used for all random number
 * generation through the static methods of this class.
 */
