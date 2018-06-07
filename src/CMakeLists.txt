cmake_minimum_required(VERSION 2.8)
project (lib)

set(GCC_COVERAGE_LINK_FLAGS  "-lvirt -g -Wall")
set(GCC_COVERAGE_COMPILE_FLAGS  "-I /usr/include/libvirt/")

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/exception")

set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
add_executable(domain main.cpp include/domain/domain.cpp include/domain/domain.h include/helper.h exception/DomainException.cpp exception/DomainException.h include/help.cpp router.h)

#target_link_libraries(domain crow)
target_link_libraries(domain ${Boost_LIBRARIES} ${LIBVIRT_LINK})
target_link_libraries(domain ${CMAKE_THREAD_LIBS_INIT} ${GCC_COVERAGE_LINK_FLAGS})

if (Tcmalloc_FOUND)
	target_link_libraries(domain ${Tcmalloc_LIBRARIES})
endif(Tcmalloc_FOUND)



