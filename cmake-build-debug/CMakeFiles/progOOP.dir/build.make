# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.15

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2019.2.3\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2019.2.3\bin\cmake\win\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\markd\CLionProjects\progOOP

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\markd\CLionProjects\progOOP\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/progOOP.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/progOOP.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/progOOP.dir/flags.make

CMakeFiles/progOOP.dir/4task/4_2.cpp.obj: CMakeFiles/progOOP.dir/flags.make
CMakeFiles/progOOP.dir/4task/4_2.cpp.obj: ../4task/4_2.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\markd\CLionProjects\progOOP\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/progOOP.dir/4task/4_2.cpp.obj"
	C:\PROGRA~2\MINGW-~1\I686-8~1.0-P\mingw32\bin\G__~1.EXE  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\progOOP.dir\4task\4_2.cpp.obj -c C:\Users\markd\CLionProjects\progOOP\4task\4_2.cpp

CMakeFiles/progOOP.dir/4task/4_2.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/progOOP.dir/4task/4_2.cpp.i"
	C:\PROGRA~2\MINGW-~1\I686-8~1.0-P\mingw32\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\markd\CLionProjects\progOOP\4task\4_2.cpp > CMakeFiles\progOOP.dir\4task\4_2.cpp.i

CMakeFiles/progOOP.dir/4task/4_2.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/progOOP.dir/4task/4_2.cpp.s"
	C:\PROGRA~2\MINGW-~1\I686-8~1.0-P\mingw32\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\markd\CLionProjects\progOOP\4task\4_2.cpp -o CMakeFiles\progOOP.dir\4task\4_2.cpp.s

# Object files for target progOOP
progOOP_OBJECTS = \
"CMakeFiles/progOOP.dir/4task/4_2.cpp.obj"

# External object files for target progOOP
progOOP_EXTERNAL_OBJECTS =

progOOP.exe: CMakeFiles/progOOP.dir/4task/4_2.cpp.obj
progOOP.exe: CMakeFiles/progOOP.dir/build.make
progOOP.exe: CMakeFiles/progOOP.dir/linklibs.rsp
progOOP.exe: CMakeFiles/progOOP.dir/objects1.rsp
progOOP.exe: CMakeFiles/progOOP.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\markd\CLionProjects\progOOP\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable progOOP.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\progOOP.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/progOOP.dir/build: progOOP.exe

.PHONY : CMakeFiles/progOOP.dir/build

CMakeFiles/progOOP.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\progOOP.dir\cmake_clean.cmake
.PHONY : CMakeFiles/progOOP.dir/clean

CMakeFiles/progOOP.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\markd\CLionProjects\progOOP C:\Users\markd\CLionProjects\progOOP C:\Users\markd\CLionProjects\progOOP\cmake-build-debug C:\Users\markd\CLionProjects\progOOP\cmake-build-debug C:\Users\markd\CLionProjects\progOOP\cmake-build-debug\CMakeFiles\progOOP.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/progOOP.dir/depend

