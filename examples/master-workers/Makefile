
CC := gcc
CXX := g++

CURRENT_DIR=$(shell echo `pwd`)
MPIC_DIR=$(CURRENT_DIR)/../..
LIBDAEMON_PATH := $(MPIC_DIR)/3rdparty/libdaemon

CFLAGS=-g -c -fPIC -D_DEBUG \
	   -Wall -Wno-unused-parameter -Wno-unused-function -Wunused-variable -Wunused-value \
	   -Wshadow -Wcast-qual -Wcast-align -Wwrite-strings -Wsign-compare \
	   -Winvalid-pch -fms-extensions -Wfloat-equal -Wextra \
	   -I . -I $(LIBDAEMON_PATH) -D'LOCALSTATEDIR="./"' -MMD

CPPFLAGS= $(CFLAGS) -Woverloaded-virtual -Wsign-promo -fno-gnu-keywords -std=c++11
LDFLAGS = -lglog -lgflags

SRCS := $(wildcard *.cc) $(wildcard $(LIBDAEMON_PATH)/libdaemon/*.c)
OBJS := $(patsubst %, %.o, $(SRCS))
DEPS := $(patsubst %.o, %.d, $(OBJS))

TARGET=master_workers_test

all : $(TARGET) 

check : $(TARGET)
	./$^

$(TARGET) : $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

-include $(DEPS)

%.cc.o : %.cc
	$(CXX) $(CFLAGS) $(CPPFLAGS) $< -o $@

%.c.o : %.c 
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o *.d $(OBJS) $(DEPS) $(TARGET)

