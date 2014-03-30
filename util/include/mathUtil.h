/*!
 * @brief Provide utility functions of math
 * @author koturn 0;
 * @file mathUtil.h
 */
#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#ifdef _MSC_VER
// <cmath> doesn't provides M_PI even if you define _USE_MATH_DEFINES
#  define _USE_MATH_DEFINES
#  include <math.h>
#endif
#include <cmath>
#include "../../include/commonUtil/compat.h"


ATTR_PURE ATTR_NOTHROW CONSTEXPR_CXX11 ALWAYSINLINE static double
degreeToRadian(double degree) noexcept;

template<typename VType>
ATTR_PURE ATTR_NOTHROW CONSTEXPR_CXX11 ALWAYSINLINE static VType
clipping(const VType &x, const VType &min, const VType &max) noexcept;

ATTR_PURE ATTR_NOTHROW ALWAYSINLINE static double
calcDistance(const CvPoint &p1, const CvPoint &p2) noexcept;

#ifdef _MSC_VER
ATTR_PURE ATTR_NOTHROW ALWAYSINLINE static double
round(double x) noexcept;
#endif




/*!
 * @brief Clipping value
 * @param [in] x    The value you want to clipping
 * @param [in] min  Minimum value
 * @param [in] max  Maximum value
 * @return  Clipped value
 */
template<typename VType>
ATTR_PURE ATTR_NOTHROW CONSTEXPR_CXX11 ALWAYSINLINE static VType
clipping(const VType &x, const VType &min, const VType &max) noexcept
{
  return x < min ? min
    : x > max ? max
    : x;
}


/*!
 * @brief Convert degree-value to radian-value
 * @param [in] degree  A degree-value
 * @return  A radian-value which is converted from degree-value
 */
ATTR_PURE ATTR_NOTHROW CONSTEXPR_CXX11 ALWAYSINLINE static double
degreeToRadian(double degree) noexcept
{
  return degree * M_PI / 180;
}


/*!
 * @brief Calculate two-dimensional euclidean distance between first point and
 *        second point
 * @param [in] p1  First point
 * @param [in] p2  Second point
 * @return  two-dimensional euclidean distance between first point and second point
 */
ATTR_PURE ATTR_NOTHROW ALWAYSINLINE static double
calcDistance(const CvPoint &p1, const CvPoint &p2) noexcept
{
  return std::sqrt(std::pow(p1.x - p2.x, 2.0) + std::pow(p1.y - p2.y, 2.0));
}


#ifdef _MSC_VER
/*!
 * @brief Do rounded to the given number
 * @param [in]  A value you want to do rounded
 * @return  rounded value
 */
ATTR_PURE ATTR_NOTHROW ALWAYSINLINE static double
round(double x) noexcept
{
  return x > 0.0 ? std::floor(x + 0.5) : -1.0 * std::floor(std::fabs(x) + 0.5);
}
#endif


#endif  // MATH_UTIL_H
