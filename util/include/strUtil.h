/*!
 * @brief Provide utility functions of string
 * @author koturn 0;
 * @file strUtil.h
 */
#ifndef STR_UTIL_H
#define STR_UTIL_H

#include <string>
#include "../../include/commonUtil/compat.h"


ATTR_NOTHROW inline static std::string
removeSuffix(const char *filename) noexcept;

ATTR_NOTHROW inline static std::string
getSuffix(const char *filename) noexcept;




/*!
 * @brief Remove the suffix from a file name
 * @param [in] filename  A File name
 * @return  File name without suffix
 */
ATTR_NOTHROW inline static std::string
removeSuffix(const char *filename) noexcept
{
  std::string sFilename      = std::string(filename);
  std::string::size_type idx = sFilename.find_last_of('.');
  return idx == std::string::npos ? sFilename : sFilename.substr(0, idx);
}


/*!
 * @brief Get the suffix from a file name
 * @param [in] filename  A File name
 * @return  The suffix of the file name
 */
ATTR_NOTHROW inline static std::string
getSuffix(const char *filename) noexcept
{
  static const std::string DEFAULT_SUFFIX = "png";
  std::string sFilename      = std::string(filename);
  std::string::size_type idx = sFilename.find_last_of('.');
  return (idx == std::string::npos || idx == sFilename.length() - 1) ? DEFAULT_SUFFIX : sFilename.substr(idx + 1);
}




#endif  // STR_UTIL_H
