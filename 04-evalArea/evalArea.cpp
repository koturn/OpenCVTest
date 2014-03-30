/*!
 * @brief Calculate metrics of the image
 *
 * By one degree from zero to 360 degrees, calculates the distance to the
 * position where the boundary of the region from the center of gravity.
 *
 * @author  koturn 0;
 * @file    evalArea.cpp
 */
#include <gccUtil/nowarnings.h>
#include <cmath>
#include <cstdio>
#include <getopt.h>
#include <vector>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <commonUtil/compat.h>
#include <commonUtil/foreach.h>
#include <gccUtil/restorewarnings.h>

#include "../util/include/cvUtil.h"
#include "../util/include/mathUtil.h"
#include "../util/include/strUtil.h"


//! The structre of parameters for this program
typedef struct {
  const char *srcFilename;      //!< A name of filled iamge
  const char *plotFilename;     //!< A name of image which is plotted points
  const char *dstFilename;      //!< A name of result csv-file
  bool        isSave;           //!< Save combined image or not
  bool        isShow;           //!< Show combined image or not
  int         foregroundColor;  //!< A color of filled region
  int         plotColor;        //!< A color for plotting
  int         gPointColor;      //!< A color of gravity point
  SizeInfo    sizeInfo;         //!< Size information of the iamge
} Param;

static Param
parseArguments(int argc, char *argv[]);

ATTR_NOTHROW ALWAYSINLINE static void
showUsage(const char *progname) noexcept;

ATTR_PURE ATTR_NOTHROW CONSTEXPR_CXX14 static CvPoint
calcMoment(const cv::Mat &image, int foregroundColor) noexcept;

ATTR_NOTHROW static std::vector<CvPoint>
evalArea(const cv::Mat &image, const CvPoint &cp, int foregroundColor) noexcept;

ATTR_NOTHROW static cv::Mat
plotCrossPoints(const cv::Mat &image, const std::vector<CvPoint> &crossPoints, int plotColor) noexcept;

ATTR_NOTHROW static std::vector<double>
calcResults(const CvPoint &gp, const std::vector<CvPoint> &crossPoints) noexcept;

static const int R_MASK   = 0x00ff0000;  //!< Mask for taking out the red from int value
static const int G_MASK   = 0x0000ff00;  //!< Mask for taking out the green from int value
static const int B_MASK   = 0x000000ff;  //!< Mask for taking out the blue from int value


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
  cv::Mat image = cv::imread(param.srcFilename);
  if (image.data == nullptr) {
    std::cerr << "Failed to read image file: " << param.srcFilename << std::endl;
    return EXIT_FAILURE;
  }

  std::printf("foregroundColor = 0x%08x\n", param.foregroundColor);
  CvPoint gp = calcMoment(image, param.foregroundColor);
  std::printf("moment = (%d, %d)\n", gp.x, gp.y);

  std::vector<CvPoint> crossPoints = evalArea(image, gp, param.foregroundColor);
  // FOREACH (cp, crossPoints) {
  //   std::printf("(x, y) = (%d, %d)\n", cp->x, cp->y);
  // }
  // std::printf("%u points was plotted\n", crossPoints.size());

  std::vector<double> scores = calcResults(gp, crossPoints);
  if (param.dstFilename == nullptr) {
    REP (i, scores.size()) {
      std::printf("%03d,%f\n", i, scores[i]);
    }
  } else {
    std::FILE *fp = std::fopen(param.dstFilename, "w");
    REP (i, scores.size()) {
      std::fprintf(fp, "%03d,%f\n", i, scores[i]);
    }
    std::fclose(fp);
  }

  cv::Mat plottedImage = plotCrossPoints(image, crossPoints, param.plotColor);
  cv::Scalar color(
      (param.gPointColor & B_MASK),
      (param.gPointColor & G_MASK) >> 8,
      (param.gPointColor & R_MASK) >> 16
  );
  cv::circle(plottedImage, cv::Point(gp.x, gp.y), 1, color, -1, CV_AA);
  if (param.isShow) {
    cv::namedWindow("srcImage", CV_WINDOW_AUTOSIZE);
    cv::namedWindow("dstImage", CV_WINDOW_AUTOSIZE);
    cv::imshow("srcImage", resizeImage(image, param.sizeInfo));
    cv::imshow("dstImage", resizeImage(plottedImage, param.sizeInfo));
    cv::waitKey(0);
  }

  if (!param.isSave) {
    return EXIT_SUCCESS;
  }

  std::string plotFilename;
  if (param.plotFilename == nullptr) {
    plotFilename = removeSuffix(param.srcFilename) + "-plotted." + getSuffix(param.srcFilename);
  } else {
    plotFilename = std::string(param.plotFilename);
  }
  if (!cv::imwrite(plotFilename, plottedImage)) {
    std::cerr << "Failed to write image: " << plotFilename << std::endl;
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
    {"color",      required_argument, nullptr, 'c'},
    {"foreground", required_argument, nullptr, 'f'},
    {"gcolor",     required_argument, nullptr, 'g'},
    {"help",       no_argument,       nullptr, 'h'},
    {"output",     required_argument, nullptr, 'o'},
    {"plot-file",  required_argument, nullptr, 'p'},
    {"size",       required_argument, nullptr, 's'},
    {0, 0, 0, 0}   // must be filled with zero
  };

  int ret;
  int optidx;
  Param param = {
    nullptr,
    nullptr,
    nullptr,
    true,
    true,
    0x00000000,
    0x00ff0000,
    0x0000ff00,
    {-1, -1, 1.0, 1.0, 0.5}
  };
  while ((ret = getopt_long(argc, argv, "f:g:ho:s:", opts, &optidx)) != -1) {
    switch (ret) {
      case 0:    // --nosave
        param.isSave = false;
        break;
      case 1:    // --noshow
        param.isShow = false;
        break;
      case 'c':  // -c or --color
        if (std::sscanf(optarg, "%x", reinterpret_cast<unsigned int *>(&param.plotColor)) != 1) {
          throw "Invalid option argument: -c, --color";
        }
        break;
      case 'f':  // -f or --foreground
        if (std::sscanf(optarg, "%x", reinterpret_cast<unsigned int *>(&param.foregroundColor)) != 1) {
          throw "Invalid option argument: -f, --foreground";
        }
        break;
      case 'g':  // -g or --gcolor
        if (std::sscanf(optarg, "%x", reinterpret_cast<unsigned int *>(&param.gPointColor)) != 1) {
          throw "Invalid option argument: -g, --gcolor";
        }
        break;
      case 'h':  // -h or --help
        showUsage(argv[0]);
        std::exit(EXIT_SUCCESS);
      case 'o':  // -o or --output
        param.dstFilename = optarg;
        break;
      case 'p':  // -p or --plot-file
        param.plotFilename = optarg;
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
               "  -c COLOR, --color=COLOR\n"
               "    specify plot color [0x000000 ~ 0xffffff]\n"
               "      DEFAULT_VALUE = 0xff0000\n"
               "  -f COLOR, --foreground=COLOR\n"
               "    specify object color [0x000000 ~ 0xffffff]\n"
               "      DEFAULT_VALUE = 0x000000\n"
               "  -g COLOR, --gcolor=COLOR\n"
               "    specify gravity-point color [0x000000 ~ 0xffffff]\n"
               "      DEFAULT_VALUE = 0x00ff00\n"
               "  -h, --help\n"
               "    show help and exit\n"
               "  -o FILENAME, --output=FILENAME\n"
               "    Specify output csv-file name\n"
               "    If this option isn't specified, output is stdout\n"
               "  -p FILENAME, --plot-file=FILENAME\n"
               "    Specify output image-file name\n"
               "      DEFAULT_VALUE = SRC_FILENAME-plotted.SRC_FILENAME_SUFFIX\n"
               "  -s SIZE_STRING, --size=SIZE_STRING\n"
               "    Specify output image-size to show [WWWxHHH, RRR%, auto, original]\n"
               "      DEFAULT_VALUE = auto\n"
               "  --nosave\n"
               "    Don't write result-image to file\n"
               "  --noshow\n"
               "    Don't show result-image to window"
            << std::endl;
}


/*!
 * @brief Calculate moment of the filled region in image
 * @param [in] image            A filled image
 * @param [in] foregroundColor  A color of filled region
 * @return  Center of gravity of the filled region in image
 */
ATTR_PURE ATTR_NOTHROW CONSTEXPR_CXX14 static CvPoint
calcMoment(const cv::Mat &image, int foregroundColor) noexcept
{
  static const int RGB_MASK = 0x00ffffff;
  CvPoint gp = {0, 0};
  int cnt = 0;
  #pragma omp parallel for
  REP_I (i, image.rows) {
    REP_I (j, image.cols) {
      int rgb = (*reinterpret_cast<int *>(&image.data[i * image.step + j * image.elemSize()]) & RGB_MASK);
      if (rgb == foregroundColor) {
        gp.x += j;
        gp.y += i;
        cnt++;
      }
    }
  }
  std::printf("cnt = %d\n", cnt);
  if (cnt == 0) {
    gp.x = -1;
    gp.y = -1;
  } else {
    gp.x /= cnt;
    gp.y /= cnt;
  }
  return gp;
}


/*!
 * @brief Calculate metrics of image for evaluation
 * @param [in] image            A filled image
 * @param [in] gp               Center of gravity of the filled region in image
 * @param [in] foregroundColor  A color of filled region
 * @return  A metrics for evaluation
 */
ATTR_NOTHROW static std::vector<CvPoint>
evalArea(const cv::Mat &image, const CvPoint &gp, int foregroundColor) noexcept
{
  static const int    RGB_MASK    = 0x00ffffff;
  static const double DEGREE_STEP = 1.0;
  static const double DEGREE_MAX  = 180.0;

  std::vector<CvPoint> crossPoints1;
  std::vector<CvPoint> crossPoints2;
  for (double theta = 0.0; theta < DEGREE_MAX; theta += DEGREE_STEP) {
    double a = std::tan(degreeToRadian(theta));
    double b = gp.y - gp.x * a;
    int rest = (static_cast<int>(theta) / 45) % 4;
    if (rest == 1 || rest == 2) {  // y-based
      FOR (y, gp.y, image.rows) {
        int x = static_cast<int>(round((y - b) / a));
        x = clipping(x, 0, image.cols - 1);
        int rgb = (*reinterpret_cast<int *>(&image.data[y * image.step + x * image.elemSize()]) & RGB_MASK);
        if (rgb != foregroundColor || y == image.rows - 1 || x == 0 || x == image.cols - 1) {
          CvPoint crossPoint = {x, y};
          crossPoints1.push_back(crossPoint);
          break;
        }
      }
      RFOR (y, gp.y, 0) {
        int x = static_cast<int>(round((y - b) / a));
        x = clipping(x, 0, image.cols - 1);
        int rgb = (*reinterpret_cast<int *>(&image.data[y * image.step + x * image.elemSize()]) & RGB_MASK);
        if (rgb != foregroundColor || y == 0 || x == 0 || x == image.cols - 1) {
          CvPoint crossPoint = {x, y};
          crossPoints2.push_back(crossPoint);
          break;
        }
      }
    } else {  // x-based
      FOR (x, gp.x, image.cols) {
        int y = static_cast<int>(round(x * a + b));
        y = clipping(y, 0, image.rows - 1);
        int rgb = (*reinterpret_cast<int *>(&image.data[y * image.step + x * image.elemSize()]) & RGB_MASK);
        if (rgb != foregroundColor || x == 0 || x == image.cols - 1 || y == 0 || y == image.rows - 1) {
          CvPoint crossPoint = {x, y};
          crossPoints1.push_back(crossPoint);
          break;
        }
      }
      RFOR (x, gp.x, 0) {
        int y = static_cast<int>(round(x * a + b));
        y = clipping(y, 0, image.rows - 1);
        int rgb = (*reinterpret_cast<int *>(&image.data[y * image.step + x * image.elemSize()]) & RGB_MASK);
        if (rgb != foregroundColor || x == 0 || x == image.cols - 1  || y == 0 || y == image.rows - 1) {
          CvPoint crossPoint = {x, y};
          crossPoints2.push_back(crossPoint);
          break;
        }
      }
    }
  }
  crossPoints1.insert(crossPoints1.end(), crossPoints2.begin(), crossPoints2.end());
  return crossPoints1;
}


/*!
 * @brief Plot points of metrics on the image
 * @param [in] image        A filled image
 * @param [in] crossPoints  Points in the boundary of the region
 * @param [in] plotColor    A color for plotting
 * @return  Plotted image
 */
ATTR_NOTHROW static cv::Mat
plotCrossPoints(const cv::Mat &image, const std::vector<CvPoint> &crossPoints, int plotColor) noexcept
{
  cv::Mat plottedImage = image.clone();
  cv::Scalar color(
      (plotColor & B_MASK),
      (plotColor & G_MASK) >> 8,
      (plotColor & R_MASK) >> 16
  );
  FOREACH(cp, crossPoints) {
    cv::circle(plottedImage, cv::Point(cp->x, cp->y), 1, color, -1, CV_AA);
  }
  return plottedImage;
}


/*!
 * @brief Calculates the distance to the position where the boundary of the
 *        region from the center of gravity
 *
 * @param [in] gp           Center of gravity point
 * @param [in] crossPoints  Points in the boundary of the region
 * @return The distance to the position where the boundary of the region from
 *         the center of gravity
 */
ATTR_NOTHROW static std::vector<double>
calcResults(const CvPoint &gp, const std::vector<CvPoint> &crossPoints) noexcept
{
  std::vector<double> scores;
  FOREACH (cp, crossPoints) {
    double distance = calcDistance(gp, *cp);
    scores.push_back(distance);
  }
  return scores;
}
