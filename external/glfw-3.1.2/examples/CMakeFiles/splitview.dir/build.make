# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.6

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/scallywag/game/external

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/scallywag/game/external

# Include any dependencies generated for this target.
include glfw-3.1.2/examples/CMakeFiles/splitview.dir/depend.make

# Include the progress variables for this target.
include glfw-3.1.2/examples/CMakeFiles/splitview.dir/progress.make

# Include the compile flags for this target's objects.
include glfw-3.1.2/examples/CMakeFiles/splitview.dir/flags.make

glfw-3.1.2/examples/CMakeFiles/splitview.dir/splitview.c.o: glfw-3.1.2/examples/CMakeFiles/splitview.dir/flags.make
glfw-3.1.2/examples/CMakeFiles/splitview.dir/splitview.c.o: glfw-3.1.2/examples/splitview.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/scallywag/game/external/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object glfw-3.1.2/examples/CMakeFiles/splitview.dir/splitview.c.o"
	cd /home/scallywag/game/external/glfw-3.1.2/examples && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/splitview.dir/splitview.c.o   -c /home/scallywag/game/external/glfw-3.1.2/examples/splitview.c

glfw-3.1.2/examples/CMakeFiles/splitview.dir/splitview.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/splitview.dir/splitview.c.i"
	cd /home/scallywag/game/external/glfw-3.1.2/examples && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/scallywag/game/external/glfw-3.1.2/examples/splitview.c > CMakeFiles/splitview.dir/splitview.c.i

glfw-3.1.2/examples/CMakeFiles/splitview.dir/splitview.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/splitview.dir/splitview.c.s"
	cd /home/scallywag/game/external/glfw-3.1.2/examples && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/scallywag/game/external/glfw-3.1.2/examples/splitview.c -o CMakeFiles/splitview.dir/splitview.c.s

glfw-3.1.2/examples/CMakeFiles/splitview.dir/splitview.c.o.requires:

.PHONY : glfw-3.1.2/examples/CMakeFiles/splitview.dir/splitview.c.o.requires

glfw-3.1.2/examples/CMakeFiles/splitview.dir/splitview.c.o.provides: glfw-3.1.2/examples/CMakeFiles/splitview.dir/splitview.c.o.requires
	$(MAKE) -f glfw-3.1.2/examples/CMakeFiles/splitview.dir/build.make glfw-3.1.2/examples/CMakeFiles/splitview.dir/splitview.c.o.provides.build
.PHONY : glfw-3.1.2/examples/CMakeFiles/splitview.dir/splitview.c.o.provides

glfw-3.1.2/examples/CMakeFiles/splitview.dir/splitview.c.o.provides.build: glfw-3.1.2/examples/CMakeFiles/splitview.dir/splitview.c.o


# Object files for target splitview
splitview_OBJECTS = \
"CMakeFiles/splitview.dir/splitview.c.o"

# External object files for target splitview
splitview_EXTERNAL_OBJECTS =

glfw-3.1.2/examples/splitview: glfw-3.1.2/examples/CMakeFiles/splitview.dir/splitview.c.o
glfw-3.1.2/examples/splitview: glfw-3.1.2/examples/CMakeFiles/splitview.dir/build.make
glfw-3.1.2/examples/splitview: glfw-3.1.2/src/libglfw3.a
glfw-3.1.2/examples/splitview: /usr/lib64/librt.so
glfw-3.1.2/examples/splitview: /usr/lib64/libm.so
glfw-3.1.2/examples/splitview: /usr/lib64/libX11.so
glfw-3.1.2/examples/splitview: /usr/lib64/libXrandr.so
glfw-3.1.2/examples/splitview: /usr/lib64/libXinerama.so
glfw-3.1.2/examples/splitview: /usr/lib64/libXi.so
glfw-3.1.2/examples/splitview: /usr/lib64/libXxf86vm.so
glfw-3.1.2/examples/splitview: /usr/lib64/libXcursor.so
glfw-3.1.2/examples/splitview: /usr/lib64/libGL.so
glfw-3.1.2/examples/splitview: glfw-3.1.2/examples/CMakeFiles/splitview.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/scallywag/game/external/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable splitview"
	cd /home/scallywag/game/external/glfw-3.1.2/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/splitview.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
glfw-3.1.2/examples/CMakeFiles/splitview.dir/build: glfw-3.1.2/examples/splitview

.PHONY : glfw-3.1.2/examples/CMakeFiles/splitview.dir/build

glfw-3.1.2/examples/CMakeFiles/splitview.dir/requires: glfw-3.1.2/examples/CMakeFiles/splitview.dir/splitview.c.o.requires

.PHONY : glfw-3.1.2/examples/CMakeFiles/splitview.dir/requires

glfw-3.1.2/examples/CMakeFiles/splitview.dir/clean:
	cd /home/scallywag/game/external/glfw-3.1.2/examples && $(CMAKE_COMMAND) -P CMakeFiles/splitview.dir/cmake_clean.cmake
.PHONY : glfw-3.1.2/examples/CMakeFiles/splitview.dir/clean

glfw-3.1.2/examples/CMakeFiles/splitview.dir/depend:
	cd /home/scallywag/game/external && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/scallywag/game/external /home/scallywag/game/external/glfw-3.1.2/examples /home/scallywag/game/external /home/scallywag/game/external/glfw-3.1.2/examples /home/scallywag/game/external/glfw-3.1.2/examples/CMakeFiles/splitview.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : glfw-3.1.2/examples/CMakeFiles/splitview.dir/depend
