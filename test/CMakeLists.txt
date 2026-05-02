# find boost unittest package
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

# add subdirs
add_subdirectory(eval)
add_subdirectory(integration)
add_subdirectory(unit)
add_subdirectory(performance)
