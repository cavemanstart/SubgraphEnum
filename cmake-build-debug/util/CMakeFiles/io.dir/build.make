# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.26

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "D:\program\CLion 2023.2.2\bin\cmake\win\x64\bin\cmake.exe"

# The command to remove a file.
RM = "D:\program\CLion 2023.2.2\bin\cmake\win\x64\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\workspace\cLionWorkSpace\HCPathEnum-main

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\workspace\cLionWorkSpace\HCPathEnum-main\cmake-build-debug

# Include any dependencies generated for this target.
include util/CMakeFiles/io.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include util/CMakeFiles/io.dir/compiler_depend.make

# Include the progress variables for this target.
include util/CMakeFiles/io.dir/progress.make

# Include the compile flags for this target's objects.
include util/CMakeFiles/io.dir/flags.make

util/CMakeFiles/io.dir/io/io.cpp.obj: util/CMakeFiles/io.dir/flags.make
util/CMakeFiles/io.dir/io/io.cpp.obj: util/CMakeFiles/io.dir/includes_CXX.rsp
util/CMakeFiles/io.dir/io/io.cpp.obj: D:/workspace/cLionWorkSpace/HCPathEnum-main/util/io/io.cpp
util/CMakeFiles/io.dir/io/io.cpp.obj: util/CMakeFiles/io.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\workspace\cLionWorkSpace\HCPathEnum-main\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object util/CMakeFiles/io.dir/io/io.cpp.obj"
	cd /d D:\workspace\cLionWorkSpace\HCPathEnum-main\cmake-build-debug\util && D:\program\mingw\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT util/CMakeFiles/io.dir/io/io.cpp.obj -MF CMakeFiles\io.dir\io\io.cpp.obj.d -o CMakeFiles\io.dir\io\io.cpp.obj -c D:\workspace\cLionWorkSpace\HCPathEnum-main\util\io\io.cpp

util/CMakeFiles/io.dir/io/io.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/io.dir/io/io.cpp.i"
	cd /d D:\workspace\cLionWorkSpace\HCPathEnum-main\cmake-build-debug\util && D:\program\mingw\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\workspace\cLionWorkSpace\HCPathEnum-main\util\io\io.cpp > CMakeFiles\io.dir\io\io.cpp.i

util/CMakeFiles/io.dir/io/io.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/io.dir/io/io.cpp.s"
	cd /d D:\workspace\cLionWorkSpace\HCPathEnum-main\cmake-build-debug\util && D:\program\mingw\MinGW\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\workspace\cLionWorkSpace\HCPathEnum-main\util\io\io.cpp -o CMakeFiles\io.dir\io\io.cpp.s

# Object files for target io
io_OBJECTS = \
"CMakeFiles/io.dir/io/io.cpp.obj"

# External object files for target io
io_EXTERNAL_OBJECTS =

util/libio.dll: util/CMakeFiles/io.dir/io/io.cpp.obj
util/libio.dll: util/CMakeFiles/io.dir/build.make
util/libio.dll: util/CMakeFiles/io.dir/linkLibs.rsp
util/libio.dll: util/CMakeFiles/io.dir/objects1.rsp
util/libio.dll: util/CMakeFiles/io.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\workspace\cLionWorkSpace\HCPathEnum-main\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library libio.dll"
	cd /d D:\workspace\cLionWorkSpace\HCPathEnum-main\cmake-build-debug\util && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\io.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
util/CMakeFiles/io.dir/build: util/libio.dll
.PHONY : util/CMakeFiles/io.dir/build

util/CMakeFiles/io.dir/clean:
	cd /d D:\workspace\cLionWorkSpace\HCPathEnum-main\cmake-build-debug\util && $(CMAKE_COMMAND) -P CMakeFiles\io.dir\cmake_clean.cmake
.PHONY : util/CMakeFiles/io.dir/clean

util/CMakeFiles/io.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\workspace\cLionWorkSpace\HCPathEnum-main D:\workspace\cLionWorkSpace\HCPathEnum-main\util D:\workspace\cLionWorkSpace\HCPathEnum-main\cmake-build-debug D:\workspace\cLionWorkSpace\HCPathEnum-main\cmake-build-debug\util D:\workspace\cLionWorkSpace\HCPathEnum-main\cmake-build-debug\util\CMakeFiles\io.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : util/CMakeFiles/io.dir/depend

