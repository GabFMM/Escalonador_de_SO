# Specify the minimum required CMake version
cmake_minimum_required(VERSION 3.22.1)

# Define the project name
project(Scheduler LANGUAGES CXX)

# Specify the C++ standard to use
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add an executable target
# The first argument is the executable name
# Subsequent arguments are the source files for the executable
add_executable(Scheduler.exe 
	main.cpp
        ExtraInfo.cpp
	ExtraInfo.h
	GanttChartGenerator.cpp
	GanttChartGenerator.h
	Menu.cpp
	Menu.h
	Scheduler.cpp
	Scheduler.h
	Simulator.cpp
	Simulator.h
	TCB.cpp
	TCB.h
	IO_operation.cpp
	IO_operation.h
)

# Build type padrão: Debug
set(CMAKE_BUILD_TYPE Debug)