
SUBDIRS = daemon monitor-worker master-workers

all : 
	for t in $(SUBDIRS); do $(MAKE) -C $$t; done

clean:
	for t in $(SUBDIRS); do $(MAKE) clean -C $$t; done

