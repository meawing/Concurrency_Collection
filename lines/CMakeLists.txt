get_filename_component(LIB_INCLUDE_PATH ".." ABSOLUTE)
get_filename_component(LIB_SYSTEM_INCLUDE_PATH "../contrib" ABSOLUTE)

get_filename_component(LIB_PATH "." ABSOLUTE)

file(GLOB_RECURSE LIB_CXX_SOURCES ${LIB_PATH}/*.cpp)
file(GLOB_RECURSE LIB_HEADERS ${LIB_PATH}/*.hpp)
file(GLOB_RECURSE LIB_ASM_SOURCES ${LIB_PATH}/*.S)


enable_language(ASM)

add_library(lines STATIC ${LIB_CXX_SOURCES} ${LIB_ASM_SOURCES} ${LIB_HEADERS})
target_include_directories(lines PUBLIC ${LIB_INCLUDE_PATH})
target_include_directories(lines SYSTEM PUBLIC ${LIB_SYSTEM_INCLUDE_PATH})
target_link_libraries(lines PUBLIC libassert::assert)
