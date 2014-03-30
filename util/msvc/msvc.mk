CC      = cl
AR      = lib
RM      = del /F

MACROS  = /D_CRT_SECURE_NO_WARNINGS /D_SECURE_SCL=0
CFLAGS  = /nologo /c /O2 /W4 $(MACROS)
LDFLAGS = /nologo /O2
ARFLAGS = /nologo

DST_DIR = lib
SRC_DIR = src
TARGET        = $(DST_DIR)\getopt.lib
OBJ1_BASENAME = getopt.obj
OBJ2_BASENAME = getopt_long.obj
OBJ1          = $(DST_DIR)\$(OBJ1_BASENAME)
OBJ2          = $(DST_DIR)\$(OBJ2_BASENAME)
SRC1          = $(SRC_DIR)\$(OBJ1_BASENAME:.obj=.c)
SRC2          = $(SRC_DIR)\$(OBJ2_BASENAME:.obj=.c)
HEADER        = include/getopt.h


.SUFFIXES : .c .obj
.c.obj :
	$(CC) $(CFLAGS) $** /c /Fo$@


all : $(TARGET)

$(TARGET) : $(OBJ1) $(OBJ2)
	$(AR) $(ARFLAGS) $** /OUT:$@

$(OBJ1) : $(SRC1)
	$(CC) $(CFLAGS) $** /Fo$@

$(OBJ2) : $(SRC2) $(HEADER)
	$(CC) $(CFLAGS) $** /Fo$@


clean :
	$(RM) $(TARGET) $(OBJ1) $(OBJ2)
objclean :
	$(RM) $(OBJ1) $(OBJ2)
