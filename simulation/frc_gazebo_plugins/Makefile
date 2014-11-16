prefix = /usr
lib.dir = $(prefix)/lib
plugin.dir = $(lib.dir)/frcsim/plugins
build.dir = build

ext = .so
ifeq ($(shell uname), Darwin)
	ext = .dylib
endif

.PHONY : all build copy-plugins clean docs clean-docs

all: build copy-plugins

build:
	cd msgs && make
	cd dc_motor && make
	cd pneumatic_piston && make
	cd potentiometer && make
	cd rangefinder && make
	cd encoder && make
	cd gyro && make
	cd limit_switch && make
	cd clock && make

copy-plugins:
	mkdir -p plugins
	cp msgs/build/libgz_msgs$(ext) plugins
	cp dc_motor/build/libgz_dc_motor$(ext) plugins
	cp pneumatic_piston/build/libgz_pneumatic_piston$(ext) plugins
	cp potentiometer/build/libgz_potentiometer$(ext) plugins
	cp rangefinder/build/libgz_rangefinder$(ext) plugins
	cp encoder/build/libgz_encoder$(ext) plugins
	cp gyro/build/libgz_gyro$(ext) plugins
	cp limit_switch/build/libgz_limit_switch$(ext) plugins
	cp clock/build/libgz_clock$(ext) plugins

clean: clean-docs
	cd msgs && make clean
	cd dc_motor && make clean
	cd pneumatic_piston && make clean
	cd potentiometer && make clean
	cd rangefinder && make clean
	cd encoder && make clean
	cd gyro && make clean
	cd limit_switch && make clean
	cd clock && make clean
	-rm -r plugins

install: all
	mkdir -p $(DESTDIR)$(plugin.dir)
	install plugins/* $(DESTDIR)$(plugin.dir)

docs:
	doxygen frc_gazebo_plugins.doxy

clean-docs:
	-rm -r docs
