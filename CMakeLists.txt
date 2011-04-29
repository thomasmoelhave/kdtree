project(kdtree)

find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(main main.cpp primitives.h kdtree.h cditraits.h csv.cpp csv.h)
target_link_libraries(main ${Boost_LIBRARIES})
