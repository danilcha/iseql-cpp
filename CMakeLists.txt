cmake_minimum_required(VERSION 2.4)

project(iseql-cpp CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_definitions(-DCOUNTERS)

#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_LIBCPP_DEBUG=0 -D_LIBCPP_DEBUG2=0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic -Wall -Wextra -Wconversion -Wdisabled-optimization")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-date-time")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shadow")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat-pedantic")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-old-style-cast")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-float-equal")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-padded")
endif()



add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(result-processor)



SET(MY_REMOTE_LOCATION im:iseql-cpp)
SET(IL_REMOTE_LOCATION im:iseql-il)

add_custom_target(upload-to-ironmaiden
	COMMAND rsync -aPvz
		--exclude='.*' --exclude=results --exclude=target
		${CMAKE_CURRENT_SOURCE_DIR}/
		${MY_REMOTE_LOCATION})

add_custom_target(upload-to-ironlady
	COMMAND rsync -aPvz
		--exclude='.*' --exclude=results --exclude=target
		${CMAKE_CURRENT_SOURCE_DIR}/
		${IL_REMOTE_LOCATION})


add_custom_target(downloads-results-from-ironmaiden
	COMMAND rsync -aPvzL
		--exclude='.*'
		${MY_REMOTE_LOCATION}/results/
		${CMAKE_CURRENT_SOURCE_DIR}/results)
