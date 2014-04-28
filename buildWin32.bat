rem "build SimpleOpenNI 32bit"
mkdir build32
cd build32
cmake -G "Visual Studio 9 2008" ^
-DMACH_ARCH="32" ^
-DCMAKE_BUILD_TYPE=Release ^
-DOPEN_NI_BASE="C:\Program Files (x86)\OpenNI2" ^
-DOPEN_NI_LIBDIR="C:\Program Files (x86)\OpenNI2\Lib" ^
-DNITE_BASE="C:\Program Files (x86)\PrimeSense\NiTE2" ^
-DEIGEN3D_INCLUDE=C:\Users\Public\Documents\development\libs\graphics\3d\eigen\eigen-eigen-c40708b9088d ^
-DBOOST_ROOT="C:\local\boost_1_54_0" ^
-DBOOST_LIBRARYDIR="C:\local\boost_1_54_0\lib32-msvc-9.0" ^
-DP5_JAR="C:\Users\max\Documents\pDev\localApp\processing-2.0.2_32\core\library" ^
..
cd ..