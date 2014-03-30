#include <gccUtil/nowarnings.h>
#include <climits>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <getopt.h>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <commonUtil/compat.h>
#include <commonUtil/foreach.h>
#include <gccUtil/restorewarnings.h>

#include "../util/include/cvUtil.h"
#include "../util/include/strUtil.h"


//! The structre of parameters for this program
typedef struct {
  const char *srcFilename;
  const char *dstFilename;
  bool        isSave;
  bool        isShow;
  bool        isXBase;
  int         trimBlank;
  int         foregroundColor;
  SizeInfo    sizeInfo;
} Param;


static Param
parseArguments(int argc, char *argv[]);

ATTR_NOTHROW ALWAYSINLINE static void
showUsage(const char *progname) noexcept;

ATTR_NOTHROW static cv::Mat
fillAreaXBase(const cv::Mat &srcImage, int foregroundColor) noexcept;

ATTR_NOTHROW static cv::Mat
fillAreaYBase(const cv::Mat &srcImage, int foregroundColor) noexcept;

ATTR_NOTHROW static cv::Rect
searchArea(const cv::Mat &image, int foregroundColor) noexcept;

ATTR_NOTHROW static cv::Rect
addBlankToRect(const cv::Mat &image, const cv::Rect &roiRect, int blank) noexcept;


static const int R_MASK   = 0x00ff0000;  //!< Mask for taking out the red from int value
static const int G_MASK   = 0x0000ff00;  //!< Mask for taking out the green from int value
static const int B_MASK   = 0x000000ff;  //!< Mask for taking out the blue from int value
static const int RGB_MASK = 0x00ffffff;  //!< Mask for taking out the RGB from int value


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

  cv::Mat dstImage;
  if (param.isXBase) {
    dstImage = fillAreaXBase(srcImage, param.foregroundColor);
  } else {
    dstImage = fillAreaYBase(srcImage, param.foregroundColor);
  }

  if (param.trimBlank != -1) {
    cv::Rect roiRect = addBlankToRect(dstImage, searchArea(dstImage, param.foregroundColor), param.trimBlank);
    dstImage = dstImage(cv::Rect(roiRect.x, roiRect.y, roiRect.width, roiRect.height));
  }

  if (param.isShow) {
    cv::namedWindow("srcImage", CV_WINDOW_AUTOSIZE);
    cv::namedWindow("dstImage", CV_WINDOW_AUTOSIZE);
    cv::imshow("srcImage", resizeImage(srcImage, param.sizeInfo));
    cv::imshow("dstImage", resizeImage(dstImage, param.sizeInfo));
    cv::waitKey(0);
  }

  if (!param.isSave) {
    return EXIT_SUCCESS;
  }

  std::string dstFilename;
  if (param.dstFilename == nullptr) {
    dstFilename = removeSuffix(param.srcFilename) + "-filled." + getSuffix(param.srcFilename);
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
    {"nosave",     no_argument,       nullptr, 0},
    {"noshow",     no_argument,       nullptr, 1},
    {"direction",  required_argument, nullptr, 'd'},
    {"foreground", required_argument, nullptr, 'f'},
    {"help",       no_argument,       nullptr, 'h'},
    {"output",     required_argument, nullptr, 'o'},
    {"size",       required_argument, nullptr, 's'},
    {"trim",       optional_argument, nullptr, 't'},
    {0, 0, 0, 0}   // must be filled with zero
  };

  int ret;
  int optidx;
  Param param = {nullptr, nullptr, true, true, true, -1, 0x00000000, {-1, -1, 1.0, 1.0, 0.5}};
  while ((ret = getopt_long(argc, argv, "d:f:ho:s:t:", opts, &optidx)) != -1) {
    switch (ret) {
      case 0:    // --nosave
        param.isSave = false;
        break;
      case 1:    // --noshow
        param.isShow = false;
        break;
      case 'd':  // -d or --direction
        if (!std::strcmp(optarg, "x")) {
          param.isXBase = true;
        } else if (!std::strcmp(optarg, "y")) {
          param.isXBase = false;
        } else {
          throw "Invalid option argument: -s, --scan";
        }
        break;
      case 'f':  // -f or --foreground
        if (std::sscanf(optarg, "%x", reinterpret_cast<unsigned int *>(&param.foregroundColor)) != 1) {
          throw "Invalid option argument: -f, --foreground";
        }
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
      case 't':  // -t or --trim
        if (optarg == nullptr) {
          param.trimBlank = 0;
        } else if (std::sscanf(optarg, "%d", &param.trimBlank) != 1) {
          throw "Invalid option argument: -t, --trim";
        }
        if (param.trimBlank < 0) {
          throw "Invalid value for option argument: -t, --trim (negative value is not allowed)";
        }
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
               "  -d DIRECTION, --direction=DIRECTION\n"
               "    Specify scanning direction [x or y]\n"
               "      DEFAULT_VALUE = x\n"
               "  -f COLOR, --foreground=COLOR\n"
               "    Specify object color [0x000000 ~ 0xffffff]\n"
               "      DEFAULT_VALUE = 0x000000\n"
               "  -h, --help\n"
               "    Show help and exit\n"
               "  -o FILENAME, --output=FILENAME\n"
               "    Specify output image-file name\n"
               "      DEFAULT_VALUE = SRC_FILENAME-filled.SRC_FILENAME_SUFFIX\n"
               "  -s SIZE_STRING, --size=SIZE_STRING\n"
               "    Specify output image-size to show [WWWxHHH, RRR%, auto, original]\n"
               "      DEFAULT_VALUE = auto\n"
               "  -t (BLANK_SPACE), --trim(=BLANK_SPACE)\n"
               "    Specify blank-space for destination image\n"
               "    argument is optional\n"
               "  --nosave\n"
               "    Don't write result-image to file\n"
               "  --noshow\n"
               "    Don't show result-image to window"
            << std::endl;
}


/*!
 * @brief Fill area surrounded by specified color line with x-axis base
 * @param [in] srcImage         A image you want to fill
 * @param [in] foregroundColor  A color of line and area
 * @return  A fille image
 */
ATTR_NOTHROW static cv::Mat
fillAreaXBase(const cv::Mat &srcImage, int foregroundColor) noexcept
{
  unsigned char foregroundR = static_cast<unsigned char>((foregroundColor & R_MASK) >> 16);
  unsigned char foregroundG = static_cast<unsigned char>((foregroundColor & G_MASK) >> 8);
  unsigned char foregroundB = static_cast<unsigned char>((foregroundColor & B_MASK));

  cv::Mat dstImage = srcImage.clone();
  REP_I (y, dstImage.rows) {
    int x1 = 0;
    while (x1 < dstImage.cols) {
      for (; x1 < dstImage.cols && (*reinterpret_cast<int *>(&dstImage.data[y * dstImage.step + x1 * dstImage.elemSize()]) & RGB_MASK) != foregroundColor; x1++);
      for (; x1 < dstImage.cols && (*reinterpret_cast<int *>(&dstImage.data[y * dstImage.step + x1 * dstImage.elemSize()]) & RGB_MASK) == foregroundColor; x1++);

      int x2 = x1;
      for (; x2 < dstImage.cols && (*reinterpret_cast<int *>(&dstImage.data[y * dstImage.step + x2 * dstImage.elemSize()]) & RGB_MASK) != foregroundColor; x2++);

      if (x2 == dstImage.cols) continue;

      for (int px = x1; px < x2; px++) {
        unsigned char *restrict pixelAddr = &dstImage.data[y * dstImage.step + px * dstImage.elemSize()];
        pixelAddr[0] = foregroundB;
        pixelAddr[1] = foregroundG;
        pixelAddr[2] = foregroundR;
      }
    }
  }
  return dstImage;
}


/*!
 * @brief Fill area surrounded by specified color line with y-axis base
 * @param [in] srcImage         A image you want to fill
 * @param [in] foregroundColor  A color of line and area
 * @return  A fille image
 */
ATTR_NOTHROW static cv::Mat
fillAreaYBase(const cv::Mat &srcImage, int foregroundColor) noexcept
{
  unsigned char foregroundR = static_cast<unsigned char>((foregroundColor & R_MASK) >> 16);
  unsigned char foregroundG = static_cast<unsigned char>((foregroundColor & G_MASK) >> 8);
  unsigned char foregroundB = static_cast<unsigned char>((foregroundColor & B_MASK));

  cv::Mat dstImage = srcImage.clone();
  REP_I (x, dstImage.cols) {
    int y1 = 0;
    while (y1 < dstImage.rows) {
      for (; y1 < dstImage.rows && (*reinterpret_cast<int *>(&dstImage.data[y1 * dstImage.step + x * dstImage.elemSize()]) & RGB_MASK) != foregroundColor; y1++);
      for (; y1 < dstImage.rows && (*reinterpret_cast<int *>(&dstImage.data[y1 * dstImage.step + x * dstImage.elemSize()]) & RGB_MASK) == foregroundColor; y1++);

      int y2 = y1;
      for (; y2 < dstImage.rows && (*reinterpret_cast<int *>(&dstImage.data[y2 * dstImage.step + x * dstImage.elemSize()]) & RGB_MASK) != foregroundColor; y2++);

      if (y2 == dstImage.rows) continue;

      for (int py = y1; py < y2; py++) {
        unsigned char *restrict pixelAddr = &dstImage.data[py * dstImage.step + x * dstImage.elemSize()];
        pixelAddr[0] = foregroundB;
        pixelAddr[1] = foregroundG;
        pixelAddr[2] = foregroundR;
      }
    }
  }
  return dstImage;
}


/*!
 * @brief Search the area of the filled region in image
 * @param [in] image            A filled image
 * @param [in] foregroundColor  A color of filled region
 * @return  Filled region in image
 */
ATTR_NOTHROW static cv::Rect
searchArea(const cv::Mat &image, int foregroundColor) noexcept
{
  CvPoint p1 = {image.cols, image.rows};
  CvPoint p2 = {0, 0};
  REP_I (i, image.rows) {
    REP_I (j, image.cols) {
      if ((*reinterpret_cast<int *>(&image.data[i * image.step + j * image.elemSize()]) & RGB_MASK) == foregroundColor) {
        if (p1.x > j) p1.x = j;
        if (p1.y > i) p1.y = i;
        if (p2.x < j) p2.x = j;
        if (p2.y < i) p2.y = i;
      }
    }
  }
  cv::Rect areaRect(p1.x, p1.y, p2.x - p1.x, p2.y - p1.y);
  return areaRect;
}


/*!
 * @brief Add blank to filled region
 * @param [in] image    A filled image
 * @param [in] roiRect  Filled region in image
 * @param [in] blank    blank width and height (pixel)
 * @return  Filled region with blank space
 */
ATTR_NOTHROW static cv::Rect
addBlankToRect(const cv::Mat &image, const cv::Rect &roiRect, int blank) noexcept
{
  if (blank < 1) return roiRect;

  cv::Rect newRoiRect(roiRect.x - blank, roiRect.y - blank, roiRect.width + 2 * blank, roiRect.height + 2 * blank);
  if (newRoiRect.x < 0) newRoiRect.x = 0;
  if (newRoiRect.y < 0) newRoiRect.y = 0;
  if (newRoiRect.x + newRoiRect.width  > image.cols)  newRoiRect.width  = image.cols - newRoiRect.x;
  if (newRoiRect.y + newRoiRect.height > image.rows)  newRoiRect.height = image.rows - newRoiRect.y;
  return newRoiRect;
}
