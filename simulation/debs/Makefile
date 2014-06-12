codename=trusty
allwpilib=../..

version=0.1
package-version=1
gazebo-plugins-version=$(version)
gazebo-plugins-package-version=$(gazebo-plugins-version)-$(package-version)
gazebo-models-version=$(version)
gazebo-models-package-version=$(gazebo-models-version)-$(package-version)
eclipse-plugins-version=$(version)
eclipse-plugins-package-version=$(eclipse-plugins-version)-$(package-version)
eclipse-toolchain-version=$(version)
eclipse-toolchain-package-version=$(eclipse-toolchain-version)-$(package-version)
libwpilibsim-version=$(version)
libwpilibsim-package-version=$(libwpilibsim-version)-$(package-version)
frcsim-version=$(version)
frcsim-package-version=$(frcsim-version)-$(package-version)

all: debs update-repository

allwpilib:
	cd $(allwpilib) && mvn -T 8 clean package -Dwith-eclipse-plugins -DskipTests -DskipIT

orig: clean
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

clean:
	cd frcsim-gazebo-plugins/frcsim-gazebo-plugins && debuild clean
	cd frcsim-gazebo-models/frcsim-gazebo-models && debuild clean
	cd frcsim-eclipse-plugins/frcsim-eclipse-plugins && debuild clean
	cd frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin && debuild clean
	cd frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp && debuild clean
	cd frcsim/frcsim && debuild clean

	rm -f frcsim-gazebo-plugins/frcsim-gazebo-plugins_$(gazebo-plugins-package-version).orig.tar.gz
	rm -f frcsim-gazebo-models/frcsim-gazebo-models_$(gazebo-models-package-version).orig.tar.gz
	rm -f frcsim-eclipse-plugins/frcsim-eclipse-plugins_$(eclipse-plugins-package-version).orig.tar.gz
	rm -f frcsim-eclipse-toolchain-plugin/frcsim-eclipse-toolchain-plugin_$(eclipse-toolchain-package-version).orig.tar.gz
	rm -f frcsim-libwpilib-cpp/frcsim-libwpilib-cpp_$(libwpilibsim-version).orig.tar.gz
	rm -f frcsim/frcsim_$(frcsim-package-version).orig.tar.gz

pull: pull-gazebo-plugins pull-eclipse-plugins pull-libwpilibsim-cpp orig

pull-gazebo-plugins:
	cp -rf -t frcsim-gazebo-plugins/frcsim-gazebo-plugins/ $(allwpilib)/simulation/frc_gazebo_plugin/*
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

pull-libwpilibsim-cpp:
	cp -rf -t frcsim-libwpilibsim-cpp/frcsim-libwpilibsim-cpp/ $(allwpilib)/wpilibc/wpilibC++Sim/*
	echo Increment version?

deploy:
	rsync -r -v -C -p repository/ adhenning@ccc.wpi.edu:public_html/frcsim/

