# Get the directory of this Makefile
export MKFILE_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
# -------- Directory layout --------
BUILDDIR := $(MKFILE_DIR)/build
OBJDIR   := $(BUILDDIR)
LIBDIR   := $(MKFILE_DIR)/lib
DATA	 := $(MKFILE_DIR)/data

# Check if the DEVKITARM variable is set
ifeq ($(strip $(DEVKITARM)),)
# Not on GBA. set bin2s dir to tools dir,
# because we need to use our locally built tool. (see bin2s_tool target below)
export BIN2S_DIR := $(MKFILE_DIR)/tools/bin2s/
else
# On GBA, bin2s comes with devkitPro.
export BIN2S_DIR :=
endif

#---------------------------------------------------------------------------------
# canned command sequence for converting binary data
# Taken from https://github.com/devkitPro/devkitarm-rules/blob/master/base_tools
#---------------------------------------------------------------------------------
define bin2o
	$(eval _tmpasm := $(shell mktemp))
	$(SILENTCMD)$(BIN2S_DIR)bin2s -a 4 -H `(echo $(<F) | tr . _)`.h $< > $(_tmpasm)
	$(SILENTCMD)$(CC) -x assembler-with-cpp $(CPPFLAGS) $(ASFLAGS) -c $(_tmpasm) -o $(<F).o
	@rm $(_tmpasm)
endef

# Like Poke_Transporter_GB, this is a multi-stage Makefile.
# If we are not in the build/ subdirectory, we are in the top-level dir.
# We need the multiple stages because we need to generate the tables before we can compile
# the rest of our library. (because we need them to get picked up in BINFILES)
ifneq (build,$(notdir $(CURDIR)))
# -------- Cleanup --------
# -------- Top-level targets --------
.PHONY: all lib clean dirs symlinks
all: dirs generate_tables lib

lib: generate_tables
	@$(MAKE) -C build -f $(MKFILE_DIR)/Makefile
	cp build/*.h lib/include/
	cp -r include lib/

# Ensure the build directories exist
dirs:
	@mkdir -p $(OBJDIR) $(LIBDIR) $(LIBDIR)/include

generate_tables:
	mkdir -p data
	mkdir -p to_compress
	@env -i "PATH=$(PATH)" $(MAKE) -C tools/table-generator
	@echo
	@echo "----------------------------------------------------------------"
	@echo
	@tools/table-generator/table-generator to_compress
ifeq ($(CXX),arm-none-eabi-g++)
	@echo "Compressing bin files for GBA!"
	@echo -n "["
	@find to_compress -name "*.bin" -print0 | xargs -0 -n1 ./compress_lz10.sh
	@echo "]"
	@echo "Compressing finished!"
else
	rm -rf data
	mv to_compress data
endif
	@echo
	@echo "----------------------------------------------------------------"
	@echo

clean:
	$(MAKE) -C tools/table-generator clean
	$(MAKE) -C tools/bin2s clean
	rm -rf $(BUILDDIR) $(MKFILE_DIR)/data $(MKFILE_DIR)/to_compress $(LIBDIR)

else
# -------- Toolchain defaults (can be overridden by parent) --------
CC       ?= gcc
CXX      ?= g++
AR       ?= ar
CFLAGS   ?= -O2
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

# -------- Automatic source scanning --------
C_SOURCES   := $(wildcard $(MKFILE_DIR)/source/*.c)
CPP_SOURCES := $(wildcard $(MKFILE_DIR)/source/*.cpp)
SOURCES     := $(C_SOURCES) $(CPP_SOURCES)
INCLUDES    := $(MKFILE_DIR)/include $(INCLUDE) -I$(OBJDIR)
BINFILES	:= $(wildcard $(DATA)/*.bin)

BINOFILES := $(addprefix $(OBJDIR)/,$(notdir $(BINFILES:.bin=.bin.o)))

# create object paths
OBJS := $(addprefix $(OBJDIR)/,$(notdir $(C_SOURCES:.c=.o)))
OBJS += $(addprefix $(OBJDIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))
OBJS += $(BINOFILES)
#OBJS += $(patsubst $(DATA)/%.bin,$(OBJDIR)/%.bin.o,$(BINFILES))

VPATH += $(MKFILE_DIR)/source $(DATA)

# -------- Top-level targets --------
.PHONY: all symlinks

ifeq ($(strip $(DEVKITARM)),)
# Not on GBA. Compile the shared lib as well.
# We need -fPIC for shared libs, but it's not supported on GBA.
CFLAGS += -fPIC
all: bin2s_tool $(LIBDIR)/$(STATIC_LIB) $(LIBDIR)/$(SHARED_REAL) symlinks
else
# On GBA. Only compile the static lib.
all: $(LIBDIR)/$(STATIC_LIB)
endif


# -------- Static library --------
$(LIBDIR)/$(STATIC_LIB): $(BINOFILES) $(OBJS)
	$(AR) rcs $@ $(OBJS)


# -------- Shared library (real file with full version) --------
$(LIBDIR)/$(SHARED_REAL): $(BINOFILES) $(OBJS)
	$(CXX) $(SOFLAGS) $(LDFLAGS) \
	       -Wl,-soname,$(SHARED_MAJOR) \
	       -o $@ $(OBJS)


# -------- Soname symlinks --------
symlinks: $(LIBDIR)/$(SHARED_REAL)
	ln -sf $(SHARED_REAL) $(LIBDIR)/$(SHARED_MAJOR)
	ln -sf $(SHARED_MAJOR) $(LIBDIR)/$(SHARED_UNVER)

# This tool is normally bundled with devkitPro, but it's not available on Linux or Windows.
# Since we want to allow people to build PCCS independently of Poke_Transporter_GB,
# we have bundled a copy of this tool in the tools/ directory.
# But that means we need to build it ourselves here when not building for GBA.
bin2s_tool:
	$(MAKE) -C $(MKFILE_DIR)/tools/bin2s

# -------- Object file rules --------
$(OBJDIR)/%.bin.o $(OBJDIR)/%.bin.h: %.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(INCLUDES) -c -o $@ $<

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(INCLUDES) -c -o $@ $<

endif
