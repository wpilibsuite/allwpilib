allwpilib=../..
version=DEVELOPMENT

all: java cpp site

java:
	mkdir -p build/java/
	mkdir -p site/java/
	cp -r $(allwpilib)/wpilibj/wpilibJava/src/main/java/* build/java/
	cp -r $(allwpilib)/wpilibj/wpilibJavaDevices/src/main/java/* build/java/
	# cp -r $(allwpilib)/networktables/java/src/main/java/* build/java/
	( cat java.doxy ; echo "PROJECT_NUMBER=${version}" ) | doxygen -

cpp:
	mkdir -p build/cpp/
	mkdir -p site/cpp/
	cp -r $(allwpilib)/wpilibc/wpilibC++/src build/cpp/
	cp -r $(allwpilib)/wpilibc/wpilibC++/include build/cpp/
	cp -r $(allwpilib)/wpilibc/wpilibC++Devices/src build/cpp/
	cp -r $(allwpilib)/wpilibc/wpilibC++Devices/include build/cpp/
	# cp -r $(allwpilib)/networktables/cpp/lib build/cpp/
	# cp -r $(allwpilib)/networktables/cpp/include build/cpp/
	( cat cpp.doxy ; echo "PROJECT_NUMBER=${version}" ) | doxygen -

# TODO: HAL

site:
	mkdir -p build/site/

clean:
	rm -rf build/
	rm -rf site/
