cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)
project(FREDServer)

include_directories(include/FREDServer include)
link_directories(lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

set(ALFRED_SOURCES
		"src/Alfred/alfred.cpp"
		"src/Alfred/client.cpp"
		"src/Alfred/command.cpp"
		"src/Alfred/frontend.cpp"
		"src/Alfred/info.cpp"
		"src/Alfred/print.cpp"
		"src/Alfred/rpc.cpp"
		"src/Alfred/rpcinfo.cpp"
		"src/Alfred/service.cpp"
		"src/Alfred/wait.cpp"
	)

add_library(Alfred SHARED ${ALFRED_SOURCES})
target_link_libraries(Alfred libdim.a libpthread.so)

file(GLOB SOURCES "src/main.cpp"
		"src/Parser/utility.cpp"
                "src/Parser/parser.cpp"
                "src/Parser/processmessage.cpp"
                "src/Fred/alfclients.cpp"
                "src/Fred/alfrpcinfo.cpp"
                "src/Fred/fred.cpp"
                "src/Fred/fredtopics.cpp"
                "src/Fred/mappedcommand.cpp"
                "src/Fred/groupcommand.cpp"
                "src/Fred/queue.cpp"
		"src/Fred/section.cpp"
                "src/Fred/subscribecommand.cpp"
                "src/Fred/alfinfo.cpp"
                "src/Fred/Config/groups.cpp"
                "src/Fred/Config/instructions.cpp"
                "src/Fred/Config/location.cpp"
                "src/Fred/Config/mapping.cpp"
                "src/Mapi/*.cpp"
	)

add_executable(FREDServer ${SOURCES})
target_link_libraries(FREDServer Alfred)
