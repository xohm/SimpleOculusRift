rem "build SimpleOculusRift 64bit"
mkdir build64
cd build64
cmake -G "Visual Studio 11 Win64" ^
-DMACH_ARCH="64" ^
-DCMAKE_BUILD_TYPE=Release ^
-DBOOST_ROOT="C:\local\boost_1_55_0" ^
-DBOOST_LIBRARYDIR="C:\local\boost_1_55_0\lib64-msvc-12.0" ^
-DGLEW_INC="C:\Users\max\Documents\development\glew-1.10.0\include" ^
-DGLEW_LIB="C:\Users\max\Documents\development\glew-1.10.0\lib\Release\x64" ^
-DOCULUSRIFT_DIR="C:\Users\max\Documents\development\OculusSDK" ^
-DJAVA_HOME="C:\Program Files\Java\jdk1.8.0_05" ^
-DP5_JAR="C:\Users\max\Desktop\processing-2.1.2\core\library\core.jar" ^
..
cd ..