CV_ARCH = x64
CV_VER  = 248
CV_DIR  = "C:/OpenCV/248/build"
CV_INCS = -I$(CV_DIR)/include/        \
          -I$(CV_DIR)/include/opencv/ \
          -I$(CV_DIR)/include/opencv2/
CV_LDLIBS_DIR = $(CV_DIR)/$(CV_ARCH)/mingw/lib

# static-library aliases
CV_CALIB3D    = opencv_calib3d$(CV_VER).dll
CV_CONTRIB    = opencv_contrib$(CV_VER).dll
CV_CORE       = opencv_core$(CV_VER).dll
CV_FEATURES2D = opencv_features2d$(CV_VER).dll
CV_FLANN      = opencv_flann$(CV_VER).dll
CV_GPU        = opencv_gpu$(CV_VER).dll
CV_HIGHGUI    = opencv_highgui$(CV_VER).dll
CV_IMGPROC    = opencv_imgproc$(CV_VER).dll
CV_LEGACY     = opencv_legacy$(CV_VER).dll
CV_ML         = opencv_ml$(CV_VER).dll
CV_NONFREE    = opencv_nonfree$(CV_VER).dll
CV_OBJDETECT  = opencv_objdetect$(CV_VER).dll
CV_PHOTO      = opencv_photo$(CV_VER).dll
CV_STITCHING  = opencv_stitching$(CV_VER).dll
CV_TS         = opencv_ts$(CV_VER)
CV_VIDEO      = opencv_video$(CV_VER).dll
CV_VIDEOSTAB  = opencv_videostab$(CV_VER).dll

CV_LDLIBS_ALL = -L$(CV_LDLIBS_DIR)  \
                -l$(CV_CALIB3D)     \
                -l$(CV_CONTRIB)     \
                -l$(CV_CORE)        \
                -l$(CV_FEATURES2D)  \
                -l$(CV_FLANN)       \
                -l$(CV_GPU)         \
                -l$(CV_HIGHGUI)     \
                -l$(CV_IMGPROC)     \
                -l$(CV_LEGACY)      \
                -l$(CV_ML)          \
                -l$(CV_NONFREE)     \
                -l$(CV_OBJDETECT)   \
                -l$(CV_PHOTO)       \
                -l$(CV_STITCHING)   \
                -l$(CV_TS)          \
                -l$(CV_VIDEO)       \
                -l$(CV_VIDEOSTAB)
