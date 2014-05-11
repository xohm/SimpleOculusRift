# -----------------------------------------------------------------------------
# SimpleOculusRift 
# -----------------------------------------------------------------------------
# Processing Wrapper for the Oculus Rift library
# http://github.com/xohm/SimpleOculusRift
# --------------------------------------------------------------------------
# prog:  Max Rheiner / Interaction Design / zhdk / http://iad.zhdk.ch/
# date:  06/24/2014 (m/d/y)
# ----------------------------------------------------------------------------
#
#	this envVar should be set:
#		OPEN_NI_INCLUDE < set this on osx: export OPEN_NI_INCLUDE=...path
#		OPEN_NI_LIB 
#		JAVA_HOME
#
# -----------------------------------------------------------------------------
	
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

INCLUDE(CheckIncludeFiles)

PROJECT(SimpleOculusRift)

IF(NOT MACH_ARCH)
    # default 64bit
    SET(MACH_ARCH "64")
ENDIF()

# -----------------------------------------------------------------------------
# check swig
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

# -----------------------------------------------------------------------------
# check java/jni
FIND_PACKAGE(Java REQUIRED)
FIND_PACKAGE(JNI REQUIRED)
INCLUDE_DIRECTORIES(${JAVA_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${JAVA_INCLUDE_PATH2})
 
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
 
# -----------------------------------------------------------------------------
# java tools
IF(WIN32)
	SET(JAVA_RUNTIME2  ${JAVA_HOME}/bin/java)
	SET(JAVA_ARCHIVE2  ${JAVA_HOME}/bin/jar)
	SET(JAVA_COMPILE2  ${JAVA_HOME}/bin/javac)
	SET(JAVA_DOC	   ${JAVA_HOME}/bin/javadoc)
ELSE()
	# unix
	SET(JAVA_RUNTIME2  java)
	SET(JAVA_ARCHIVE2  jar)
	SET(JAVA_COMPILE2  javac)
	SET(JAVA_DOC	   javadoc)
ENDIF()


# -----------------------------------------------------------------------------
# boost

FIND_PACKAGE(Boost REQUIRED)
IF(Boost_INCLUDE_DIRS MATCHES "Boost_INCLUDE_DIR-NOTFOUND")
	MESSAGE("Boost should be set, for example:")
	MESSAGE("> cmake -DBOOST_ROOT=/usr/??? ..")	
ELSE()
	INCLUDE_DIRECTORIES ( ${Boost_INCLUDE_DIRS} )
	LINK_DIRECTORIES ( ${Boost_LIBRARY_DIRS} )

	IF(APPLE)
		# osx
		set(Boost_LIBRARIES "boost_system-mt")
	ELSEIF(UNIX)
		# unix/linux,shared libs of boost should be installed
		#set(Boost_LIBRARIES  "boost_thread.a")
		set(Boost_LIBRARIES  "boost_system")
	ELSE()
		# windows will find the right libs by itself
		LINK_DIRECTORIES ( ${BOOST_LIBRARYDIR} )	
	ENDIF()
ENDIF()

# -----------------------------------------------------------------------------
# oculus rift
INCLUDE_DIRECTORIES(${OCULUSRIFT_DIR}/LibOVR/Include)

IF(APPLE)
	# osx
	LINK_DIRECTORIES(${OCULUSRIFT_DIR}/LibOVR/Lib/MacOS/Release)
	SET(LIBS ${LIBS} ovr)
ELSEIF(UNIX)
	# unix/linux,shared libs of boost should be installed
	LINK_DIRECTORIES(${OCULUSRIFT_DIR}/LibOVR/Lib/Linux/Release/x86_64)
	SET(LIBS ${LIBS} ovr)
ELSE()
	# windows will find the right libs by itself
	LINK_DIRECTORIES (${OCULUSRIFT_DIR}/LibOVR/Lib/x64)	
	SET(LIBS ${LIBS} libovr64)
ENDIF()


# Oculus SDK dependencies for Mac OSX:
IF (APPLE)
    FIND_LIBRARY (CORE_FOUNDATION CoreFoundation)
    FIND_LIBRARY (CORE_GRAPHICS CoreGraphics)
    FIND_LIBRARY (IOKIT IOKit)
ENDIF()

# -----------------------------------------------------------------------------
# glew

IF(WIN32)
	INCLUDE_DIRECTORIES(${GLEW_INC})
	LINK_DIRECTORIES(${GLEW_LIB})	
	SET(LIBS ${LIBS} glew32)
ELSE()
	SET(LIBS ${LIBS} GLEW)
ENDIF()

FIND_PACKAGE(OpenGL REQUIRED)

# -----------------------------------------------------------------------------
# swig
SET(CMAKE_SWIG_FLAGS -package SimpleOculusRift)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)


###############################################################################
# set local libraries


LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib)

SET(CMAKE_SKIP_RPATH NO)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)


IF(APPLE)
    # osx
    SET(CMAKE_INSTALL_RPATH "/$ORIGIN/./osx")
ELSEIF(UNIX)
    IF(${MACH_ARCH} MATCHES "64")
        SET(CMAKE_INSTALL_RPATH "$ORIGIN/linux64")
    ELSE()
        SET(CMAKE_INSTALL_RPATH "/$ORIGIN/./linux32")
    ENDIF()
ELSEIF(WIN32)
    # no rpath on windows
    #SET(CMAKE_INSTALL_RPATH "/$ORIGIN")
ENDIF()

# -----------------------------------------------------------------------------
# processing wrappers
SET(P5_WRAPPER ${CMAKE_CURRENT_SOURCE_DIR}/src/p5_src/)
SET(P5_WRAPPER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/p5_src/SimpleOculusRift.java)

# processing core lib
# shoud use find_program
IF(WIN32)
	IF(NOT DEFINED P5_JAR)	
		SET(P5_JAR "C:/Program Files (x86)/processing-1.2.1/lib/core.jar")	
	ENDIF()
ELSEIF(APPLE)
	IF(NOT DEFINED P5_JAR)	
		SET(P5_JAR "/Applications/Processing.app/Contents/Resources/Java/core.jar")
	ENDIF()
ELSE()
	# unix/linux
	# SET(P5_JAR "/?")
	IF(NOT DEFINED P5_JAR)
		MESSAGE("Set P5_JAR to the folder where Processing installed the core.jar file, for example:")
		MESSAGE(FATAL_ERROR "> cmake -DP5_JAR=/usr/???/core.jar ..")	
	ENDIF()
ENDIF()

# -----------------------------------------------------------------------------
# eigen 3d template lib
IF(DEFINED EIGEN3D_INCLUDE)
	INCLUDE_DIRECTORIES(${EIGEN3D_INCLUDE})
ELSE()
	# check if the envvar is set
	SET(EIGEN3D_INCLUDE $ENV{EIGEN3D_INCLUDE})
	IF(DEFINED EIGEN3D_INCLUDE)
		INCLUDE_DIRECTORIES(${EIGEN3D_INCLUDE})
	ELSE()
		MESSAGE("EIGEN3D_INCLUDE should be set, for example:")
		MESSAGE("> cmake -DEIGEN3D_INCLUDE=/usr/??? ..")	
	ENDIF()
ENDIF()	


SET(SWIG_SOURCES 
src/SimpleOculusRiftMain.i
src/ContextWrapper.cpp
src/ocululsRiftShader.cpp
)

SET_SOURCE_FILES_PROPERTIES(${SWIG_SOURCES} PROPERTIES CPLUSPLUS ON)

# set the folder where the swig files should land
SET(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/swig_src)
MAKE_DIRECTORY(${CMAKE_CURRENT_BINARY_DIR}/src)

IF(APPLE)
  SET(LIBS ${LIBS} ${CORE_FOUNDATION} ${CORE_GRAPHICS} ${IOKIT} ${OPENGL_LIBRARIES} "-framework ApplicationServices")
ELSEIF(UNIX)  
  SET(LIBS ${LIBS} udev pthread GL X11 Xinerama)
ELSEIF(WIN32)
  SET(LIBS ${LIBS} ${OPENGL_LIBRARIES} winmm)
ENDIF()
    
SWIG_ADD_MODULE(SimpleOculusRift Java ${SWIG_SOURCES})
SWIG_LINK_LIBRARIES(SimpleOculusRift ${JAVA_LIBRARIES} ${Boost_LIBRARIES} ${LIBS})
SET(CMAKE_SHARED_LINKER_FLAGS "-fPIC")

# set the arch extension for the lib
IF(NOT APPLE)
  IF(${MACH_ARCH} MATCHES "64")
	#64bit
	message("set 64bit extension") 
	SET(NEW_SUFFIX "64")
  ELSE()
	# 32bit
	message("set 32bit extension") 
	SET(NEW_SUFFIX "32")
  ENDIF()

  IF(WIN32)
	SET(NEW_SUFFIX ${NEW_SUFFIX}.dll)	
  ELSE()
	# unix linux
	SET(NEW_SUFFIX ${NEW_SUFFIX}.so)	
  ENDIF()

  SET_TARGET_PROPERTIES(SimpleOculusRift PROPERTIES SUFFIX ${NEW_SUFFIX})
  
ENDIF()

# only for linux
IF(UNIX)
  IF(${MACH_ARCH} MATCHES "32")
	SET_TARGET_PROPERTIES(SimpleOculusRift PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
  ENDIF()
ENDIF()

ADD_CUSTOM_COMMAND(TARGET SimpleOculusRift
                    POST_BUILD
                    COMMAND cmake -E echo "-----------------------------"
                    COMMAND cmake -E echo "Compiling Java files..."
                    COMMAND cmake -E make_directory ./SimpleOculusRift
                    COMMAND cmake -E make_directory ${CMAKE_SWIG_OUTDIR}
                    COMMAND cp "${P5_WRAPPER}*.java" "${CMAKE_SWIG_OUTDIR}"
                    COMMAND ${JAVA_COMPILE2} -classpath ${P5_JAR} ${CMAKE_SWIG_OUTDIR}/*.java -d ./
                    COMMAND cmake -E echo "-----------------------------"                    
                    COMMAND cmake -E echo "Creating jar file..."
                    COMMAND ${JAVA_ARCHIVE2} cvf SimpleOculusRift.jar SimpleOculusRift
                    COMMAND cmake -E echo "-----------------------------"
                    COMMAND cmake -E echo "Creating doc files..."
                    COMMAND cmake -E make_directory ./doc
                    COMMAND ${JAVA_DOC} -classpath ${P5_JAR} -quiet -author -public -nodeprecated -nohelp -d ./doc  -version ${CMAKE_SWIG_OUTDIR}/*.java)
