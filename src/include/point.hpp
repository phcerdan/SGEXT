#ifndef POINT_HPP
#define POINT_HPP
#include "array_utilities.hpp"
#include <string>
#include <iostream>
#include <sstream>
namespace SG {

    template<unsigned int VDimension>
    struct Point
    {
        using Self = Point<VDimension>;
        using Array = ArrayUtilities::Array3D;
        Array m_InternalArray;
        // ======================= Mirror the access pattern behavior of the std::array class
        /**
         * Mirror the std::array type aliases and member function
         * so that the Index class can be treated as a container
         * class in a way that is similar to the std::array.
         */
        using value_type = Array::value_type;
        using reference = value_type &;
        using const_reference = const value_type &;
        using iterator = value_type *;
        using const_iterator = const value_type *;
        using size_type = unsigned int;
        using difference_type = std::ptrdiff_t;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        /**
         * Mirror behavior of the std::array manipulations
         * See std::array for documentation on these methods
         */
        void assign(const value_type & newValue)
        {
            std::fill_n(begin(), size(), newValue);
        }

        void swap(Self & other)
        {
            std::swap_ranges(begin(), end(), other.begin() );
        }

        iterator begin()
        {
            return iterator(&m_InternalArray[0]);
        }

        const_iterator begin() const
        {
            return const_iterator(&m_InternalArray[0]);
        }

        iterator end()
        {
            return iterator(&m_InternalArray[VDimension]);
        }

        const_iterator end() const
        {
            return const_iterator(&m_InternalArray[VDimension]);
        }

        reverse_iterator rbegin()
        {
            return reverse_iterator(end() );
        }

        const_reverse_iterator rbegin() const
        {
            return const_reverse_iterator(end() );
        }

        reverse_iterator rend()
        {
            return reverse_iterator(begin() );
        }

        const_reverse_iterator rend() const
        {
            return const_reverse_iterator(begin() );
        }

        constexpr size_type size() const
        {
            return VDimension;
        }

        value_type& operator[](const std::size_t index)
        {
            return m_InternalArray[index];
        };
        const value_type& operator[](const std::size_t index) const
        {
            return m_InternalArray[index];
        };
        // Self& operator=(const Array& input_array)
        // {
        //     m_InternalArray = input_array;
        //     return *this;
        // };

    };
// ======================= Mirror the access pattern behavior of the std::array class
// Array comparisons.
template <unsigned int VDimension>
inline bool
operator==(const Point<VDimension> & one, const Point<VDimension> & two)
{
  return std::equal(one.begin(), one.end(), two.begin() );
}

template <unsigned int VDimension>
inline bool
operator!=(const Point<VDimension> & one, const Point<VDimension> & two)
{
  return !(one == two);
}

template <unsigned int VDimension>
inline bool
operator<(const Point<VDimension> & one, const Point<VDimension> & two)
{
  return std::lexicographical_compare(one.begin(), one.end(),
                                      two.begin(), two.end() );
}

template <unsigned int VDimension>
inline bool
operator>(const Point<VDimension> & one, const Point<VDimension> & two)
{
  return two < one;
}

template <unsigned int VDimension>
inline bool
operator<=(const Point<VDimension> & one, const Point<VDimension> & two)
{
  return !(one > two);
}

template <unsigned int VDimension>
inline bool
operator>=(const Point<VDimension> & one, const Point<VDimension> & two)
{
  return !(one < two);
}

// Specialized algorithms [6.2.2.2].
template <unsigned int VDimension>
inline void
swap(Point<VDimension> & one, Point<VDimension> & two)
{
  std::swap_ranges(one.begin(), one.end(), two.begin() );
}

std::ostream &operator<<(std::ostream &os, const SG::Point<3> & p) {
    os << "[";
    for( unsigned int i = 0; i < 2; ++i )
    {
        os << p[i] << ", ";
    }
    os << p[2];
    os << "]";
    return os;
}

std::istream &operator>>(std::istream &is, SG::Point<3> & p)
{
    auto & x = p[0];
    auto & y = p[1];
    auto & z = p[2];
    is >> x >> y >> z;
    return is;
}
} // end namespace
#endif
