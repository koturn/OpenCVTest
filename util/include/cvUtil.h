/*!
 * @brief Provide utility functions with OpenCV
 * @author koturn 0;
 * @file cvUtil.h
 */
#ifndef CV_UTIL_H
#define CV_UTIL_H

#include <cmath>
#include <cstring>
#include <opencv/cv.h>
#include "../../include/commonUtil/compat.h"

#if defined(WIN16) || defined(_WIN16) || defined(__WIN16) || defined(__WIN16__)   \
  || defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)  \
  || defined(WIN64) || defined(_WIN64) || defined(__WIN64) || defined(__WIN64__)
#  ifndef NOMINMAX
#    define NOMINMAX
#    define DEFINED_NOMINMAX 0
#  endif
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#    define DEFINED_WIN32_LEAN_AND_MEAN 0
#  endif
#  include <windows.h>
#  if DEFINED_NOMINMAX == 0
#    undef NOMINMAX
#  endif
#  if DEFINED_WIN32_LEAN_AND_MEAN == 0
#    undef WIN32_LEAN_AND_MEAN
#  endif
#  undef DEFINED_NOMINMAX
#  undef DEFINED_WIN32_LEAN_AND_MEAN
#  define IS_WINDOWS
#endif


//! Image size information
typedef struct {
  int    width;
  int    height;
  double wRate;
  double hRate;
  double autoRate;
} SizeInfo;

inline static void
parseSizeString(SizeInfo &sizeInfo, const char *sizeString);

ATTR_NOTHROW inline static cv::Mat
resizeImage(const cv::Mat &image, const SizeInfo &sizeInfo);

#ifdef IS_WINDOWS
ATTR_NOTHROW ATTR_PURE inline static SizeInfo
calcAutoSizeRate(const cv::Mat &image, double maxRate) noexcept;
#endif




/*!
 * @brief Parse a string used for specify resize image
 * @param [in,out] sizeInfo    Image size information
 * @param [in]     sizeString  A string used for specify resize image
 */
inline static void
parseSizeString(SizeInfo &sizeInfo, const char *sizeString)
{
  const static double EPS = 1e-8;
  int    a, b;
  double x, y;
  if (std::sscanf(sizeString, "%dx%d", &a, &b) == 2) {
    if (a < 1) {
      throw "Invalid width value is specified";
    } else if (b < 1) {
      throw "Invalid height value is specified";
    }
    SizeInfo si = {a, b, 1.0, 1.0, sizeInfo.autoRate};
    sizeInfo = si;
  } else if (std::sscanf(sizeString, "%lf%%x%lf%%", &x, &y) == 2) {
    if (std::abs(x) < EPS) {
      throw "Invalid width rate value is specified";
    } else if (std::abs(y) < EPS) {
      throw "Invalid height rate value is specified";
    }
    SizeInfo si = {0, 0, x / 100.0, y / 100.0, sizeInfo.autoRate};
    sizeInfo = si;
  } else if (std::sscanf(sizeString, "%lf%%", &x) == 1) {
    if (std::abs(x) < EPS) {
      throw "Invalid rate value is specified";
    }
    SizeInfo si = {0, 0, x / 100.0, x / 100.0, sizeInfo.autoRate};
    sizeInfo = si;
  } else if (!std::strcmp(sizeString, "auto")) {
    SizeInfo si = {-1, -1, 1.0, 1.0, sizeInfo.autoRate};
    sizeInfo = si;
  } else if (!std::strcmp(sizeString, "original")) {
    SizeInfo si = {0, 0, 1.0, 1.0, sizeInfo.autoRate};
    sizeInfo = si;
  } else {
    throw "Invalid size format";
  }
}


/*!
 * @brief Resize information by image size information
 * @param [in] image     Original image
 * @param [in] sizeInfo  Image size information
 */
ATTR_NOTHROW inline static cv::Mat
resizeImage(const cv::Mat &image, const SizeInfo &sizeInfo)
{
  SizeInfo resizedSizeInfo;
  if (sizeInfo.width < 0 && sizeInfo.height < 0) {
#ifdef IS_WINDOWS
    resizedSizeInfo = calcAutoSizeRate(image, sizeInfo.autoRate);
#else
    resizedSizeInfo = {0, 0, 1.0, 1.0, sizeInfo.autoRate};
#endif
  } else {
    resizedSizeInfo = sizeInfo;
  }
  cv::Mat resizedImage;
  cv::resize(
      image,
      resizedImage,
      cv::Size(resizedSizeInfo.width, resizedSizeInfo.height),
      resizedSizeInfo.wRate,
      resizedSizeInfo.hRate,
      cv::INTER_NEAREST);
  return resizedImage;
}


#ifdef IS_WINDOWS
/*!
 * @brief Calcurate resize ratio
 * @attension This function can be used Windows (MSVC and MinGW) Only.
 * @param [in] image     Image you want to resize
 * @param [in] autoRate  Resize ratio on the basis of the resolution of your display
 * @return  Resize ratio
 */
ATTR_NOTHROW ATTR_PURE inline static SizeInfo
calcAutoSizeRate(const cv::Mat &image, double autoRate) noexcept
{
  int screenWidth  = static_cast<int>(::GetSystemMetrics(SM_CXSCREEN) * autoRate);
  int screenHeight = static_cast<int>(::GetSystemMetrics(SM_CYSCREEN) * autoRate);
  if (image.rows <= screenWidth || image.cols <= screenHeight) {
    SizeInfo sizeInfo = {0, 0, 1.0, 1.0, autoRate};
    return sizeInfo;
  } else {
    SizeInfo sizeInfo = {
      0,
      0,
      static_cast<double>(screenWidth)  / image.cols,
      static_cast<double>(screenHeight) / image.rows,
      autoRate
    };
    if (sizeInfo.wRate > sizeInfo.hRate) {
      sizeInfo.wRate = sizeInfo.hRate;
    } else {
      sizeInfo.hRate = sizeInfo.wRate;
    }
    return sizeInfo;
  }
}
#endif




#undef WINDOWS_H
#endif  // CV_UTIL_H
