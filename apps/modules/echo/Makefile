include ../../../build.mk/astyle.mk

CC=gcc
CXX=g++
AR=ar
ARFLAGS=cru

PWD := $(shell pwd)
MPIC_PATH := $(PWD)/../../..
EVPP_PATH := $(PWD)/../../../../evpp

DEBUG := -D_DEBUG

CFLAGS=$(TOP_CFLAGS) -g -c -fPIC $(DEBUG) \
	   -Wall -Wno-unused-parameter -Wno-unused-function -Wunused-variable -Wunused-value \
	   -Wshadow -Wcast-qual -Wcast-align -Wwrite-strings -Wsign-compare \
	   -Winvalid-pch -fms-extensions -Wfloat-equal -Wextra \
	   -I . -I $(MPIC_PATH) -I $(EVPP_PATH) -MMD
CPPFLAGS= $(CFLAGS) -Woverloaded-virtual -Wsign-promo -fno-gnu-keywords -std=c++11

RTFLAGS = \
         -Wl,-rpath=. \
         -Wl,-rpath=$(MPIC_PATH)/mpic \
         -Wl,-rpath=$(EVPP_PATH)/build-release/lib \
         -Wl,-rpath=$(EVPP_PATH)/build-debug/lib \

LDFLAGS = $(RTFLAGS) -lglog -lpthread -lmpic -levpp \
         -L$(MPIC_PATH)/mpic \
         -L$(EVPP_PATH)/build-release/lib \
         -L$(EVPP_PATH)/build-debug/lib \

SRCS := $(wildcard *.cc) 
OBJS := $(patsubst %, %.o, $(SRCS))
DEPS := $(patsubst %.o, %.d, $(OBJS))

TARGET_SO = libechomodule.so

all : $(TARGET_SO)

$(TARGET_SO) : $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -shared -o $@
	
%.cc.o : %.cc 
	$(CXX) $(CPPFLAGS) $< -o $@

%.c.o : %.c 
	$(CC) $(CFLAGS) $< -o $@

-include $(DEPS)

fmt:
	$(ASTYLE) *.cc *.h
	rm -rf *.orig

install: $(TARGET_A) $(TARGET_SO) 

clean:
	rm -rf $(OBJS) $(DEPS) $(TARGET_SO)* $(TARGET_A) *.d *.o

