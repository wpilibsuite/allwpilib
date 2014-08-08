prefix = /usr
lib.dir = $(prefix)/lib
build.dir = build
allwpilib = ../..

all:
	mkdir -p $(build.dir)
	cp -r $(allwpilib)/wpilibc/wpilibC++ build
	cd ${build.dir} && ALLWPILIB=$(allwpilib) cmake .. && make

clean:
	rm -rf $(build.dir)

install: all
	mkdir -p $(DESTDIR)$(lib.dir)
	install $(build.dir)/libWPILibSim.so $(DESTDIR)$(lib.dir)
