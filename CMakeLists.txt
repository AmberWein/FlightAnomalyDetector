
cmake_minimum_required(VERSION 3.17)
project(ex6)


set(CMAKE_CXX_STANDARD 14)
find_package(Threads)

add_executable(ex6 MainTrain.cpp
        anomaly_detection_util.cpp anomaly_detection_util.h
        AnomalyDetector.h
        SimpleAnomalyDetector.cpp SimpleAnomalyDetector.h
        timeseries.cpp timeseries.h
        HybridAnomalyDetector.cpp
        minCircle.cpp minCircle.h
        commands.h
        CLI.h CLI.cpp
        Server.cpp Server.h)
set(GCC_COVERAGE_COMPILE_FLAGS "-pthread -std=c++11")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

target_link_libraries(ex6 ${CMAKE_THREAD_LIBS_INIT})