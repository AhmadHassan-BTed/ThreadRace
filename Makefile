# Compiler and Flags
CXX      := g++
CXXFLAGS := -std=c++11 -g -O2 -Wall -Wextra
INCLUDES := -Iinclude
LIBS     := -pthread

VISUALIZER_LIBS := -lSDL2 -lGL -pthread

# Directories
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

# Source Files for Core Benchmark
SRCS := $(SRC_DIR)/main.cpp \
        $(SRC_DIR)/Benchmarker.cpp \
        $(SRC_DIR)/tasks/AckermannTask.cpp \
        $(SRC_DIR)/strategies/SequentialStrategy.cpp \
        $(SRC_DIR)/strategies/ThreadedStrategy.cpp

# Source Files for Visualizer
VISUALIZER_SRCS := $(SRC_DIR)/visualizer/visualizer_main.cpp \
                   $(SRC_DIR)/visualizer/AckermannStackEngine.cpp \
                   $(SRC_DIR)/visualizer/AudioSynthesizer.cpp \
                   $(SRC_DIR)/visualizer/QuantumTreeRenderer.cpp \
                   $(SRC_DIR)/visualizer/Landscape3DRenderer.cpp \
                   $(SRC_DIR)/visualizer/SpiralOrbitRenderer.cpp \
                   $(SRC_DIR)/visualizer/RaceEngine.cpp \
                   $(SRC_DIR)/visualizer/RaceArenaRenderer.cpp \
                   $(SRC_DIR)/visualizer/FontRenderer.cpp

# Object Files
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
VISUALIZER_OBJS := $(VISUALIZER_SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Target Binaries
TARGET := $(BIN_DIR)/time_tester
VISUALIZER_TARGET := $(BIN_DIR)/ackermann_visualizer

# Helper for directory creation
ifeq ($(OS),Windows_NT)
    MKDIR_P = powershell -Command "if (!(Test-Path '$(subst /,\,$(1))')) { New-Item -ItemType Directory -Path '$(subst /,\,$(1))' -Force | Out-Null }"
    TARGET := $(BIN_DIR)/time_tester.exe
    VISUALIZER_TARGET := $(BIN_DIR)/ackermann_visualizer.exe
    RM = del /Q /S
else
    MKDIR_P = mkdir -p $(1)
    RM = rm -rf
endif

# Default Target
all: $(TARGET) $(VISUALIZER_TARGET)

# Link Core Benchmark Target
$(TARGET): $(OBJS)
	@$(call MKDIR_P,$(BIN_DIR))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Link C++ Visualizer Target
$(VISUALIZER_TARGET): $(VISUALIZER_OBJS)
	@$(call MKDIR_P,$(BIN_DIR))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(VISUALIZER_LIBS)

# Compile Object Files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(call MKDIR_P,$(dir $@))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

# Clean
clean:
ifeq ($(OS),Windows_NT)
	if exist $(OBJ_DIR) rmdir /S /Q $(OBJ_DIR)
	if exist $(BIN_DIR) rmdir /S /Q $(BIN_DIR)
else
	$(RM) $(OBJ_DIR) $(BIN_DIR)
endif

# Test
test: all
	@echo Running Sequential Test...
	$(TARGET) 1 1
	@echo Running Threaded Test...
	$(TARGET) 2 1

.PHONY: all clean test visualizer
