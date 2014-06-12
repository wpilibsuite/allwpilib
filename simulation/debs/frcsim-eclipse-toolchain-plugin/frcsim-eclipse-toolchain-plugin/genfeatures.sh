mkdir -p build/zips/features
mkdir -p build/features
for i in features/*.jar; do
    echo $i
	cp $i build/zips/$i.zip;
    mkdir build/$i
    unzip build/zips/$i.zip -d build/$i
done
