#include <gccUtil/nowarnings.h>
#include <cstdio>
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <commonUtil/compat.h>
#include <gccUtil/restorewarnings.h>

#include "../util/include/cvUtil.h"
#include "../util/include/strUtil.h"


//! The structre of parameters for this program
typedef struct {
  const char *srcFilename;
  const char *dstFilename;
  const char *filterName;
  bool        isSave;
  bool        isShow;
  SizeInfo    sizeInfo;
} Param;


static Param
parseArguments(int argc, char *argv[]);

ATTR_NOTHROW ALWAYSINLINE static void
showUsage(const char *progname) noexcept;

ATTR_NOTHROW inline static cv::Mat
filtering(const cv::Mat &image, const char *filterName) noexcept;


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

  cv::Mat srcImage = cv::imread(param.srcFilename);
  if (srcImage.data == nullptr) {
    return EXIT_FAILURE;
  }

  cv::Mat grayImage;
  cv::cvtColor(srcImage, grayImage, CV_BGR2GRAY);
  cv::Mat dstImage =filtering(grayImage, "laplacian");

  if (param.isShow) {
    cv::namedWindow("Original", CV_WINDOW_AUTOSIZE);
    cv::namedWindow("Original(Grayscale)", CV_WINDOW_AUTOSIZE);
    cv::namedWindow("Laplacian", CV_WINDOW_AUTOSIZE);
    cv::imshow("Original", resizeImage(srcImage, param.sizeInfo));
    cv::imshow("Original(Grayscale)", resizeImage(grayImage, param.sizeInfo));
    cv::imshow("Laplacian", resizeImage(dstImage, param.sizeInfo));
    cv::waitKey(0);
  }

  if (!param.isShow) {
    return EXIT_SUCCESS;
  }

  std::string dstFilename;
  if (param.dstFilename == nullptr) {
    dstFilename = removeSuffix(param.srcFilename) + "-edge." + getSuffix(param.srcFilename);
  } else {
    dstFilename = std::string(param.dstFilename);
  }
  if (!cv::imwrite(dstFilename, dstImage)) {
    std::cerr << "Failed to write image: " << dstFilename << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}


/*!
 * @brief Adapt edge-detection-filter to gray scale image
 * @param [in] grayImage   Gray scale source image
 * @param [in] filterName  A name of edge detection filter (sobel, laplacian or canny)
 */
ATTR_NOTHROW inline static cv::Mat
filtering(const cv::Mat &grayImage, const char *filterName) noexcept
{
  cv::Mat tmpImage;
  cv::Mat dstImage;
  if (!std::strcmp(filterName, "sobel")) {
    cv::Sobel(grayImage, tmpImage, CV_32F, 1, 1);
    cv::convertScaleAbs(tmpImage, dstImage, 1, 0);
  } else if (!std::strcmp(filterName, "laplacian")) {
    cv::Laplacian(grayImage, tmpImage, CV_32F, 3);
    cv::convertScaleAbs(tmpImage, dstImage, 1, 0);
  } else if (!std::strcmp(filterName, "canny")) {
    cv::Canny(grayImage, dstImage, 50, 200);
  }
  return dstImage;
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
    {"filter", required_argument, nullptr, 'f'},
    {"help",   no_argument,       nullptr, 'h'},
    {"output", required_argument, nullptr, 'o'},
    {"size",   required_argument, nullptr, 's'},
    {0, 0, 0, 0}   // must be filled with zero
  };

  int ret;
  int optidx;
  Param param = {nullptr, nullptr, "laplacian", true, true, {-1, -1, 1.0, 1.0, 0.5}};
  while ((ret = getopt_long(argc, argv, "f:ho:s:", opts, &optidx)) != -1) {
    switch (ret) {
      case 0:    // --nosave
        param.isSave = false;
        break;
      case 1:    // --noshow
        param.isShow = false;
        break;
      case 'f':  // -f or --filter
        param.filterName = optarg;
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
  if (optind != argc - 1) {
    throw "Invalid arguments";
  }
  param.srcFilename = argv[optind];
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
            << "  $ " << progname << " FILENAME [options]\n\n"
               "[options]\n"
               "  -f FILTER, --filter=FILTER\n"
               "    Specify edge detection filter [sobel, laplacian, canny]\n"
               "      DEFAULT_VALUE = laplacian\n"
               "  -h, --help\n"
               "    Show help and exit\n"
               "  -o FILENAME, --output=FILENAME\n"
               "    Specify output image-file name\n"
               "      DEFAULT_VALUE = SRC_FILENAME-edge.SRC_FILENAME_SUFFIX\n"
               "  -s SIZE_STRING, --size=SIZE_STRING\n"
               "    Specify output image-size to show [WWWxHHH, RRR%, auto, original]\n"
               "      DEFAULT_VALUE = auto\n"
               "  --nosave\n"
               "    Don't write result-image to file\n"
               "  --noshow\n"
               "    Don't show result-image to window"
            << std::endl;
}
