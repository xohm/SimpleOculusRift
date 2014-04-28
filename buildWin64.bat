rem "build SimpleOpenNI 64bit"
mkdir build64
cd build64
cmake -G "Visual Studio 9 2008 Win64" ^
-DMACH_ARCH="64" ^
-DCMAKE_BUILD_TYPE=Release ^
-DOPEN_NI_BASE="C:\Program Files\OpenNI2" ^
-DOPEN_NI_LIBDIR="C:\Program Files\OpenNI2\Lib" ^
-DNITE_BASE="C:\Program Files\PrimeSense\NiTE2" ^
-DEIGEN3D_INCLUDE="C:\Users\Public\Documents\development\libs\graphics\3d\eigen\eigen-eigen-c40708b9088d" ^
-DBOOST_ROOT="C:\local\boost_1_54_0" ^
-DBOOST_LIBRARYDIR="C:\local\boost_1_54_0\lib64-msvc-9.0" ^
-DP5_JAR="C:\Users\max\Documents\pDev\localApp\processing-2.0.2_64\core\library" ^
..
cd ..