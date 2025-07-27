# project settings
aux_source_directory(. SRC_LIST_JAVA_JNI)

# find java / jni packages
find_package(Java REQUIRED)
find_package(JNI REQUIRED)

message("JAVA_HOME = ${JAVA_HOME}")
message("Java_JAVA_EXECUTABLE = ${Java_JAVA_EXECUTABLE}")
message("Java_JAVAC_EXECUTABLE = ${Java_JAVAC_EXECUTABLE}")
message("Java_JAVAH_EXECUTABLE = ${Java_JAVAH_EXECUTABLE}")
message("Java_JAVADOC_EXECUTABLE = ${Java_JAVADOC_EXECUTABLE}")
message("Java_VERSION_STRING = ${Java_VERSION_STRING}")
message("Java_VERSION = ${Java_VERSION}")

message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")

# set include paths
include_directories(${JNI_INCLUDE_DIRS})

# set binary executable
add_executable(test-run-jni test-run-jni.cpp)

# boost / java jni libs
target_link_libraries(test-run-jni ${JNI_LIBRARIES})
target_link_libraries(test-run-jni ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

# add custom targets
add_custom_target(
    test-run-jni-header
    SOURCES
    ./test-run-jni.hpp
)
