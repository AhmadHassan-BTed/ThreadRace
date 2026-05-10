# Compiler and Flags
CXX      := g++
CXXFLAGS := -std=c++11 -O3 -Wall -Wextra
INCLUDES := -Iinclude
LIBS     := -pthread

# Directories
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

# Source Files
SRCS := $(SRC_DIR)/main.cpp \
        $(SRC_DIR)/Benchmarker.cpp \
        $(SRC_DIR)/tasks/AckermannTask.cpp \
        $(SRC_DIR)/strategies/SequentialStrategy.cpp \
        $(SRC_DIR)/strategies/ThreadedStrategy.cpp

# Object Files
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Target Binary
TARGET := $(BIN_DIR)/time_tester

# Helper for directory creation
ifeq ($(OS),Windows_NT)
    MKDIR_P = powershell -Command "if (!(Test-Path '$(subst /,\,$(1))')) { New-Item -ItemType Directory -Path '$(subst /,\,$(1))' -Force | Out-Null }"
    TARGET := $(BIN_DIR)/time_tester.exe
    RM = del /Q /S
else
    MKDIR_P = mkdir -p $(1)
    RM = rm -rf
endif

# Default Target
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	@$(call MKDIR_P,$(BIN_DIR))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

# Compile
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

.PHONY: all clean test
