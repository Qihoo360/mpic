
include ../build.mk/astyle.mk

EXAMPLES = daemon monitor-worker master-workers

all : prepare
	for t in $(EXAMPLES); do $(MAKE) -C $$t; done

clean:
	for t in $(EXAMPLES); do $(MAKE) clean -C $$t; done

fmt:
	for t in $(EXAMPLES); do cd $$t && $(ASTYLE) *.cc && rm -f *.orig && cd -; done

prepare :
	$(MAKE) -C ../mpic/
