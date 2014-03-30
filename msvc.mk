### This Makefile was written for nmake. ###
MAKE        = nmake /NOLOGO
CTAGS       = ctags
CTAGS_FLAGS = -R --languages=c++
MAKEFILE    = msvc.mk
UTIL_DIR    = util\msvc
TARGET_DIR1 = 01-concatImage
TARGET_DIR2 = 02-edgeDetection
TARGET_DIR3 = 03-fillArea
TARGET_DIR4 = 04-evalArea
DIR_LIST    = $(TARGET_DIR1) $(TARGET_DIR2) $(TARGET_DIR3) $(TARGET_DIR4)  \


all :
	cd $(UTIL_DIR)  &  $(MAKE) /f $(MAKEFILE)  &  cd ..\..
	@for %d in ($(DIR_LIST)) do                     \
	    cd %d  &  $(MAKE) /f $(MAKEFILE)  &  cd ..  \

ctags :
	$(CTAGS) $(CTAGS_FLAGS) util/ include/ -f common.tags
	@for %d in ($(DIR_LIST)) do                     \
	    cd %d  &  $(MAKE) /f $(MAKEFILE) ctags  &  cd ..  \

clean :
	cd $(UTIL_DIR)  &  $(MAKE) /f $(MAKEFILE) clean  &  cd ..\..
	@for %d in ($(DIR_LIST)) do                           \
	    cd %d  &  $(MAKE) /f $(MAKEFILE) clean  &  cd ..  \

objclean :
	cd $(UTIL_DIR)  &  $(MAKE) /f $(MAKEFILE) objclean  &  cd ..\..
	@for %d in ($(DIR_LIST)) do                              \
	    cd %d  &  $(MAKE) /f $(MAKEFILE) objclean  &  cd ..  \
