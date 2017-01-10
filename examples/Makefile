
EXAMPLES = daemon monitor-worker

all : prepare
	for t in $(EXAMPLES); do $(MAKE) -C $$t; done

clean:
	for t in $(EXAMPLES); do $(MAKE) clean -C $$t; done

prepare :
	$(MAKE) -C ../mpic/
