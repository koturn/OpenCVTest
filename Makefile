### This Makefile was written for GNU Make. ###
MAKE        ?= make
CTAGS       = ctags
CTAGS_FLAGS = -R --languages=c++
TARGET_DIR1 = 01-concatImage
TARGET_DIR2 = 02-edgeDetection
TARGET_DIR3 = 03-fillArea
TARGET_DIR4 = 04-evalArea
DIR_LIST    = $(TARGET_DIR1) $(TARGET_DIR2) $(TARGET_DIR3) $(TARGET_DIR4)  \


.PHONY : all
all :
	@for dir in $(DIR_LIST); do  \
		$(MAKE) -C $$dir;        \
	done

.PHONY : ctags
ctags :
	$(CTAGS) -f ./common.tags $(CTAGS_FLAGS) util/ include/
	@for dir in $(DIR_LIST); do  \
		$(MAKE) -C $$dir ctags;  \
	done

.PHONY : clean
clean :
	@for dir in $(DIR_LIST); do  \
		$(MAKE) -C $$dir clean;  \
	done

.PHONY : objclean
objclean :
	@for dir in $(DIR_LIST); do     \
		$(MAKE) -C $$dir objclean;  \
	done
