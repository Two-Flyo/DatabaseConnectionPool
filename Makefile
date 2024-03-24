# Makefile

# Compiler and linker configurations
CXX = g++
CXXFLAGS = -Wall -Iinclude -std=c++11 -g
LDFLAGS = -lmysqlclient -lpthread

# Define the target executable
TARGET = Test
# Define source directory and build directory
SRC_DIR = src
OBJ_DIR = obj

# Automatically list all the source files in src and corresponding object files in obj
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Default target
all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJECTS)
	$(CXX) $^ -o $@ $(LDFLAGS)

# Compile the source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up the build
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Prevent make from confusing the clean target with a file name
.PHONY: all clean
