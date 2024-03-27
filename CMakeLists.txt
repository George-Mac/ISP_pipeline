cmake_minimum_required(VERSION 3.25) 

project(myISP)  
  
set(CMAKE_CXX_STANDARD 17)  

set(SOURCE_TARGET
    src/main.cpp
    src/BayerDomain.cpp
    src/common.cpp
    src/Config.cpp
    src/RGBDomain.cpp
    src/YUVDomain.cpp
    src/output.cpp
    src/util.cpp
    )

set(HEADERS_TARGET
   src/BayerDomain.h
   src/common.h
   src/Config.h
   src/RGBDomain.h
   src/YUVDomain.h
   src/util.h
   src/output.h
    )


find_package(OpenCV REQUIRED)
  
include_directories(
    ${OpenCV_INCLUDE_DIRS}
    lib/Libraw/
    )  

link_directories(lib/Libraw/lib/.libs)

add_executable(myISP ${SOURCE_TARGET} ${HEADERS_TARGET})  
target_link_libraries(myISP ${OpenCV_LIBS})
target_link_libraries(myISP libraw.a)
