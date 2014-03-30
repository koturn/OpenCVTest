/*!
 * @brief Concat image files
 * @author  koturn 0;
 * @file    concatImage.cpp
 */
#include <gccUtil/nowarnings.h>
#include <iostream>
#include <getopt.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <commonUtil/compat.h>
#include <commonUtil/foreach.h>
#include <gccUtil/restorewarnings.h>

#include "../util/include/cvUtil.h"


//! The structre of parameters for this program
typedef struct {
  const char *dstFilename;  //!< Destinaion image file name
  const char *order;        //!< Combination direction
  bool        isSave;       //!< Save combined image or not
  bool        isShow;       //!< Show combined image or not
  SizeInfo    sizeInfo;     //!< Size information of the iamges
} Param;

static Param
parseArguments(int argc, char *argv[]);

ATTR_NOTHROW ALWAYSINLINE static void
showUsage(const char *progname) noexcept;

static cv::Mat
combineImages(const std::vector<cv::Mat> &images, const char *order);

ATTR_NOTHROW ATTR_PURE CONSTEXPR_CXX14 static CvSize
calcTotalImageSize(const std::vector<cv::Mat> &images) noexcept;

ATTR_NOTHROW ATTR_PURE CONSTEXPR_CXX14 static CvSize
calcMaxImageSize(const std::vector<cv::Mat> &images) noexcept;


/*!
 * @brief The entry point of this program
 * @param [in] argc  A number of comand-line arguments
 * @param [in] argv  Command-line arguments.
 * @return  exit-status
 */
int
main(int argc, char *argv[])
{
  Param param;
  try {
    param = parseArguments(argc, argv);
  } catch (const char *errmsg) {
    std::cerr << "ERROR: " << errmsg << std::endl;
    showUsage(argv[0]);
    return EXIT_FAILURE;
  }

  std::vector<cv::Mat> images;
  FOR (i, optind, argc) {
    cv::Mat image = cv::imread(argv[i]);
    if (image.data == nullptr) {
      std::cerr << "Invalid image: " << argv[i] << std::endl;
      return EXIT_FAILURE;
    } else {
      std::cout << "channel " << argv[i] << " = " << image.channels() << std::endl;
    }
    images.push_back(image);
  }

  cv::Mat combinedImage;
  try {
    combinedImage = combineImages(images, param.order);
  } catch (const char *errmsg) {
    std::cerr << "ERROR: " << errmsg << std::endl;
    return EXIT_FAILURE;
  }

  if (param.isShow) {
    cv::Mat resizedImage = resizeImage(combinedImage, param.sizeInfo);
    cv::namedWindow("Combined Image", CV_WINDOW_AUTOSIZE);
    cv::imshow("Combined Image", resizedImage);
    cv::waitKey(0);
  }

  if (!param.isSave) {
    return EXIT_SUCCESS;
  }

  std::string dstFilename;
  if (param.dstFilename == nullptr) {
    dstFilename = "concat.png";
  } else {
    dstFilename = std::string(param.dstFilename);
  }
  if (!cv::imwrite(dstFilename, combinedImage)) {
    std::cerr << "Failed to write image: " << dstFilename << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}


/*!
 * @brief Parse comamnd-line arguments and set parameters.
 *
 * 'argv' is sorted after called getopt_long().
 * @param [in]     argc  A number of command-line arguments
 * @param [in,out] argv  Coomand-line arguments
 * @return Parameter structure of this program.
 */
static Param
parseArguments(int argc, char *argv[])
{
  static const struct option opts[] = {
    {"nosave", no_argument,       nullptr, 0},
    {"noshow", no_argument,       nullptr, 1},
    {"order",  required_argument, nullptr, '2'},
    {"help",   no_argument,       nullptr, 'h'},
    {"output", required_argument, nullptr, 'o'},
    {"size",   required_argument, nullptr, 's'},
    {0, 0, 0, 0}   // must be filled with zero
  };

  int ret;
  int optidx;
  Param param = {nullptr, "x", true, true, {-1, -1, 1.0, 1.0, 0.5}};
  while ((ret = getopt_long(argc, argv, "ho:s:", opts, &optidx)) != -1) {
    switch (ret) {
      case 0:    // --nosave
        param.isSave = false;
        break;
      case 1:    // --noshow
        param.isShow = false;
        break;
      case '2':  // --order
        if (std::strcmp(optarg, "x") && std::strcmp(optarg, "y")) {
          throw "Invalid argument for option: --order";
        }
        param.order = optarg;
        break;
      case 'h':  // -h or --help
        showUsage(argv[0]);
        std::exit(EXIT_SUCCESS);
      case 'o':  // -o or --output
        param.dstFilename = optarg;
        break;
      case 's':  // -s or --size
        parseSizeString(param.sizeInfo, optarg);
        break;
      case '?':  // unknown option
        showUsage(argv[0]);
        std::exit(EXIT_FAILURE);
    }
  }
  if (optind > argc - 2) {
    throw "Invalid arguments: Specify more than tow image files";
  }
  return param;
}


/*!
 * @brief Show an usage of this program.
 * @param [in] progname  A name of this program
 */
ATTR_NOTHROW ALWAYSINLINE static void
showUsage(const char *progname) noexcept
{
  std::cout << "[Usage]\n"
            << "  $ " << progname << " FILENAME ... [options]\n\n"
               "[options]\n"
               "  -h, --help\n"
               "    Show help and exit\n"
               "  -o FILENAME, --output=FILENAME\n"
               "    Specify output image-file name\n"
               "      DEFAULT_VALUE = concat.png\n"
               "  -s SIZE_STRING, --size=SIZE_STRING\n"
               "    Specify output image-size to show [WWWxHHH, RRR%, auto, original]\n"
               "      DEFAULT_VALUE = auto\n"
               "  --nosave\n"
               "    Don't write result-image to file\n"
               "  --noshow\n"
               "    Don't show result-image to window\n"
               "  --order\n"
               "    Set combine order [x or y]\n"
               "      DEFAULT_VALUE = x"
            << std::endl;
}


/*!
 * @brief Combine images x-order or y-order
 * @param [in] images  Images
 * @param [in] order   A direction combination ("x" or "y")
 * @return  A combined image
 */
static cv::Mat
combineImages(const std::vector<cv::Mat> &images, const char *order)
{
  CvSize totalSize = calcTotalImageSize(images);
  CvSize maxSize   = calcMaxImageSize(images);

  if (!std::strcmp(order, "x")) {
    cv::Mat combinedImage(cv::Size(totalSize.width, maxSize.height), CV_8UC3);
    cv::Rect roiRect;
    FOREACH (elm, images) {
      roiRect.width  = elm->cols;
      roiRect.height = elm->rows;

      cv::Mat roi(combinedImage, roiRect);
      elm->copyTo(roi);
      roiRect.x += elm->cols;
    }
    return combinedImage;
  } else if (!std::strcmp(order, "y")) {
    cv::Mat combinedImage(cv::Size(maxSize.width, totalSize.height), CV_8UC3);
    cv::Rect roiRect;
    FOREACH (elm, images) {
      roiRect.width  = elm->cols;
      roiRect.height = elm->rows;

      cv::Mat roi(combinedImage, roiRect);
      elm->copyTo(roi);
      roiRect.y += elm->rows;
    }
    return combinedImage;
  } else {
    throw "Invalid order is specified";
  }
}


/*!
 * @brief Calcurate total size of images
 * @param [in]  images
 * @return  A total size of images.
 */
ATTR_NOTHROW ATTR_PURE CONSTEXPR_CXX14 static CvSize
calcTotalImageSize(const std::vector<cv::Mat> &images) noexcept
{
  CvSize totalSize = {0, 0};
  FOREACH (image, images) {
    totalSize.width  += image->cols;
    totalSize.height += image->rows;
  }
  return totalSize;
}


/*!
 * @brief Calcurate max size of image
 * @param [in]  images
 * @return  Maximum size of image
 */
ATTR_NOTHROW ATTR_PURE CONSTEXPR_CXX14 static CvSize
calcMaxImageSize(const std::vector<cv::Mat> &images) noexcept
{
  CvSize maxSize = {0, 0};
  FOREACH (image, images) {
    maxSize.width  = maxSize.width  < image->cols ? image->cols : maxSize.width;
    maxSize.height = maxSize.height < image->rows ? image->rows : maxSize.height;
  }
  return maxSize;
}
