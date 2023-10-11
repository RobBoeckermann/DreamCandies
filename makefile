CC = g++
CPPFLAGS = -g -Wall -std=c++17
INCLUDES = -I/usr/include/gtest
LDFLAGS = -lgtest -lgtest_main

SRCS := $(shell find . -type f -name '*.cpp')
EXECS = $(patsubst %.cpp,%,$(SRCS))

all: $(EXECS)

$(EXECS): %: %.cpp
	$(CC) $(CPPFLAGS) -o $@ $< $(INCLUDES) $(LDFLAGS)


build: $(SRC:.cpp=)

run: $(SRC:.cpp=)
	./$(SRC:.cpp=)

clean:
	$(RM) $(EXECS)
