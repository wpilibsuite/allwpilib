prefix = /usr
lib.dir = $(prefix)/lib
plugin.dir = $(lib.dir)/frcsim/plugins
build.dir = build

all:
	mkdir -p $(build.dir)
	cd ${build.dir} && cmake .. && make

clean:
	rm -rf $(build.dir)

install: all
	mkdir -p $(DESTDIR)$(plugin.dir)
	install $(build.dir)/libfrc_gazebo_plugin.so $(DESTDIR)$(plugin.dir)
