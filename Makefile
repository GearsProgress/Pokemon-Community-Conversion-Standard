# -------- Toolchain defaults (can be overridden by parent) --------
CC       ?= gcc
CXX      ?= g++
AR       ?= ar
CFLAGS   ?= -Os -fPIC -fno-rtti -fno-exceptions -fno-unwind-tables -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -g
CXXFLAGS ?= $(CFLAGS)
LDFLAGS  ?=
SOFLAGS  ?= -shared

# -------- Library name + versioning --------
TARGET         := pccs
MAJOR_VERSION  := 1
MINOR_VERSION  := 0
FULL_VERSION   := $(MAJOR_VERSION).$(MINOR_VERSION)

STATIC_LIB     := lib$(TARGET).a
SHARED_REAL    := lib$(TARGET).so.$(FULL_VERSION)
SHARED_MAJOR   := lib$(TARGET).so.$(MAJOR_VERSION)
SHARED_UNVER   := lib$(TARGET).so

# -------- Directory layout --------
# Get the directory of this Makefile
MKFILE_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
BUILDDIR := $(MKFILE_DIR)/build
OBJDIR   := $(BUILDDIR)
LIBDIR   := $(MKFILE_DIR)

# -------- Automatic source scanning --------
C_SOURCES   := $(wildcard source/*.c)
CPP_SOURCES := $(wildcard source/*.cpp)
SOURCES     := $(C_SOURCES) $(CPP_SOURCES)
INCLUDES    := $(MKFILE_DIR)/include $(INCLUDE)

# create object paths
OBJS := $(patsubst %.c,$(OBJDIR)/%.o,$(C_SOURCES))
OBJS += $(patsubst %.cpp,$(OBJDIR)/%.o,$(CPP_SOURCES))

# -------- Top-level targets --------
.PHONY: all clean dirs symlinks
all: dirs $(LIBDIR)/$(STATIC_LIB) $(LIBDIR)/$(SHARED_REAL) symlinks


# Ensure the build directories exist
dirs:
	@mkdir -p $(OBJDIR) $(LIBDIR)


# -------- Static library --------
$(LIBDIR)/$(STATIC_LIB): $(OBJS)
	$(AR) rcs $@ $(OBJS)


# -------- Shared library (real file with full version) --------
$(LIBDIR)/$(SHARED_REAL): $(OBJS)
	$(CXX) $(SOFLAGS) $(LDFLAGS) \
	       -Wl,-soname,$(SHARED_MAJOR) \
	       -o $@ $(OBJS)


# -------- Soname symlinks --------
symlinks: $(LIBDIR)/$(SHARED_REAL)
	ln -sf $(SHARED_REAL) $(LIBDIR)/$(SHARED_MAJOR)
	ln -sf $(SHARED_MAJOR) $(LIBDIR)/$(SHARED_UNVER)


# -------- Object file rules --------
$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCLUDES) -c -o $@ $<

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(INCLUDES) -c -o $@ $<


# -------- Cleanup --------
clean:
	rm -rf $(BUILDDIR) $(LIBDIR)/*.a $(LIBDIR)/*.so*
