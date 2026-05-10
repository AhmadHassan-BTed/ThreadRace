CXX = g++
CXXFLAGS = -Wall -pthread -O2
SRC = src/main.cpp
TARGET = time_tester

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) a.out

test: $(TARGET)
	@echo "Testing Typical Mode..."
	time ./$(TARGET) 1
	@echo "Testing Threaded Mode..."
	time ./$(TARGET) 2

.PHONY: all clean test
