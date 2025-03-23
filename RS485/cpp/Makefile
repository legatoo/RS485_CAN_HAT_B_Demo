CXX = g++
CC = gcc
CXXFLAGS = -Wall -std=c++11 -I. -Ilib/Config -DUSE_DEV_LIB
CFLAGS = -Wall -I. -Ilib/Config -DUSE_DEV_LIB
LDFLAGS = -pthread

TARGET = pressure_reader
SRCS = main.cpp
CSRCS = lib/Config/DEV_Config.c lib/Config/sysfs_gpio.c lib/Config/dev_hardware_UART.c
OBJS = $(SRCS:.cpp=.o) $(CSRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean 