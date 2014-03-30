### This Makefile was written for GNU Make. ###
include ../opencv_env.mk
CV_LDLIBS = -L$(CV_LDLIBS_DIR) \
            -l$(CV_CORE)       \
            -l$(CV_HIGHGUI)    \
            -l$(CV_IMGPROC)
CTAGS       = ctags
CTAGS_FLAGS = -R --languages=c++

CXX      = g++
STD      = gnu++0x
INCS     = -I../include/ $(CV_INCS)
CXXFLAGS = -pipe $(CXX_WARNING_FLAGS) $(CXXOPTFLAGS) $(INCS) $(if $(STD), $(addprefix -std=, $(STD)),) $(MACROS)
LDFLAGS  = -pipe $(LDOPTFLAGS)
LDLIBS   = -lm $(CV_LDLIBS)
TARGET   = fillArea
OBJ      = $(addsuffix .o, $(basename $(TARGET)))
SRC      = $(OBJ:%.o=%.cpp)

C_WARNING_FLAGS = -Wall -Wextra -Wformat=2 -Wstrict-aliasing=2  \
                  -Wcast-align -Wcast-qual -Wconversion         \
                  -Wfloat-equal -Wpointer-arith -Wswitch-enum   \
                  -Wwrite-strings -pedantic
CXX_WARNING_FLAGS = $(C_WARNING_FLAGS) -Weffc++ -Woverloaded-virtual


ifeq ($(OS),Windows_NT)
    TARGET := $(addsuffix .exe, $(TARGET))
else
    TARGET := $(addsuffix .out, $(TARGET))
endif

# priority : DEBUG > LTO > OPT
ifeq ($(OPT),true)
    CXXOPTFLAGS  = -flto -Ofast -march=native
    LDOPTFLAGS   = -flto -Ofast -s
else
    CXXOPTFLAGS  = -O3
    LDOPTFLAGS   = -O3 -s
endif
ifeq ($(LTO),true)
    CXXOPTFLAGS  = -flto
    LDOPTFLAGS   = -flto
endif
ifeq ($(DEBUG),true)
    CXXOPTFLAGS  = -O0 -g3 -ftrapv -fstack-protector -D_FORTIFY_SOURCE=2
endif

ifeq ($(OMP),true)
    CXXOPTFLAGS  += -fopenmp
    LDOPTFLAGS   += -fopenmp
else
    CXXOPTFLAGS  += -Wno-unknown-pragmas
endif

%.exe :
	$(CXX) $(LDFLAGS) $(filter %.c %.cpp %.o, $^) $(LDLIBS) -o $@
%.out :
	$(CXX) $(LDFLAGS) $(filter %.c %.cpp %.o, $^) $(LDLIBS) -o $@


all : $(TARGET)

$(TARGET) : $(OBJ)

$(OBJ) : $(SRC)


.PHONY : ctags
ctags :
	$(CTAGS) $(CTAGS_FLAGS)
.PHONY : clean
clean :
	$(RM) $(TARGET) $(OBJ)
.PHONY : objclean
objclean :
	$(RM) $(OBJ)
