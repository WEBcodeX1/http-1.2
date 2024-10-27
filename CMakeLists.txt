# project settings
project(x0-data-gateway)
cmake_minimum_required(VERSION 3.0)
aux_source_directory(. SRC_LIST)

# find python3
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(PythonLibs REQUIRED)

# find boost::python
find_package(Boost 1.74 REQUIRED COMPONENTS system filesystem python)

# set g++ compiler flags
set(CMAKE_CXX_FLAGS "-I/usr/include/python3.10 -I/usr/include/python3.10 -g -fPIC -Wall -pthread -O2 -fstack-protector-strong -std=c++20 -Wno-unused-result -Wsign-compare -Wformat -Werror=format-security -DNDEBUG -fwrapv")
#set(CMAKE_CXX_FLAGS "-I/usr/include/python3.10 -I/usr/include/python3.10 -g -fPIC -Wall -pthread -fstack-protector-strong -std=c++20 -Wno-unused-result -Wsign-compare -Wformat -Werror=format-security -fwrapv")

# set boost specs
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)

# add executable
add_executable(${PROJECT_NAME} ${SRC_LIST})

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
    ./CPU.hpp
    ./Helper.hpp
    ./Global.hpp
    ./Server.hpp
    ./Client.hpp
    ./Filesystem.hpp
    ./Configuration.hpp
    ./MemoryManager.hpp
    ./ThreadHandler.hpp
    ./IPCHandler.hpp
    ./IPCHandlerAS.hpp
    ./ClientHandler.hpp
    ./ASProcessHandler.hpp
    ./ASRequestHandler.hpp
    ./ResultProcessor.hpp
    ./ResultOrder.hpp
    ./Constant.hpp
    ./Vector.hpp
)

# add subdirs
add_subdirectory(lib)
add_subdirectory(test)

# install
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
