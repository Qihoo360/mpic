include ../rpm/build.mk

CC=gcc
CXX=g++
AR=ar
ARFLAGS=cru
CURRENT_DIR=$(shell echo `pwd`)
SOURCE_DIR=$(CURRENT_DIR)

LIBDAEMON_PATH := ../3rdparty/libdaemon

DEBUG := -D_DEBUG

CFLAGS=$(TOP_CFLAGS) -g -c -fPIC $(DEBUG) \
	   -Wall -Wno-unused-parameter -Wno-unused-function -Wunused-variable -Wunused-value \
	   -Wshadow -Wcast-qual -Wcast-align -Wwrite-strings -Wsign-compare \
	   -Winvalid-pch -fms-extensions -Wfloat-equal -Wextra \
	   -I .. -I $(LIBDAEMON_PATH) -D'LOCALSTATEDIR="./"' -MMD
CPPFLAGS= $(CFLAGS) -Woverloaded-virtual -Wsign-promo -fno-gnu-keywords -std=c++11

RTFLAGS := \
    -Wl,-rpath=.

LDFLAGS= $(RTFLAGS) -lpthread \


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
	
check : $(TARGET_A) $(TARGET_SO)	
	$(MAKE) install
	$(MAKE) check -C test
	$(MAKE) check -C quick_test
	$(MAKE) check -C quick_gtest

%.cc.o : %.cc 
	$(CXX) $(CPPFLAGS) $< -o $@

%.c.o : %.c 
	$(CC) $(CFLAGS) $< -o $@

-include $(DEPS)

t:
	@echo "LIBNAME:" $(LIBNAME)
	@echo "SRCS:" $(SRCS)
	@echo "OBJS:" $(OBJS)
	@echo "CFLAGS:" $(CFLAGS)
	@echo "CPPFLAGS:" $(CPPFLAGS)

install: $(TARGET_A) $(TARGET_SO) 

clean:
	rm -rf $(OBJS) $(DEPS) $(TARGET_SO) $(TARGET_A)

