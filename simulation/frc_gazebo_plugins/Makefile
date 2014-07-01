prefix = /usr
lib.dir = $(prefix)/lib
plugin.dir = $(lib.dir)/frcsim/plugins
build.dir = build

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
	cp dc_motor/build/libgz_dc_motor.so plugins
	cp pneumatic_piston/build/libgz_pneumatic_piston.so plugins
	cp potentiometer/build/libgz_potentiometer.so plugins
	cp rangefinder/build/libgz_rangefinder.so plugins
	cp encoder/build/libgz_encoder.so plugins
	cp gyro/build/libgz_gyro.so plugins
	cp limit_switch/build/libgz_limit_switch.so plugins
	cp clock/build/libgz_clock.so plugins

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

docs:
	doxygen frc_gazebo_plugins.doxy

clean-docs:
	-rm -r docs
