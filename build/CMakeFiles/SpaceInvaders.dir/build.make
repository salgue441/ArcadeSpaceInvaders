# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/d/developer/SpaceInvaders

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/d/developer/SpaceInvaders/build

# Include any dependencies generated for this target.
include CMakeFiles/SpaceInvaders.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/SpaceInvaders.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/SpaceInvaders.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/SpaceInvaders.dir/flags.make

CMakeFiles/SpaceInvaders.dir/main.cpp.o: CMakeFiles/SpaceInvaders.dir/flags.make
CMakeFiles/SpaceInvaders.dir/main.cpp.o: ../main.cpp
CMakeFiles/SpaceInvaders.dir/main.cpp.o: CMakeFiles/SpaceInvaders.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/developer/SpaceInvaders/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/SpaceInvaders.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/SpaceInvaders.dir/main.cpp.o -MF CMakeFiles/SpaceInvaders.dir/main.cpp.o.d -o CMakeFiles/SpaceInvaders.dir/main.cpp.o -c /mnt/d/developer/SpaceInvaders/main.cpp

CMakeFiles/SpaceInvaders.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/SpaceInvaders.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/developer/SpaceInvaders/main.cpp > CMakeFiles/SpaceInvaders.dir/main.cpp.i

CMakeFiles/SpaceInvaders.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/SpaceInvaders.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/developer/SpaceInvaders/main.cpp -o CMakeFiles/SpaceInvaders.dir/main.cpp.s

# Object files for target SpaceInvaders
SpaceInvaders_OBJECTS = \
"CMakeFiles/SpaceInvaders.dir/main.cpp.o"

# External object files for target SpaceInvaders
SpaceInvaders_EXTERNAL_OBJECTS =

SpaceInvaders: CMakeFiles/SpaceInvaders.dir/main.cpp.o
SpaceInvaders: CMakeFiles/SpaceInvaders.dir/build.make
SpaceInvaders: /usr/lib/x86_64-linux-gnu/libGLEW.so
SpaceInvaders: /usr/lib/x86_64-linux-gnu/libGLX.so
SpaceInvaders: /usr/lib/x86_64-linux-gnu/libOpenGL.so
SpaceInvaders: CMakeFiles/SpaceInvaders.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/d/developer/SpaceInvaders/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable SpaceInvaders"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SpaceInvaders.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/SpaceInvaders.dir/build: SpaceInvaders
.PHONY : CMakeFiles/SpaceInvaders.dir/build

CMakeFiles/SpaceInvaders.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/SpaceInvaders.dir/cmake_clean.cmake
.PHONY : CMakeFiles/SpaceInvaders.dir/clean

CMakeFiles/SpaceInvaders.dir/depend:
	cd /mnt/d/developer/SpaceInvaders/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/d/developer/SpaceInvaders /mnt/d/developer/SpaceInvaders /mnt/d/developer/SpaceInvaders/build /mnt/d/developer/SpaceInvaders/build /mnt/d/developer/SpaceInvaders/build/CMakeFiles/SpaceInvaders.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/SpaceInvaders.dir/depend
