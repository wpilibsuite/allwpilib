prefix = /usr
lib.dir = $(prefix)/lib
build.dir = build

all:
	mkdir -p $(build.dir)
	cd ${build.dir} && cmake .. && make

clean:
	rm -rf $(build.dir)

install: all
	mkdir -p $(DESTDIR)$(lib.dir)
	install $(build.dir)/libWPILibSim.so $(DESTDIR)$(lib.dir)
	install $(build.dir)/build/wpilibC++/libWPILib.a $(DESTDIR)$(lib.dir)
