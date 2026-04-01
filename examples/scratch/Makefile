# Source files directory
SRC_DIR := $(CURDIR)

PCCS_INC_DIR := $(CURDIR)/../../include

# Build directory
BUILD_DIR := build

# # Compiler flags
CXXFLAGS := -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-unused-variable -I $(CURDIR) -I $(PCCS_INC_DIR) -g -Os

# Source files (add more as needed)
SRCS := $(shell find $(SRC_DIR) -type f -name '*.cpp')
# Object files
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

LIBPCCS := ../../lib/libpccs.a

# Ensure necessary directories exist
# This function ensures the directory for the target exists
define make_directory
	@mkdir -p $(dir $@)
endef

# Target executable
TARGET := example

# Phony targets
.PHONY: all clean

# Default target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBPCCS)

# Rule to compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(make_directory)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create the build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean rule
clean:
	rm -rf $(BUILD_DIR) $(TARGET)