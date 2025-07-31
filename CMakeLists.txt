# project settings
cmake_minimum_required(VERSION 3.11)
project(falcon-as)

# do not print out warnings about unused variables
set(ignoreMe "${JAVA_BACKEND} ${DEBUG_BUILD}")

# find python3
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(PythonLibs REQUIRED)
include_directories( ${PYTHON_INCLUDE_DIRS} )

# find boost::python
find_package(Boost 1.74 REQUIRED COMPONENTS system filesystem python)

# check for -D JAVA_BACKEND=1
if ( DEFINED JAVA_BACKEND )

    # find java / jni packages
    find_package(Java REQUIRED)
    find_package(JNI REQUIRED)

    message("Java_JAVA_EXECUTABLE = ${Java_JAVA_EXECUTABLE}")
    message("Java_JAVAC_EXECUTABLE = ${Java_JAVAC_EXECUTABLE}")
    message("Java_JAVAH_EXECUTABLE = ${Java_JAVAH_EXECUTABLE}")
    message("Java_JAVADOC_EXECUTABLE = ${Java_JAVADOC_EXECUTABLE}")
    message("Java_VERSION_STRING = ${Java_VERSION_STRING}")
    message("Java_VERSION = ${Java_VERSION}")

    message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
    message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")

endif()

# set g++ compiler flags
if ( DEFINED DEBUG_BUILD )
    add_compile_definitions(DEBUG_BUILD)
    set(CMAKE_CXX_FLAGS "-g -fPIC -Wall -pthread -std=c++20 -Wno-unused-result -Wsign-compare -Wformat -Werror=format-security -fwrapv -rdynamic")
else()
    set(CMAKE_CXX_FLAGS "-g -fPIC -Wall -pthread -O3 -fstack-protector-strong -std=c++20 -Wno-unused-result -Wsign-compare -Wformat -Werror=format-security -DNDEBUG -fwrapv")
endif()

# set boost specs
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)

# add executable
aux_source_directory(./src SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

# java or python backend
if ( DEFINED JAVA_BACKEND )
    add_compile_definitions(JAVA_BACKEND)
    include_directories(${JNI_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${JNI_LIBRARIES})
    add_custom_target(
        JavaBackendHeaderFiles
        SOURCES
        ./src/ASBackend/JavaBackend.hpp
    )
else()
    add_custom_target(
        PythonBackendHeaderFiles
        SOURCES
        ./src/ASBackend/PythonBackend.hpp
    )
endif()

# set include
target_include_directories(${PROJECT_NAME} PRIVATE ${Python3_INCLUDE_DIRS})

# set target link libraries
target_link_libraries(
    ${PROJECT_NAME}
    ${PYTHON_LIBRARIES}
    ${Boost_LIBRARIES}
    libhttp
)

# add custom targets
add_custom_target(
    ProjectHeaderFiles
    SOURCES
    ./src/Helper.hpp
    ./src//Global.hpp
    ./src//Server.hpp
    ./src//Client.hpp
    ./src//Filesystem.hpp
    ./src//Configuration.hpp
    ./src//MemoryManager.hpp
    ./src//ThreadHandler.hpp
    ./src//IPCHandler.hpp
    ./src//IPCHandlerAS.hpp
    ./src//ClientHandler.hpp
    ./src//ASProcessHandler.hpp
    ./src//ASRequestHandler.hpp
    ./src//ResultProcessor.hpp
    ./src//ResultOrder.hpp
    ./src//Constant.hpp
    ./src//Vector.hpp
)

# add subdirs
add_subdirectory(lib)
add_subdirectory(test)

# install
install(CODE "execute_process(COMMAND ./scripts/add_user_group.sh)")
install(CODE "execute_process(COMMAND ./scripts/mk_www.sh)")
install(CODE "execute_process(COMMAND ./scripts/cp_etc.sh)")
install(CODE "execute_process(COMMAND ./scripts/set-transparent-hugepages.sh)")
install(CODE "execute_process(COMMAND ./scripts/patch_etc_hosts.sh)")
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
