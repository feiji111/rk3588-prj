# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

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
CMAKE_SOURCE_DIR = /home/Tronlong/rk3588/detect_track

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/Tronlong/rk3588/detect_track/build

# Include any dependencies generated for this target.
include CMakeFiles/detect_track.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/detect_track.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/detect_track.dir/flags.make

CMakeFiles/detect_track.dir/main.cpp.o: CMakeFiles/detect_track.dir/flags.make
CMakeFiles/detect_track.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/Tronlong/rk3588/detect_track/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/detect_track.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/detect_track.dir/main.cpp.o -c /home/Tronlong/rk3588/detect_track/main.cpp

CMakeFiles/detect_track.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/detect_track.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/Tronlong/rk3588/detect_track/main.cpp > CMakeFiles/detect_track.dir/main.cpp.i

CMakeFiles/detect_track.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/detect_track.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/Tronlong/rk3588/detect_track/main.cpp -o CMakeFiles/detect_track.dir/main.cpp.s

CMakeFiles/detect_track.dir/src/coreNum.cpp.o: CMakeFiles/detect_track.dir/flags.make
CMakeFiles/detect_track.dir/src/coreNum.cpp.o: ../src/coreNum.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/Tronlong/rk3588/detect_track/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/detect_track.dir/src/coreNum.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/detect_track.dir/src/coreNum.cpp.o -c /home/Tronlong/rk3588/detect_track/src/coreNum.cpp

CMakeFiles/detect_track.dir/src/coreNum.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/detect_track.dir/src/coreNum.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/Tronlong/rk3588/detect_track/src/coreNum.cpp > CMakeFiles/detect_track.dir/src/coreNum.cpp.i

CMakeFiles/detect_track.dir/src/coreNum.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/detect_track.dir/src/coreNum.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/Tronlong/rk3588/detect_track/src/coreNum.cpp -o CMakeFiles/detect_track.dir/src/coreNum.cpp.s

CMakeFiles/detect_track.dir/src/model.cpp.o: CMakeFiles/detect_track.dir/flags.make
CMakeFiles/detect_track.dir/src/model.cpp.o: ../src/model.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/Tronlong/rk3588/detect_track/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/detect_track.dir/src/model.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/detect_track.dir/src/model.cpp.o -c /home/Tronlong/rk3588/detect_track/src/model.cpp

CMakeFiles/detect_track.dir/src/model.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/detect_track.dir/src/model.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/Tronlong/rk3588/detect_track/src/model.cpp > CMakeFiles/detect_track.dir/src/model.cpp.i

CMakeFiles/detect_track.dir/src/model.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/detect_track.dir/src/model.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/Tronlong/rk3588/detect_track/src/model.cpp -o CMakeFiles/detect_track.dir/src/model.cpp.s

CMakeFiles/detect_track.dir/src/postprocess.cpp.o: CMakeFiles/detect_track.dir/flags.make
CMakeFiles/detect_track.dir/src/postprocess.cpp.o: ../src/postprocess.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/Tronlong/rk3588/detect_track/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/detect_track.dir/src/postprocess.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/detect_track.dir/src/postprocess.cpp.o -c /home/Tronlong/rk3588/detect_track/src/postprocess.cpp

CMakeFiles/detect_track.dir/src/postprocess.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/detect_track.dir/src/postprocess.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/Tronlong/rk3588/detect_track/src/postprocess.cpp > CMakeFiles/detect_track.dir/src/postprocess.cpp.i

CMakeFiles/detect_track.dir/src/postprocess.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/detect_track.dir/src/postprocess.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/Tronlong/rk3588/detect_track/src/postprocess.cpp -o CMakeFiles/detect_track.dir/src/postprocess.cpp.s

CMakeFiles/detect_track.dir/src/preprocess.cpp.o: CMakeFiles/detect_track.dir/flags.make
CMakeFiles/detect_track.dir/src/preprocess.cpp.o: ../src/preprocess.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/Tronlong/rk3588/detect_track/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/detect_track.dir/src/preprocess.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/detect_track.dir/src/preprocess.cpp.o -c /home/Tronlong/rk3588/detect_track/src/preprocess.cpp

CMakeFiles/detect_track.dir/src/preprocess.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/detect_track.dir/src/preprocess.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/Tronlong/rk3588/detect_track/src/preprocess.cpp > CMakeFiles/detect_track.dir/src/preprocess.cpp.i

CMakeFiles/detect_track.dir/src/preprocess.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/detect_track.dir/src/preprocess.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/Tronlong/rk3588/detect_track/src/preprocess.cpp -o CMakeFiles/detect_track.dir/src/preprocess.cpp.s

# Object files for target detect_track
detect_track_OBJECTS = \
"CMakeFiles/detect_track.dir/main.cpp.o" \
"CMakeFiles/detect_track.dir/src/coreNum.cpp.o" \
"CMakeFiles/detect_track.dir/src/model.cpp.o" \
"CMakeFiles/detect_track.dir/src/postprocess.cpp.o" \
"CMakeFiles/detect_track.dir/src/preprocess.cpp.o"

# External object files for target detect_track
detect_track_EXTERNAL_OBJECTS =

detect_track: CMakeFiles/detect_track.dir/main.cpp.o
detect_track: CMakeFiles/detect_track.dir/src/coreNum.cpp.o
detect_track: CMakeFiles/detect_track.dir/src/model.cpp.o
detect_track: CMakeFiles/detect_track.dir/src/postprocess.cpp.o
detect_track: CMakeFiles/detect_track.dir/src/preprocess.cpp.o
detect_track: CMakeFiles/detect_track.dir/build.make
detect_track: /usr/local/lib/libopencv_gapi.so.4.9.0
detect_track: /usr/local/lib/libopencv_stitching.so.4.9.0
detect_track: /usr/local/lib/libopencv_alphamat.so.4.9.0
detect_track: /usr/local/lib/libopencv_aruco.so.4.9.0
detect_track: /usr/local/lib/libopencv_bgsegm.so.4.9.0
detect_track: /usr/local/lib/libopencv_bioinspired.so.4.9.0
detect_track: /usr/local/lib/libopencv_ccalib.so.4.9.0
detect_track: /usr/local/lib/libopencv_dnn_objdetect.so.4.9.0
detect_track: /usr/local/lib/libopencv_dnn_superres.so.4.9.0
detect_track: /usr/local/lib/libopencv_dpm.so.4.9.0
detect_track: /usr/local/lib/libopencv_face.so.4.9.0
detect_track: /usr/local/lib/libopencv_freetype.so.4.9.0
detect_track: /usr/local/lib/libopencv_fuzzy.so.4.9.0
detect_track: /usr/local/lib/libopencv_hfs.so.4.9.0
detect_track: /usr/local/lib/libopencv_img_hash.so.4.9.0
detect_track: /usr/local/lib/libopencv_intensity_transform.so.4.9.0
detect_track: /usr/local/lib/libopencv_line_descriptor.so.4.9.0
detect_track: /usr/local/lib/libopencv_mcc.so.4.9.0
detect_track: /usr/local/lib/libopencv_quality.so.4.9.0
detect_track: /usr/local/lib/libopencv_rapid.so.4.9.0
detect_track: /usr/local/lib/libopencv_reg.so.4.9.0
detect_track: /usr/local/lib/libopencv_rgbd.so.4.9.0
detect_track: /usr/local/lib/libopencv_saliency.so.4.9.0
detect_track: /usr/local/lib/libopencv_stereo.so.4.9.0
detect_track: /usr/local/lib/libopencv_structured_light.so.4.9.0
detect_track: /usr/local/lib/libopencv_superres.so.4.9.0
detect_track: /usr/local/lib/libopencv_surface_matching.so.4.9.0
detect_track: /usr/local/lib/libopencv_tracking.so.4.9.0
detect_track: /usr/local/lib/libopencv_videostab.so.4.9.0
detect_track: /usr/local/lib/libopencv_wechat_qrcode.so.4.9.0
detect_track: /usr/local/lib/libopencv_xfeatures2d.so.4.9.0
detect_track: /usr/local/lib/libopencv_xobjdetect.so.4.9.0
detect_track: /usr/local/lib/libopencv_xphoto.so.4.9.0
detect_track: ../lib/rknn_api/librknnrt.so
detect_track: ../lib/rga/librga.so
detect_track: /usr/local/lib/libopencv_shape.so.4.9.0
detect_track: /usr/local/lib/libopencv_highgui.so.4.9.0
detect_track: /usr/local/lib/libopencv_datasets.so.4.9.0
detect_track: /usr/local/lib/libopencv_plot.so.4.9.0
detect_track: /usr/local/lib/libopencv_text.so.4.9.0
detect_track: /usr/local/lib/libopencv_ml.so.4.9.0
detect_track: /usr/local/lib/libopencv_phase_unwrapping.so.4.9.0
detect_track: /usr/local/lib/libopencv_optflow.so.4.9.0
detect_track: /usr/local/lib/libopencv_ximgproc.so.4.9.0
detect_track: /usr/local/lib/libopencv_video.so.4.9.0
detect_track: /usr/local/lib/libopencv_videoio.so.4.9.0
detect_track: /usr/local/lib/libopencv_imgcodecs.so.4.9.0
detect_track: /usr/local/lib/libopencv_objdetect.so.4.9.0
detect_track: /usr/local/lib/libopencv_calib3d.so.4.9.0
detect_track: /usr/local/lib/libopencv_dnn.so.4.9.0
detect_track: /usr/local/lib/libopencv_features2d.so.4.9.0
detect_track: /usr/local/lib/libopencv_flann.so.4.9.0
detect_track: /usr/local/lib/libopencv_photo.so.4.9.0
detect_track: /usr/local/lib/libopencv_imgproc.so.4.9.0
detect_track: /usr/local/lib/libopencv_core.so.4.9.0
detect_track: CMakeFiles/detect_track.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/Tronlong/rk3588/detect_track/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable detect_track"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/detect_track.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/detect_track.dir/build: detect_track

.PHONY : CMakeFiles/detect_track.dir/build

CMakeFiles/detect_track.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/detect_track.dir/cmake_clean.cmake
.PHONY : CMakeFiles/detect_track.dir/clean

CMakeFiles/detect_track.dir/depend:
	cd /home/Tronlong/rk3588/detect_track/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/Tronlong/rk3588/detect_track /home/Tronlong/rk3588/detect_track /home/Tronlong/rk3588/detect_track/build /home/Tronlong/rk3588/detect_track/build /home/Tronlong/rk3588/detect_track/build/CMakeFiles/detect_track.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/detect_track.dir/depend

