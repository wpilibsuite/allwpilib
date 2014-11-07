codename=trusty
allwpilib=../..

package-version = 1
gazebo-plugins-version = 0.2
gazebo-plugins-package-version = $(gazebo-plugins-version)-$(package-version)
gazebo-models-version = 0.3
gazebo-models-package-version = $(gazebo-models-version)-$(package-version)
gazebo-models-orig-url = https://usfirst.collab.net/sf/frs/do/downloadFile/projects.wpilib/frs.simulation.frcsim_gazebo_models/frs1070?dl=1
eclipse-plugins-version = 0.1
eclipse-plugins-package-version = $(eclipse-plugins-version)-$(package-version)
eclipse-toolchain-version = 0.1
eclipse-toolchain-package-version = $(eclipse-toolchain-version)-$(package-version)
libwpilibsim-version = 0.1
libwpilibsim-package-version = $(libwpilibsim-version)-$(package-version)
frcsim-version = 0.1
frcsim-package-version = $(frcsim-version)-$(package-version)

# Main Targets
.PHONY: all jenkins allwpilib pull debs update-repository clean-repository clean install

all: update-repository

jenkins: update-repository

allwpilib:
	cd $(allwpilib) && mvn -T 8 clean package -Dwith-eclipse-plugins -DskipTests -DskipIT

orig: orig-frcsim-gazebo-plugins orig-frcsim-gazebo-models orig-frcsim-eclipse-plugins \
	  orig-frcsim-libwpilibsim-cpp orig-frcsim

debs: debs-frcsim-gazebo-plugins debs-frcsim-gazebo-models debs-frcsim-eclipse-plugins \
	  debs-frcsim-libwpilibsim-cpp debs-frcsim

update-repository: debs clean-repository 
	cd repository && reprepro includedeb $(codename) ../frcsim-gazebo-plugins/frcsim-gazebo-plugins_$(gazebo-plugins-package-version)_amd64.deb
	cd repository && reprepro includedeb $(codename) ../frcsim-gazebo-plugins/frcsim-gazebo-plugins_$(gazebo-plugins-package-version)_i386.deb
	cd repository && reprepro includedeb $(codename) ../frcsim-gazebo-models/frcsim-gazebo-models_$(gazebo-models-package-version)_all.deb
	cd repository && reprepro includedeb $(codename) ../frcsim-eclipse-plugins/frcsim-eclipse-plugins_$(eclipse-plugins-package-version)_all.deb
	# cd repository && reprepro includedeb $(codename) ../frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin_$(eclipse-toolchain-package-version)_all.deb
	cd repository && reprepro includedeb $(codename) ../frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp_$(libwpilibsim-package-version)_amd64.deb
	cd repository && reprepro includedeb $(codename) ../frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp_$(libwpilibsim-package-version)_i386.deb
	cd repository && reprepro includedeb $(codename) ../frcsim/frcsim_$(frcsim-package-version)_all.deb
	echo Repository Updated

clean-repository:
	cd repository && reprepro remove $(codename) frcsim-gazebo-plugins
	cd repository && reprepro remove $(codename) frcsim-gazebo-models
	cd repository && reprepro remove $(codename) frcsim-eclipse-plugins
	# cd repository && reprepro remove $(codename) frcsim-eclipse-toolchain-plugin
	cd repository && reprepro remove $(codename) frcsim-libwpilibsim-cpp
	cd repository && reprepro remove $(codename) frcsim

clean:
	-rm frcsim-gazebo-models/frcsim-gazebo-models_$(gazebo-models-version).orig.tar.gz
	-rm frcsim-gazebo-plugins/frcsim-gazebo-plugins_$(gazebo-plugins-version).orig.tar.gz
	-rm frcsim-eclipse-plugins/frcsim-eclipse-plugins_$(eclipse-plugins-version).orig.tar.gz
	-rm frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin_$(eclipse-toolchain-version).orig.tar.gz
	-rm frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp_$(libwpilibsim-version).orig.tar.gz
	-rm frcsim/frcsim_$(frcsim-version).orig.tar.gz

	-rm frcsim-gazebo-plugins/frcsim-gazebo-plugins_$(gazebo-plugins-package-version)_amd64.deb
	-rm frcsim-gazebo-plugins/frcsim-gazebo-plugins_$(gazebo-plugins-package-version)_i386.deb
	-rm frcsim-gazebo-models/frcsim-gazebo-models_$(gazebo-models-package-version)_all.deb
	-rm frcsim-eclipse-plugins/frcsim-eclipse-plugins_$(eclipse-plugins-package-version)_all.deb
	-rm frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin_$(eclipse-toolchain-package-version)_all.deb
	-rm frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp_$(libwpilibsim-package-version)_amd64.deb
	-rm frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp_$(libwpilibsim-package-version)_i386.deb
	-rm -r frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp/allwpilib
	-rm frcsim/frcsim_$(frcsim-package-version)_all.deb

install:
	sudo dpkg -i frcsim-gazebo-plugins/frcsim-gazebo-plugins_$(gazebo-plugins-package-version)_amd64.deb \
	frcsim-gazebo-models/frcsim-gazebo-models_$(gazebo-models-package-version)_all.deb \
	frcsim-eclipse-plugins/frcsim-eclipse-plugins_$(eclipse-plugins-package-version)_all.deb \
	frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp_$(libwpilibsim-package-version)_amd64.deb \
	frcsim/frcsim_$(frcsim-package-version)_all.deb

# orig support (for faster parallel builds)
.PHONY: orig-frcsim-gazebo-plugins orig-frcsim-gazebo-models orig-frcsim-eclipse-plugins \
	  	orig-frcsim-eclipse-toolchain-plugin orig-frcsim-libwpilibsim-cpp orig-frcsim
.PHONY:	pull-gazebo-plugins pull-eclipse-plugins pull-libwpilibsim-cpp

orig-frcsim-gazebo-models: frcsim-gazebo-models/frcsim-gazebo-models_$(gazebo-models-version).orig.tar.gz
frcsim-gazebo-models/frcsim-gazebo-models_$(gazebo-models-version).orig.tar.gz:
	cd frcsim-gazebo-models && \
	    wget --no-check-certificate $(gazebo-models-orig-url) -O frcsim-gazebo-models_$(gazebo-models-version).orig.tar.gz && \
	    tar xvf frcsim-gazebo-models_$(gazebo-models-version).orig.tar.gz

orig-frcsim-gazebo-plugins: frcsim-gazebo-plugins/frcsim-gazebo-plugins_$(gazebo-plugins-version).orig.tar.gz
pull-gazebo-plugins:
	cp -rf -t frcsim-gazebo-plugins/frcsim-gazebo-plugins/ $(allwpilib)/simulation/frc_gazebo_plugins/*
	echo Increment version?
frcsim-gazebo-plugins/frcsim-gazebo-plugins_$(gazebo-plugins-version).orig.tar.gz: pull-gazebo-plugins
	cd frcsim-gazebo-plugins/frcsim-gazebo-plugins && debuild clean
	rm -f frcsim-gazebo-plugins/frcsim-gazebo-plugins_$(gazebo-plugins-version).orig.tar.gz
	cd frcsim-gazebo-plugins && tar --exclude="./debian" -czvf \
		frcsim-gazebo-plugins_$(gazebo-plugins-version).orig.tar.gz frcsim-gazebo-plugins

orig-frcsim-eclipse-plugins: frcsim-eclipse-plugins/frcsim-eclipse-plugins_$(eclipse-plugins-version).orig.tar.gz
pull-eclipse-plugins:
	rm -rf frcsim-eclipse-plugins/frcsim-eclipse-plugins/plugins
	rm -rf frcsim-eclipse-plugins/frcsim-eclipse-plugins/features
	mkdir -p frcsim-eclipse-plugins/frcsim-eclipse-plugins/plugins
	mkdir -p frcsim-eclipse-plugins/frcsim-eclipse-plugins/features
	cp $(allwpilib)/eclipse-plugins/edu.wpi.first.wpilib.plugins.updatesite/target/site/plugins/edu.wpi.first.wpilib.plugins.java_* frcsim-eclipse-plugins/frcsim-eclipse-plugins/plugins
	cp $(allwpilib)/eclipse-plugins/edu.wpi.first.wpilib.plugins.updatesite/target/site/plugins/edu.wpi.first.wpilib.plugins.cpp_* frcsim-eclipse-plugins/frcsim-eclipse-plugins/plugins
	cp $(allwpilib)/eclipse-plugins/edu.wpi.first.wpilib.plugins.updatesite/target/site/plugins/edu.wpi.first.wpilib.plugins.core_* frcsim-eclipse-plugins/frcsim-eclipse-plugins/plugins
	cp $(allwpilib)/eclipse-plugins/edu.wpi.first.wpilib.plugins.updatesite/target/site/features/edu.wpi.first.wpilib.plugins.java.feature_* frcsim-eclipse-plugins/frcsim-eclipse-plugins/features
	cp $(allwpilib)/eclipse-plugins/edu.wpi.first.wpilib.plugins.updatesite/target/site/features/edu.wpi.first.wpilib.plugins.cpp.feature_* frcsim-eclipse-plugins/frcsim-eclipse-plugins/features
	cp $(allwpilib)/eclipse-plugins/edu.wpi.first.wpilib.plugins.updatesite/target/site/features/edu.wpi.first.wpilib.plugins.core.feature_* frcsim-eclipse-plugins/frcsim-eclipse-plugins/features
	echo Increment version?
frcsim-eclipse-plugins/frcsim-eclipse-plugins_$(eclipse-plugins-version).orig.tar.gz: pull-eclipse-plugins
	cd frcsim-eclipse-plugins/frcsim-eclipse-plugins && debuild clean
	rm -f frcsim-eclipse-plugins/frcsim-eclipse-plugins_$(eclipse-plugins-version).orig.tar.gz
	cd frcsim-eclipse-plugins && tar --exclude="./debian" -czvf \
		frcsim-eclipse-plugins_$(eclipse-plugins-version).orig.tar.gz frcsim-eclipse-plugins

orig-frcsim-eclipse-toolchain-plugin: frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin_$(eclipse-toolchain-version).orig.tar.gz
pull-eclipse-toolchain:
	rm -rf frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin/plugins
	rm -rf frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin/features
	mkdir -p frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin/plugins
	mkdir -p frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin/features
	cp $(allwpilib)/eclipse-plugins/edu.wpi.first.wpilib.plugins.updatesite/target/site/plugins/edu.wpi.first.wpilib.plugins.cpp.toolchains.linux_* frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin/plugins/
	cp $(allwpilib)/eclipse-plugins/edu.wpi.first.wpilib.plugins.updatesite/target/site/features/edu.wpi.first.wpilib.plugins.cpp.toolchains.linux.feature_* frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin/features/
	echo Increment version?
frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin_$(eclipse-toolchain-version).orig.tar.gz: pull-eclipse-toolchain
	cd frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin && debuild clean
	rm -f frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin_$(eclipse-toolchain-version).orig.tar.gz
	cd frcsim-eclipse-toolchain-plugin && tar --exclude="./debian" -czvf \
		frcsim-eclipse-toolchain-plugin_$(eclipse-toolchain-version).orig.tar.gz frcsim-eclipse-toolchain-plugin

orig-frcsim-libwpilibsim-cpp: frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp_$(libwpilibsim-version).orig.tar.gz
pull-libwpilibsim-cpp:
	mkdir -p frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp/allwpilib
	cp -rf $(allwpilib)/hal frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp/allwpilib
	mkdir -p frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp/allwpilib/networktables
	cp -rf $(allwpilib)/networktables/cpp frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp/allwpilib/networktables
	mkdir -p frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp/allwpilib/wpilibc
	cp -r $(allwpilib)/wpilibc/wpilibC++ frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp/allwpilib/wpilibc/wpilibC++
	cp -r $(allwpilib)/wpilibc/wpilibC++Sim frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp/allwpilib/wpilibc/wpilibC++Sim
	echo 'cmake_minimum_required(VERSION 2.8)' > frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp/CMakeLists.txt
	echo 'project(WPILibSim)' >> frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp/CMakeLists.txt
	echo 'add_subdirectory(allwpilib/wpilibc/wpilibC++Sim)' >> frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp/CMakeLists.txt
	echo Increment version?
frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp_$(libwpilibsim-version).orig.tar.gz: pull-libwpilibsim-cpp
	cd frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp && debuild clean
	rm -f frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp_$(libwpilibsim-version).orig.tar.gz
	cd frcsim-libwpilibsim-cpp && tar --exclude="./debian" -czvf \
		frcsim-libwpilibsim-cpp_$(libwpilibsim-version).orig.tar.gz frcsim-libwpilibsim-cpp

orig-frcsim: frcsim/frcsim_$(frcsim-version).orig.tar.gz
frcsim/frcsim_$(frcsim-version).orig.tar.gz:
	cd frcsim/frcsim && debuild clean
	rm -f frcsim/frcsim_$(frcsim-version).orig.tar.gz
	cd frcsim && tar --exclude="./debian" -czvf \
		frcsim_$(frcsim-version).orig.tar.gz frcsim

# debs support (for faster parallel builds)
.PHONY: debs-frcsim-gazebo-plugins debs-frcsim-gazebo-models debs-frcsim-eclipse-plugins \
		debs-frcsim-eclipse-toolchain-plugin debs-frcsim-libwpilibsim-cpp debs-frcsim

debs-frcsim-gazebo-plugins: frcsim-gazebo-plugins/frcsim-gazebo-plugins_$(gazebo-plugins-package-version)_amd64.deb frcsim-gazebo-plugins/frcsim-gazebo-plugins_$(gazebo-plugins-package-version)_i386.deb
frcsim-gazebo-plugins/frcsim-gazebo-plugins_$(gazebo-plugins-package-version)_i386.deb: frcsim-gazebo-plugins/frcsim-gazebo-plugins_$(gazebo-plugins-version).orig.tar.gz frcsim-gazebo-plugins/frcsim-gazebo-plugins_$(gazebo-plugins-package-version)_amd64.deb
	cd frcsim-gazebo-plugins/frcsim-gazebo-plugins && make clean
	cd frcsim-gazebo-plugins && sbuild --arch=i386 -A -d trusty-i386 frcsim-gazebo-plugins_$(gazebo-plugins-package-version).dsc
frcsim-gazebo-plugins/frcsim-gazebo-plugins_$(gazebo-plugins-package-version)_amd64.deb: frcsim-gazebo-plugins/frcsim-gazebo-plugins_$(gazebo-plugins-version).orig.tar.gz
	cd frcsim-gazebo-plugins/frcsim-gazebo-plugins && debuild --no-lintian -us -uc -aamd64

debs-frcsim-gazebo-models: frcsim-gazebo-models/frcsim-gazebo-models_$(gazebo-models-package-version)_all.deb
frcsim-gazebo-models/frcsim-gazebo-models_$(gazebo-models-package-version)_all.deb: frcsim-gazebo-models/frcsim-gazebo-models_$(gazebo-models-version).orig.tar.gz
	cd frcsim-gazebo-models/frcsim-gazebo-models && debuild --no-lintian -us -uc

debs-frcsim-eclipse-plugins: frcsim-eclipse-plugins/frcsim-eclipse-plugins_$(eclipse-plugins-package-version)_all.deb
frcsim-eclipse-plugins/frcsim-eclipse-plugins_$(eclipse-plugins-package-version)_all.deb: frcsim-eclipse-plugins/frcsim-eclipse-plugins_$(eclipse-plugins-version).orig.tar.gz
	cd frcsim-eclipse-plugins/frcsim-eclipse-plugins && debuild --no-lintian -us -uc

debs-frcsim-eclipse-toolchain-plugin: frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin_$(eclipse-toolchain-package-version)_all.deb
frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin_$(eclipse-toolchain-package-version)_all.deb: frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin_$(eclipse-toolchain-version).orig.tar.gz
	cd frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin && debuild --no-lintian -us -uc

debs-frcsim-libwpilibsim-cpp: frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp_$(libwpilibsim-package-version)_amd64.deb frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp_$(libwpilibsim-package-version)_i386.deb
frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp_$(libwpilibsim-package-version)_i386.deb: frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp_$(libwpilibsim-version).orig.tar.gz frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp_$(libwpilibsim-package-version)_amd64.deb
	cd frcsim-libwpilibsim-cpp && sbuild --arch=i386 -A -d trusty-i386 frcsim-libwpilibsim-cpp_$(libwpilibsim-package-version).dsc
frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp_$(libwpilibsim-package-version)_amd64.deb: frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp_$(libwpilibsim-version).orig.tar.gz
	cd frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp && debuild --no-lintian -us -uc -aamd64

debs-frcsim: frcsim/frcsim_$(frcsim-package-version)_all.deb
frcsim/frcsim_$(frcsim-package-version)_all.deb: frcsim/frcsim_$(frcsim-version).orig.tar.gz
	cd frcsim/frcsim && debuild --no-lintian -us -uc

### SBUILD
#
# For building 32-bit builds on 64-bit machines
#
# $ mk-sbuild --arch i386 --personality=linux32 trusty
# To CHANGE the golden image: sudo schroot -c source:trusty-i386 -u root
# To ENTER an image snapshot: schroot -c trusty-i386
# To BUILD within a snapshot: sbuild -A -d trusty-i386 PACKAGE*.dsc
# 
# sbuild --arch=i386 -A -d trusty-i386 frcsim-gazebo-plugins_0.2-1.dsc
