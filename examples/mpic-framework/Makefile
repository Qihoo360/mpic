
CXX := g++

CURRENT_DIR=$(shell echo `pwd`)
MPIC_DIR=$(CURRENT_DIR)/../..

CPPFLAGS= -g -c -fPIC \
		  -D_DEBUG \
		  -Wshadow -Wcast-qual -Wcast-align -Wwrite-strings \
		  -Wsign-compare -Winvalid-pch -fms-extensions -Wall \
		  -MMD -Woverloaded-virtual -Wsign-promo -fno-gnu-keywords -std=c++11\
		  -I $(MPIC_DIR)\
		  -I $(MPIC_DIR)/3rdparty/libdaemon \

RTFLAGS := \
    -Wl,-rpath=. \
    -Wl,-rpath=$(MPIC_DIR)/mpic

LDFLAGS= $(RTFLAGS) \
		 -L$(MPIC_DIR)/mpic \
		 -lmpic -lglog -lgflags

SRCS := $(wildcard *.cc)
OBJS := $(patsubst %.cc, %.o, $(SRCS))
DEPS := $(patsubst %.o, %.d, $(OBJS))

TARGET=mpic-framework

all : $(TARGET) 

check : $(TARGET)
	./$^

$(TARGET) : $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

-include $(DEPS)

%.o : %.cc
	$(CXX) $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -rf *.o *.d $(OBJS) $(DEPS) $(TARGET)

