# Makefile for application using shared libraries
SHELL		:= /bin/sh
PATH		:= /bin:/usr/bin:/sbin:/usr/sbin
BUILD_DATE	:=`date  +'%Y%m%d_%I-%M-%S'`

# CAN BE OVERRIDEN ON CMDLINE
SVN_DIR		= "file:///opt/SVN/repos/Projects"
INSTALL_DIR  	= /usr/local/bin
BUILD_ROOT	= .
CMD_LINE_DEFS   = -DVERSION="built on $BUILD_DATE"
#--------------------------------------------
# Use these directories
#--------------------------------------------
ECHO		:= echo -e
SRC_DIR 	:= ./src
BUILD_DIR 	:= $(BUILD_ROOT)/build
TEST_DIR	:= ./test
LIB_DIR 	:= ./lib
#--------------------------------------------
# Use these tools
#--------------------------------------------
ECHO		:= echo -e
CC		:= gcc
RM		:= rm -f
INSTALL		:= install -m
INSTALL_PERM 	:= 755 # rwxr-xr-x
#--------------------------------------------

#--------------------------------------------
# Need this version of libUtility
#--------------------------------------------
LIB_UTILITY_VER 	:= 0.0.0
LIB_UTILITY_DIR 	:= $(LIB_DIR)/libutility-$(LIB_UTILITY_VER)
LIB_UTILITY_SRC 	:= $(LIB_UTILITY_DIR)/src
#--------------------------------------------

#--------------------------------------------
# Use these headers
#--------------------------------------------
INCLUDES 	:= -I$(SRC_DIR) -I$(LIB_UTILITY_SRC)
#--------------------------------------------

#--------------------------------------------
# Build with these flags, only add common libs here
#--------------------------------------------
ANSI_C_89	:= -ansi -std=c89 -pedantic
ANSI_C_99	:= -ansi -std=c99 -pedantic
POSIX_C		:= -std=gnu99 -pedantic -D_XOPEN_SOURCE=500 -D_GNU_SOURCE  -D_BSD_SOURCE
WARNINGS	:= -Wall
CFLAGS		:= $(POSIX_C) $(WARNINGS) $(INCLUDES) $(CMD_LINE_DEFS)
CPPFLAGS 	:= $(CFLAGS)
CXXFLAGS 	:= $(CFLAGS)
TEST_CFLAGS	:= $(CFLAGS)
#--------------------------------------------

# dependencies live here
DEPS 		:= "._`pwd | sed -e 's:/:_:g'`_depend"
#--------------------------------------------

# patches live here 
# allow parallel builds of debug/release source trees
ifndef RELEASE
BUILD_TYPE	:= Debug
CFLAGS 		:= -g -O0 -DENABLE_DEBUG $(CFLAGS)
BUILD_DIR 	:= $(BUILD_DIR)/Debug
OBJ_DIR 	:= $(BUILD_DIR)/objs_debug
DEPS		:= $(BUILD_DIR)/$(DEPS)_debug
else
BUILD_TYPE	:= Release
CFLAGS 		:= -O2 $(CFLAGS)
OBJ_DIR 	:= $(BUILD_DIR)/objs_release
DEPS		:= $(BUILD_DIR)/$(DEPS)_release
endif

TEST_CFLAGS	:= $(CFLAGS)
LDFLAGS		 = -L./ -L$(OBJ_DIR) -L$(BUILD_DIR)

TARGET		:= gen-ruby

ifdef BUILDING_LIB
CFLAGS		:= -fPIC $(CFLAGS)
TEST_LDFLAGS	:= -l$(subst lib,,$(subst .so,,$(TARGET)))
endif

# pull in utility library
LIB_HDRS := 	
LIB_OBJS :=#	$(subst $(LIB_UTILITY_SRC), $(OBJ_DIR),$(subst .c,.o, $(LIB_SRCS)))

HDRS 	 :=	$(SRC_DIR)/type_macros.h  \
		$(SRC_DIR)/gen-ruby.h $(SRC_DIR)/libUtility.h 

SRCS 	 := 	$(SRC_DIR)/macro-decl.c	  \
		$(SRC_DIR)/libUtility.c	$(SRC_DIR)/class-decl.c	  \
		$(SRC_DIR)/main.c	  \
		$(SRC_DIR)/statics.c	  \
		$(SRC_DIR)/util.c

OBJS 	 :=	$(subst $(SRC_DIR), $(OBJ_DIR),$(subst .c,.o, $(SRCS))) $(LIB_OBJS)

ifdef DO_TESTING
TEST_SRCS    :=	$(TEST_DIR)/test.c
TEST_OBJS    :=	$(subst $(TEST_DIR), $(OBJ_DIR),$(subst .c,.o, $(TEST_SRCS)))
TEST_LDFLAGS := -L$(BUILD_DIR) $(TEST_LDFLAGS)
TEST_TARGETS := $(subst .o,,$(subst OBJ,BUILD, $(TEST_OBJS)))
endif

first: preq depend build

build:	$(BUILD_DIR)/$(TARGET)

ifdef DO_TESTING
test: 	.phony first
	@ $(CC) $(TEST_CFLAGS) $(TEST_DIR)/test.c $(TEST_LDFLAGS) -o $(BUILD_DIR)/test	
endif

.phony:
	@ touch .phony
	@ - $(RM) .phony

$(BUILD_DIR)/$(TARGET) : $(OBJS)
ifdef BUILDING_LIB
	@ $(CC) -shared $(OBJS) $(CFLAGS) -Wl $(LDFLAGS) -o $(BUILD_DIR)/$(TARGET)
else
	@ $(CC) $(OBJS) $(CFLAGS) $(LDFLAGS) -o $(BUILD_DIR)/$(TARGET)
endif

# checkout sources here
preq: .phony
#	@  - [ ! -d $(LIB_UTILITY_DIR) ] &&  svn co "$(SVN_DIR)/libutility-$(LIB_UTILITY_VER)" "$(LIB_UTILITY_DIR)" ;

depend: preq

# DEPENDENCY GENERATION TARGET
# 1) actual generation of dependency listing
# 2) prepending value of OBJ_DIR to each build target
	@ $(ECHO) "entering depend"
ifndef BUILD_DIR
	@ $(ECHO) "BUILD_DIR is unset"
	@ false
endif
ifndef CFLAGS
	@ $(ECHO) "CFLAGS is unset"
	@ false
endif
ifndef SRC_DIR
	@ $(ECHO) "SRC_DIR is unset"
	@ false
endif
ifndef BUILD_DIR
	@ $(ECHO) "BUILD_DIR is unset"
	@ false
endif
ifndef OBJ_DIR
	@ $(ECHO) "OBJ_DIR is unset"
	@ false
endif
ifndef TARGET
	@ $(ECHO) "TARGET is unset"
	@ false
endif

ifdef VERBOSE
	@ $(ECHO) "Generating Dependencies for $(BUILD_DIR)/$(TARGET)"
	@ $(ECHO) "LDFLAGS=[$(LDFLAGS)]"
	@ $(ECHO) "CFLAGS=[$(CFLAGS)]"
	@ $(ECHO) "SRC_DIR=[$(SRC_DIR)]"
	@ $(ECHO) "SRCS=[$(SRCS)]"
	@ $(ECHO) "OBJS=[$(OBJS)]"
	@ $(ECHO) "DEPS=[$(DEPS)]"
	@ $(ECHO) "BUILD_ROOT=[$(BUILD_ROOT)]"
	@ $(ECHO) "BUILD_DIR=[$(BUILD_DIR)]"
	@ $(ECHO) "OBJ_DIR=[$(OBJ_DIR)]"
	@ $(ECHO) "TARGET=[$(TARGET)]"
	@ $(ECHO) "TEST TARGETS=[$(TEST_TARGETS)]"
endif
	@- [ -d "$(BUILD_DIR)" ] || mkdir -p $(BUILD_DIR) && $(ECHO) "created $(BUILD_DIR)"
	@- [ -d "$(OBJ_DIR)" ]   || mkdir $(OBJ_DIR) && $(ECHO) "created $(OBJ_DIR)"
	@ $(ECHO) "$(BUILD_TYPE) on :" `date +"%F %T "` "Version: $(BUILD_DATE)" 
	@ $(CC) -MM $(CFLAGS) $(SRCS) $(HDRS) $(TEST_SRCS) > $(DEPS)
	@ $(ECHO) "leaving  depend\n"

clean: .phony lib-clean
	@ $(ECHO) "entering clean"
	@ $(RM) -r $(BUILD_DIR)
	@ $(ECHO) "leaving  clean\n"

# INSTALL TARGET (requires root access)
# 1) copy target to final destination
# 2) chmod target to final permisions
install: .phony build
	@ $(ECHO) "entering install"
	! ( $(INSTALL) $(INSTALL_PERM) $(BUILD_DIR)/$(TARGET) $(INSTALL_DIR) ) || $(ECHO) "$(TARGET) is installed"
	@ $(ECHO) "leaving  install\n"

all:	clean depend build

lib-clean: .phony
#	@ - [ -d $(LIB_UTILITY_DIR) ]  && $(RM) -r "$(LIB_UTILITY_DIR)" "$(LIB_DIR)"  ;

#########################################
# Build Directory Rules
#########################################
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@ $(CC) -c $< $(CFLAGS) -o $@ 

# magic to allow parallel build directories
$(OBJ_DIR)/%.o : $(LIB_UTILITY_SRC)/%.c
	@ $(CC) -c $< $(CFLAGS) -o $@ 

# magic to allow parallel build directories
$(OBJ_DIR)/%.o : $(TEST_DIR)/%.c
	@ $(CC) -c $< $(TEST_CFLAGS) -o  $@ 
#########################################
vpath %.c $(SRC_DIR)
vpath %.c $(LIB_UTILITY_SRC)
vpath %.o $(OBJ_DIR)
vpath %.so $(BUILD_DIR)
vpath %$(DEPS) $(OBJ_DIR)

-include $(DEPS)
