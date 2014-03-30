### This Makefile was written for nmake. ###
include ../opencv_env_msvc.mk
WIN32_LDIBS = User32.lib
CV_LDLIBS   = /LIBPATH:$(CV_LDLIBS_DIR) \
              $(CV_CORE)                \
              $(CV_HIGHGUI)             \
              $(CV_IMGPROC)
GETOPT_LIB  = ../util/msvc/lib/getopt.lib
GETOPT_INCS = /I../util/msvc/include/

CTAGS       = ctags
CTAGS_FLAGS = -R --languages=c++

CPP      = cl
RM       = del /F
INCS     = /I../include/ $(GETOPT_INCS) $(CV_INCS)
MACROS   = /D_CRT_SECURE_NO_WARNINGS /D_SECURE_SCL=0
CPPFLAGS = /nologo /c /EHsc /O2 /W4 $(INCS) $(MACROS)
LDFLAGS  = /nologo /O2
LDLIBS   = /link $(WIN32_LDIBS) $(GETOPT_LIB) $(CV_LDLIBS)
TARGET   = edgeDetection.exe
OBJ      = $(TARGET:.exe=.obj)
SRC      = $(TARGET:.exe=.cpp)


all : $(TARGET)

$(TARGET) : $(OBJ)
	$(CPP) $** /Fe$@ $(LDFLAGS) $(LDLIBS)

$(OBJ) : $(SRC)
	$(CPP) $** $(CPPFLAGS) /Fo$@


ctags :
	$(CTAGS) $(CTAGS_FLAGS)
clean :
	$(RM) $(TARGET) $(OBJ) $(TARGET:.exe=.exp) $(TARGET:.exe=.lib)
objclean :
	$(RM) $(OBJ) $(TARGET:.exe=.exp) $(TARGET:.exe=.lib)
