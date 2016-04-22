CXX := g++
CXXFLAGS := -Wall -g -O2
LDFLAGS :=
LDLIBS :=

SRCS := $(wildcard *.cpp)
TARGETS := $(patsubst %.cpp, %, ${SRCS})

.PHONY: all clean

all: ${TARGETS}

clean:
	${RM} ${TARGETS}

${TARGETS}: %: %.cpp
	$(CXX) ${CXXFLAGS} -o $@ $^ ${LDFLAGS}
