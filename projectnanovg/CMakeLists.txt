cmake_minimum_required(VERSION 3.6.0)
project(Main)
include_directories(
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/src
)
file(GLOB SRCS
	"${PROJECT_SOURCE_DIR}/include/*"
	"${PROJECT_SOURCE_DIR}/src/*"
)
#add_definitions(-DGLES2)
add_executable(Main ${SRCS})
find_package(SDL2 REQUIRED)
target_link_libraries(Main ${SDL2_LIBRARIES})
target_link_libraries(Main GLESv2)
target_link_libraries(Main m)