# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library(native-lib SHARED src/main/cpp/native-lib.cpp)
find_library(log-lib log)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.


# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

#set(pathToProject C:/Users/laris/AndroidStudioProjects/Facetracker)
#set(pathToProject C:/Users/laris/Documents/Mestrado/Disciplinas/2017.1/SistemasOperacionais/Projeto/facetracker/Facetracker)
#set(pathToOpenCv C:/opencv-3.2.0-android-sdk/OpenCV-android-sdk)
#set(pathToFaceTracker C:/Users/laris/Documents/Mestrado/Disciplinas/2017.1/SistemasOperacionais/Projeto/facetracker/Facetracker/app/src/main/cpp/FaceTracker)

set(pathToProject /run/media/gabriel/Arquivos/Git/FaceTrackerAndroid/Facetracker/)
set(pathToOpenCv /run/media/gabriel/Arquivos/Graduacao/2017.01/SO/OpenCV-android-sdk/)
set(pathToFaceTracker /run/media/gabriel/Arquivos/Git/FaceTrackerAndroid/Facetracker/app/src/main/cpp/FaceTracker)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

#Addition suggested by Bruno Alexandre Krinski 20160825
include_directories(${pathToOpenCv}/sdk/native/jni/include)
add_library( lib_opencv SHARED IMPORTED )
set_target_properties(lib_opencv PROPERTIES IMPORTED_LOCATION ${pathToProject}/app/src/main/jniLibs/${ANDROID_ABI}/libopencv_java3.so)
target_link_libraries(native-lib $\{log-lib} lib_opencv)

#includes FaceTracker
include_directories(/run/media/gabriel/Arquivos/Git/FaceTrackerAndroid/Facetracker/app/src/main/cpp/FaceTracker)

#include libraries
add_library(CLM SHARED src/main/cpp/CLM.cc)
add_library(FCheck SHARED src/main/cpp/FCheck.cc)
add_library(FDet SHARED src/main/cpp/FDet.cc)
add_library(IO SHARED src/main/cpp/IO.cc)
add_library(Patch SHARED src/main/cpp/Patch.cc)
add_library(PAW SHARED src/main/cpp/PAW.cc)
add_library(PDM SHARED src/main/cpp/PDM.cc)
add_library(Tracker SHARED src/main/cpp/Tracker.cc)
add_library(face_tracker SHARED src/main/cpp/face_tracker.cc)

target_link_libraries(CLM $\{log-lib} lib_opencv)
target_link_libraries(FCheck $\{log-lib} lib_opencv)
target_link_libraries(FDet $\{log-lib} lib_opencv)
target_link_libraries(IO $\{log-lib} lib_opencv)
target_link_libraries(Patch $\{log-lib} lib_opencv)
target_link_libraries(PAW $\{log-lib} lib_opencv)
target_link_libraries(PDM $\{log-lib} lib_opencv)
target_link_libraries(Tracker $\{log-lib} lib_opencv)
target_link_libraries(face_tracker $\{log-lib} lib_opencv)

target_link_libraries(face_tracker Tracker CLM FCheck FDet PAW Patch IO PDM)
target_link_libraries(PAW IO)
target_link_libraries(Patch IO)
target_link_libraries(FCheck PAW IO)
target_link_libraries(CLM PDM IO Patch)
target_link_libraries(PDM IO)
target_link_libraries(Tracker CLM FDet PDM FCheck IO)

target_link_libraries(native-lib android log)







