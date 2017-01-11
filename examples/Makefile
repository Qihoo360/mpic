
include ../build.mk/astyle.mk

SUBDIRS = daemon monitor-worker master-workers

all : prepare
	for t in $(SUBDIRS); do $(MAKE) -C $$t; done

clean:
	for t in $(SUBDIRS); do $(MAKE) clean -C $$t; done

fmt:
	for t in $(SUBDIRS); do cd $$t && $(ASTYLE) *.cc && rm -f *.orig && cd -; done

prepare :
	$(MAKE) -C ../mpic/
