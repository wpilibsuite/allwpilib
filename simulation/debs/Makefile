codename=trusty
allwpilib=../..

package-version = 1
gazebo-plugins-version = 0.2
gazebo-plugins-package-version = $(gazebo-plugins-version)-$(package-version)
gazebo-models-version = 0.2
gazebo-models-package-version = $(gazebo-models-version)-$(package-version)
gazebo-models-orig-url = https://usfirst.collab.net/sf/frs/do/downloadFile/projects.wpilib/frs.simulation.frcsim_gazebo_models/frs1052?dl=1
eclipse-plugins-version = 0.1
eclipse-plugins-package-version = $(eclipse-plugins-version)-$(package-version)
eclipse-toolchain-version = 0.1
eclipse-toolchain-package-version = $(eclipse-toolchain-version)-$(package-version)
libwpilibsim-version = 0.1
libwpilibsim-package-version = $(libwpilibsim-version)-$(package-version)
frcsim-version = 0.1
frcsim-package-version = $(frcsim-version)-$(package-version)

all: debs update-repository

jenkins: download-models pull-eclipse-toolchain pull all

allwpilib:
	cd $(allwpilib) && mvn -T 8 clean package -Dwith-eclipse-plugins -DskipTests -DskipIT

orig: pre-orig-clean
	cd frcsim-gazebo-plugins && tar --exclude="./debian" -czvf \
		frcsim-gazebo-plugins_${gazebo-plugins-version}.orig.tar.gz frcsim-gazebo-plugins
	cd frcsim-gazebo-models && tar --exclude="./debian" -czvf \
		frcsim-gazebo-models_${gazebo-models-version}.orig.tar.gz frcsim-gazebo-models
	cd frcsim-eclipse-plugins && tar --exclude="./debian" -czvf \
		frcsim-eclipse-plugins_${eclipse-plugins-version}.orig.tar.gz frcsim-eclipse-plugins
	cd frcsim-eclipse-toolchain-plugin && tar --exclude="./debian" -czvf \
		frcsim-eclipse-toolchain-plugin_${eclipse-toolchain-version}.orig.tar.gz frcsim-eclipse-toolchain-plugin
	cd frcsim-libwpilibsim-cpp && tar --exclude="./debian" -czvf \
		frcsim-libwpilibsim-cpp_${libwpilibsim-version}.orig.tar.gz frcsim-libwpilibsim-cpp
	cd frcsim && tar --exclude="./debian" -czvf \
		frcsim_${frcsim-version}.orig.tar.gz frcsim

debs:
	cd frcsim-gazebo-plugins/frcsim-gazebo-plugins && debuild -us -uc -iamd64
	cd frcsim-gazebo-models/frcsim-gazebo-models && debuild -us -uc
	cd frcsim-eclipse-plugins/frcsim-eclipse-plugins && debuild -us -uc
	cd frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin && debuild -us -uc
	cd frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp && debuild -us -uc
	cd frcsim/frcsim && debuild -us -uc

update-repository: clean-repository
	cd repository && reprepro includedeb $(codename) ../frcsim-gazebo-plugins/frcsim-gazebo-plugins_$(gazebo-plugins-package-version)_amd64.deb
	cd repository && reprepro includedeb $(codename) ../frcsim-gazebo-models/frcsim-gazebo-models_$(gazebo-models-package-version)_all.deb
	cd repository && reprepro includedeb $(codename) ../frcsim-eclipse-plugins/frcsim-eclipse-plugins_$(eclipse-plugins-package-version)_all.deb
	cd repository && reprepro includedeb $(codename) ../frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin_$(eclipse-toolchain-package-version)_all.deb
	cd repository && reprepro includedeb $(codename) ../frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp_$(libwpilibsim-package-version)_amd64.deb
	cd repository && reprepro includedeb $(codename) ../frcsim/frcsim_$(frcsim-package-version)_all.deb

clean-repository:
	cd repository && reprepro remove $(codename) frcsim-gazebo-plugins
	cd repository && reprepro remove $(codename) frcsim-gazebo-models
	cd repository && reprepro remove $(codename) frcsim-eclipse-plugins
	cd repository && reprepro remove $(codename) frcsim-eclipse-toolchain-plugin
	cd repository && reprepro remove $(codename) frcsim-libwpilibsim-cpp
	cd repository && reprepro remove $(codename) frcsim

pre-orig-clean:
	cd frcsim-gazebo-plugins/frcsim-gazebo-plugins && debuild clean
	cd frcsim-gazebo-models/frcsim-gazebo-models && debuild clean
	cd frcsim-eclipse-plugins/frcsim-eclipse-plugins && debuild clean
	cd frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin && debuild clean
	cd frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp && debuild clean
	cd frcsim/frcsim && debuild clean

	rm -f frcsim-gazebo-plugins/frcsim-gazebo-plugins_$(gazebo-plugins-version).orig.tar.gz
	rm -f frcsim-gazebo-models/frcsim-gazebo-models_$(gazebo-models-version).orig.tar.gz
	rm -f frcsim-eclipse-plugins/frcsim-eclipse-plugins_$(eclipse-plugins-version).orig.tar.gz
	rm -f frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin_$(eclipse-toolchain-version).orig.tar.gz
	rm -f frcsim-libwpilib-cpp/frcsim-libwpilib-cpp_$(libwpilibsim-version).orig.tar.gz
	rm -f frcsim/frcsim_$(frcsim-version).orig.tar.gz

clean: pre-orig-clean
	rm -rf frcsim-eclipse-plugins/frcsim-eclipse-plugins/plugins
	rm -rf frcsim-eclipse-plugins/frcsim-eclipse-plugins/features

	rm -rf frcsim-gazebo-plugins/frcsim-gazebo-plugins/src

	rm -rf frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp/src
	rm -rf frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp/include
	rm -f frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp/Makefile
	rm -f frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp/CMakeLists.txt
	rm -rf frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp/target

pull: clean pull-gazebo-plugins pull-eclipse-plugins pull-libwpilibsim-cpp orig

pull-gazebo-plugins:
	cp -rf -t frcsim-gazebo-plugins/frcsim-gazebo-plugins/ $(allwpilib)/simulation/frc_gazebo_plugins/*
	echo Increment version?

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

pull-eclipse-toolchain:
	rm -rf frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin/plugins
	rm -rf frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin/features
	mkdir -p frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin/plugins
	mkdir -p frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin/features
	cp $(allwpilib)/eclipse-plugins/edu.wpi.first.wpilib.plugins.updatesite/target/site/plugins/edu.wpi.first.wpilib.plugins.cpp.toolchains.linux_* frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin/plugins/
	cp $(allwpilib)/eclipse-plugins/edu.wpi.first.wpilib.plugins.updatesite/target/site/features/edu.wpi.first.wpilib.plugins.cpp.toolchains.linux.feature_* frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin/features/
	echo Increment version?

pull-libwpilibsim-cpp:
	cp -rf -t frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp/ $(allwpilib)/wpilibc/wpilibC++Sim/*
	echo Increment version?

download-models:
	cd frcsim-gazebo-models && \
	    wget --no-check-certificate $(gazebo-models-orig-url) -O frcsim-gazebo-models_${gazebo-models-version}.orig.tar.gz && \
	    tar xvf frcsim-gazebo-models_${gazebo-models-version}.orig.tar.gz

install:
	sudo dpkg -i frcsim-gazebo-plugins/frcsim-gazebo-plugins_0.2-1_amd64.deb \
                 frcsim-gazebo-models/frcsim-gazebo-models_0.2-1_all.deb \
                 frcsim-eclipse-plugins/frcsim-eclipse-plugins_0.1-1_all.deb \
                 frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin_0.1-1_all.deb \
                 frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp_0.1-1_amd64.deb \
                 frcsim/frcsim_0.1-1_all.deb

deploy:
	rsync -r -v -C -p repository/ adhenning@ccc.wpi.edu:public_html/frcsim/

