CXX = g++
CXXFLAGS = -std=c++11 -I. -pthread
SRCS = main.cpp pkg/huffman.cpp pkg/file_io.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = huffman_server

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

main.o: main.cpp pkg/file_io.h
huffman.o: pkg/huffman.cpp pkg/huffman.h
file_io.o: pkg/file_io.cpp pkg/file_io.h pkg/huffman.h

clean:
	rm -f $(TARGET) $(OBJS) temp_input temp_compressed.bin temp_decompressed.txt
