:: This file is a helper for allC++Sim
::
:: Usage: cd /build && ../configure
::
:: WARNING -- this is only temporary, and only meant for debug, and only works on my computer
@set WS=C:\Users\peter\gz-ws
@set BOOST_PATH=%WS%\boost_1_56_0
@set BOOST_LIBRARY_DIR=%BOOST_PATH%\lib64-msvc-12.0

@set PROTOBUF_PATH=%WS%\protobuf-2.6.0-win64-vc12

@set OGRE_PATH=%WS%\ogre_src_v1-8-1-vc12-x64-release-debug\build\install\Debug
@set OGRE_INCLUDE_DIR=%OGRE_PATH%\include;%OGRE_PATH%\include\OGRE;%OGRE_PATH%\include\OGRE\RTShaderSystem;%OGRE_PATH%\include\OGRE\Terrain;%OGRE_PATH%\include\OGRE\Paging
@set OGRE_LIBRARY_DIR=%OGRE_PATH%\lib\Debug
set OGRE_LIB_SUFFIX=_d.lib
@set OGRE_LIBS=%OGRE_LIBRARY_DIR%\OgreMain%OGRE_LIB_SUFFIX%;%OGRE_LIBRARY_DIR%\OgreOverlay%OGRE_LIB_SUFFIX%;%OGRE_LIBRARY_DIR%\OgreRTShaderSystem%OGRE_LIB_SUFFIX%;%OGRE_LIBRARY_DIR%\OgreTerrain%OGRE_LIB_SUFFIX%;%OGRE_LIBRARY_DIR%\OgrePaging%OGRE_LIB_SUFFIX%
@set OGRE_LIBS=%OGRE_LIBRARY_DIR%\OgreMain%OGRE_LIB_SUFFIX%;%OGRE_LIBRARY_DIR%\OgreRTShaderSystem%OGRE_LIB_SUFFIX%;%OGRE_LIBRARY_DIR%\OgreTerrain%OGRE_LIB_SUFFIX%;%OGRE_LIBRARY_DIR%\OgrePaging%OGRE_LIB_SUFFIX%

@set FREEIMAGE_PATH=%WS%\FreeImage-vc12-x64-release-debug
@set FREEIMAGE_INCLUDE_DIR=%FREEIMAGE_PATH%\Source

@set TBB_PATH=%WS%\tbb43_20141023oss
@set TBB_INCLUDEDIR=%TBB_PATH%\include

@set DLFCN_WIN32_PATH=%WS%\dlfcn-win32-vc12-x64-release-debug\build\install\Debug
@set DLFCN_WIN32_INCLUDE_DIR=%DLFCN_WIN32_PATH%\include

@set TINY_XML_INCLUDE_DIR=%WS%\sdformat\src\win\tinyxml

@set GAZEBO_PATH=%WS%\gazebo\build\install\Debug\lib\cmake\gazebo
@set SDFORMAT_PATH=%WS%\sdformat\build\install\Debug\lib\cmake\sdformat
@set IGNITION-MATH_PATH=%WS%\ign-math\build\install\Debug\lib\cmake\ignition-math2

@set INCLUDE=%TINY_XML_INCLUDE_DIR%;%FREEIMAGE_INCLUDE_DIR%;%TBB_INCLUDEDIR%;%DLFCN_WIN32_INCLUDE_DIR%;%INCLUDE%
@set LIB=%LIB%

cmake -G "NMake Makefiles"^
	-DCMAKE_INSTALL_PREFIX=build^
    -DCMAKE_PREFIX_PATH="%GAZEBO_PATH%;%SDFORMAT_PATH%;%IGNITION-MATH_PATH%"^
    -DOGRE_FOUND=1^
    -DOGRE_INCLUDE_DIRS="%OGRE_INCLUDE_DIR%"^
    -DOGRE_LIBRARIES="%OGRE_LIBS%"^
    -DPROTOBUF_SRC_ROOT_FOLDER="%PROTOBUF_PATH%"^
    -DBOOST_ROOT="%BOOST_PATH%"^
    -DBOOST_LIBRARYDIR="%BOOST_LIBRARY_DIR%"^
    ..
