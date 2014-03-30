CV_ARCH  = x64
CV_VER   = 248
MSVC_VER = vc12
CV_DIR   = "C:/OpenCV/248/build"
CV_INCS  = -I$(CV_DIR)/include/        \
           -I$(CV_DIR)/include/opencv/ \
           -I$(CV_DIR)/include/opencv2/
CV_LDLIBS_DIR = $(CV_DIR)/$(CV_ARCH)/$(MSVC_VER)/lib

# static-library aliases
CV_CALIB3D    = opencv_calib3d$(CV_VER).lib
CV_CONTRIB    = opencv_contrib$(CV_VER).lib
CV_CORE       = opencv_core$(CV_VER).lib
CV_FEATURES2D = opencv_features2d$(CV_VER).lib
CV_FLANN      = opencv_flann$(CV_VER).lib
CV_GPU        = opencv_gpu$(CV_VER).lib
CV_HIGHGUI    = opencv_highgui$(CV_VER).lib
CV_IMGPROC    = opencv_imgproc$(CV_VER).lib
CV_LEGACY     = opencv_legacy$(CV_VER).lib
CV_ML         = opencv_ml$(CV_VER).lib
CV_NONFREE    = opencv_nonfree$(CV_VER).lib
CV_OBJDETECT  = opencv_objdetect$(CV_VER).lib
CV_PHOTO      = opencv_photo$(CV_VER).lib
CV_STITCHING  = opencv_stitching$(CV_VER).lib
CV_TS         = opencv_ts$(CV_VER).lib
CV_VIDEO      = opencv_video$(CV_VER).lib
CV_VIDEOSTAB  = opencv_videostab$(CV_VER).lib

CV_LDLIBS_ALL = /link /LIBPATH:$(CV_LDLIBS_DIR)  \
                $(CV_CALIB3D)     \
                $(CV_CONTRIB)     \
                $(CV_CORE)        \
                $(CV_FEATURES2D)  \
                $(CV_FLANN)       \
                $(CV_GPU)         \
                $(CV_HIGHGUI)     \
                $(CV_IMGPROC)     \
                $(CV_LEGACY)      \
                $(CV_ML)          \
                $(CV_NONFREE)     \
                $(CV_OBJDETECT)   \
                $(CV_PHOTO)       \
                $(CV_STITCHING)   \
                $(CV_TS)          \
                $(CV_VIDEO)       \
                $(CV_VIDEOSTAB)
