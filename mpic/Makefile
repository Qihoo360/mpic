include ../rpm/build.mk
include ../build.mk/astyle.mk

CC=gcc
CXX=g++
AR=ar
ARFLAGS=cru

PWD := $(shell pwd)
LIBDAEMON_PATH := ../3rdparty/libdaemon

DEBUG := -D_DEBUG

CFLAGS=$(TOP_CFLAGS) -g -c -fPIC $(DEBUG) \
	   -Wall -Wno-unused-parameter -Wno-unused-function -Wunused-variable -Wunused-value \
	   -Wshadow -Wcast-qual -Wcast-align -Wwrite-strings -Wsign-compare \
	   -Winvalid-pch -fms-extensions -Wfloat-equal -Wextra \
	   -I . -I .. -I $(LIBDAEMON_PATH) -D'LOCALSTATEDIR="./"' -MMD
CPPFLAGS= $(CFLAGS) -Woverloaded-virtual -Wsign-promo -fno-gnu-keywords -std=c++11

RTFLAGS := \
    -Wl,-rpath=. \

LDFLAGS= $(RTFLAGS) -lpthread -lglog \

SRCS := $(wildcard *.cc) $(wildcard $(LIBDAEMON_PATH)/libdaemon/*.c)
OBJS := $(patsubst %, %.o, $(SRCS))
DEPS := $(patsubst %.o, %.d, $(OBJS))

LIBNAME=mpic
TARGET_SO=lib$(LIBNAME).so
TARGET_A=lib$(LIBNAME).a

all : $(TARGET_SO) $(TARGET_A)
	$(MAKE) install

$(TARGET_A) : $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS) 
	ranlib $@			   

$(TARGET_SO) : $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -shared -o $@.$(FULL)
	@rm -fr $(@)
	@rm -fr $(@).$(MAJOR)
	ln -s $(@).$(FULL) $(@).$(MAJOR)
	ln -s $(@).$(MAJOR) $(@)
	
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

