# -------------------------------------------------------
# OpenLayer cmake build script. Creates makefiles for unix 
# based systems and/or
# project files for a given environment like MSVC.
#
# Written by: juvinious
# -------------------------------------------------------

# -------------------------------------------------------
# Ensure that we are doing an out of source build
# Prevents any mishaps
# -------------------------------------------------------
if(EXISTS ${CMAKE_SOURCE_DIR}/CMakeCache.txt)
	file(REMOVE ${CMAKE_SOURCE_DIR}/CMakeCache.txt)
	file(REMOVE_RECURSE ${CMAKE_SOURCE_DIR}/CMakeFiles)
endif(EXISTS ${CMAKE_SOURCE_DIR}/CMakeCache.txt)      
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	  message(FATAL_ERROR "Oops. Don't do an in-source build. Create an extra directory change into it and run cmake pointing to the base directory. IE: mkdir mybuild; cd mybuild; cmakesetup ../")
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})


# -------------------------------------------------------
# Directory in which extra macros can be found
# -------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/build)

# -------------------------------------------------------
# Add in uninstall target 
# -------------------------------------------------------
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/build/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# -------------------------------------------------------
# project name
# -------------------------------------------------------
project (OpenLayer)

# -------------------------------------------------------
# Including needed macros
include(CMakeMacros)

# -------------------------------------------------------
# User setable options
# -------------------------------------------------------
option(STATIC_LIB "Make the library static?" on)
option(OL_NO_TTF "Disable TTF" off)
if(NOT OL_NO_TTF)
	set(TTF_OK 1)
endif(NOT OL_NO_TTF)
dependent_option(USE_NEW_TTF "Use internal font renderer" on "TTF_OK" on)
option(OL_NO_PNG "Disable PNG support" off)
option(OL_NO_OLD_API "Disable backwards compatibility with 2.0 and below" off)
option(OL_NO_STATE_CHANGE "Disable state changes" off)

# -------------------------------------------------------
# Check whether to create a shared or static library
# -------------------------------------------------------
if(STATIC_LIB)
	set(STATIC_LIB STATIC)
else(STATIC_LIB)
	set(STATIC_LIB SHARED)
endif(STATIC_LIB)

# -------------------------------------------------------
# Paths and system setup
# -------------------------------------------------------

# -------------------------------------------------------
# UNIX BASED SYSTEMS
# -------------------------------------------------------
if(UNIX AND NOT CYGWIN)
	# -------------------------------------------------------
	# Take those user options and set the necessary compile time preprocessors
	# -------------------------------------------------------
	if(NOT OL_NO_TTF)
		if(USE_NEW_TTF)
			add_definitions(-DUSE_NEW_TTF)
		endif(USE_NEW_TTF)
	else(NOT OL_NO_TTF)
		add_definitions(-DOL_NO_TTF)
	endif(NOT OL_NO_TTF)
	if(OL_NO_PNG)
		add_definitions(-DOL_NO_PNG)
	endif(OL_NO_PNG)
	if(OL_NO_OLD_API)
		add_definitions(-DOL_NO_OLD_API)
	endif(OL_NO_OLD_API)
	if(OL_NO_STATE_CHANGE)
		add_definitions(-DOL_NO_STATE_CHANGE)
	endif(OL_NO_STATE_CHANGE)
	#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/unix-build)
	set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
	
	#libraries and stuff
	#set(ALLEGRO_LIBS '`allegro-config --libs`')
	exec_program(allegro-config ARGS --libs OUTPUT_VARIABLE ALLEGRO_LIBS)
	set(ALLEGROGL_LIBS -lagl)
	if(NOT OL_NO_PNG)
		set(PNG_LIBS -lldpng -lpng)
	endif(NOT OL_NO_PNG)
	if(NOT OL_NO_TTF)
		add_definitions("`freetype-config --cflags`")
		exec_program(freetype-config ARGS --cflags OUTPUT_VARIABLE FREETYPE_INCLUDE)
		exec_program(freetype-config ARGS --libs OUTPUT_VARIABLE FT_LIBS)
		if(NOT USE_NEW_TTF)
			set(FREETYPE_LIBS -lglyph-agl ${FT_LIBS})
		else(NOT USE_NEW_TTF)
			set(FREETYPE_LIBS ${FT_LIBS})
		endif(NOT USE_NEW_TTF)
	endif(NOT OL_NO_TTF)
	set(OPENGL_LIBS -lGL -lGLU)
endif(UNIX AND NOT CYGWIN)

# -------------------------------------------------------
# MINGW
# -------------------------------------------------------

if(MINGW)
	# -------------------------------------------------------
	# Take those user options and set the necessary compile time preprocessors
	# -------------------------------------------------------
	if(NOT OL_NO_TTF)
		if(USE_NEW_TTF)
			add_definitions(-DUSE_NEW_TTF)
		endif(USE_NEW_TTF)
	else(NOT OL_NO_TTF)
		add_definitions(-DOL_NO_TTF)
	endif(NOT OL_NO_TTF)
	if(OL_NO_PNG)
		add_definitions(-DOL_NO_PNG)
	endif(OL_NO_PNG)
	if(OL_NO_OLD_API)
		add_definitions(-DOL_NO_OLD_API)
	endif(OL_NO_OLD_API)
	if(OL_NO_STATE_CHANGE)
		add_definitions(-DOL_NO_STATE_CHANGE)
	endif(OL_NO_STATE_CHANGE)
	
	#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/mingw-build)
	set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
	
	find_path(CMAKE_INCLUDE_PATH NAMES stdio.h PATHS c:/mingw/include)
	find_path(CMAKE_LIBRARY_PATH NAMES libstdc++.a PATHS c:/mingw/lib)
	if(NOT CMAKE_INCLUDE_PATH OR NOT CMAKE_LIBRARY_PATH)
		message(FATAL_ERROR "Cannot find your mingw include or library paths. Please set them.")
	endif(NOT CMAKE_INCLUDE_PATH OR NOT CMAKE_LIBRARY_PATH)
	
	# -------------------------------------------------------	
	# library checks
	# Finding all the required libs needed to compile openlayer
	# List:
	#		* allegro
	#		* allegroGL
	#		* freetype (glyphkeeper depending on user settings)
	#		* libpng
	# -------------------------------------------------------
	set(ALLEGRO_LIBS -lalleg)
	set(ALLEGROGL_LIBS -lagl)
	if(NOT OL_NO_PNG)
		set(PNG_LIBS -lldpng -lpng)
	endif(NOT OL_NO_PNG)
	if(NOT OL_NO_TTF)
		find_path(FREETYPE_INCLUDE NAMES freetype PATHS c:/mingw/include/freetype2)
		if(NOT FREETYPE_INCLUDE)
			message(FATAL_ERROR "Cannot find freetype include directory. Please indicate where the directory is and reconfigure.")
		endif(NOT FREETYPE_INCLUDE)
		if(NOT USE_NEW_TTF)
			set(FREETYPE_LIBS -lglyph-agl -lfreetype -lz)
		else(NOT USE_NEW_TTF)
			set(FREETYPE_LIBS -lfreetype -lz)
		endif(NOT USE_NEW_TTF)
	endif(NOT OL_NO_TTF)
	set(OPENGL_LIBS -lgdi32 -lglu32 -lopengl32)
	
	find_library(ALLEGRO NAMES alleg)
	if(NOT ALLEGRO)
		message(FATAL_ERROR "Cannot find allegro. Please indicate where the library is and reconfigure.")
	endif(NOT ALLEGRO)
	find_library(ALLEGROGL NAMES agl)
	if(NOT ALLEGROGL)
		message(FATAL_ERROR "Cannot find allegroGL. Please indicate where the library is and reconfigure.")
	endif(NOT ALLEGROGL)
	if(NOT OL_NO_TTF)
		find_library(FREETYPE NAMES freetype)
		if(NOT FREETYPE)
			message(FATAL_ERROR "Cannot find freetype. Please indicate where the library is and reconfigure.")
		endif(NOT FREETYPE)
		if(NOT USE_NEW_TTF)
			find_library(GLYPKEEPER NAMES glyph-agl)
			if(NOT GLYPHKEEPER)
				message(FATAL_ERROR "Cannot find GlyphKeeper built with allegrogl target. Please indicate where the library is and reconfigure else use the internal renderer.")
			endif(NOT GLYPHKEEPER)
		endif(NOT USE_NEW_TTF)
	endif(NOT OL_NO_TTF)
	if(NOT OL_NO_PNG)
		find_library(LIBPNG NAMES png png12)
		if(NOT LIBPNG)
			message(FATAL_ERROR "Cannot find libpng. Please indicate where the library is and reconfigure else disable png support.")
		else(NOT LIBPNG)
			find_library(LOADPNG NAMES ldpng)
			if(NOT LOADPNG)
				message(FATAL_ERROR "Cannot find loadpng. Please indicate where the library is and reconfigure else disable png support.")
			endif(NOT LOADPNG)
		endif(NOT LIBPNG)
	endif(NOT OL_NO_PNG)
#	-lopenlayer -lglyph-agl -lfreetype -lldpng -lpng -lz -lagl -lalleg -luser32 #-lgdi32 -lglu32-lopengl32
endif(MINGW)

if(MSVC)
	# -------------------------------------------------------
	# Take those user options and set the necessary compile time preprocessors
	# -------------------------------------------------------
	set(STATIC_LIB SHARED)
	set(CMAKE_CONFIGURATION_TYPES Release)
	if(NOT OL_NO_TTF)
		if(USE_NEW_TTF)
			add_definitions(/DUSE_NEW_TTF)
		endif(USE_NEW_TTF)
	else(NOT OL_NO_TTF)
		add_definitions(/DOL_NO_TTF)
	endif(NOT OL_NO_TTF)
	if(OL_NO_PNG)
		add_definitions(/DOL_NO_PNG)
	endif(OL_NO_PNG)
	if(OL_NO_OLD_API)
		add_definitions(/DOL_NO_OLD_API)
	endif(OL_NO_OLD_API)
	if(OL_NO_STATE_CHANGE)
		add_definitions(/DOL_NO_STATE_CHANGE)
	endif(OL_NO_STATE_CHANGE)
	
	#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/msvc-build)
	set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
	
	find_path(CMAKE_LIBRARY_PATH NAMES msvcrt.lib)
	find_path(CMAKE_INCLUDE_PATH NAMES stdio.h PATHS "C:\\Program Files\\Microsoft Visual Studio 8\\VC\\include")
	if(NOT CMAKE_INCLUDE_PATH OR NOT CMAKE_LIBRARY_PATH)
		message(FATAL_ERROR "Cannot find your msvc include or library paths. Please set them.")
	endif(NOT CMAKE_INCLUDE_PATH OR NOT CMAKE_LIBRARY_PATH)
	
	# -------------------------------------------------------	
	# library checks
	# Finding all the required libs needed to compile openlayer
	# List:
	#		* allegro
	#		* allegroGL
	#		* freetype (glyphkeeper depending on user settings)
	#		* libpng
	# -------------------------------------------------------
	set(ALLEGRO_LIBS /alleg)
	set(ALLEGROGL_LIBS /agl)
	if(NOT OL_NO_PNG)
		set(PNG_LIBS /ldpng /libpng)
	endif(NOT OL_NO_PNG)
	if(NOT OL_NO_TTF)
		find_path(FREETYPE_INCLUDE NAMES freetype PATHS ${CMAKE_INCLUDE_PATH}/freetype2)
		if(NOT FREETYPE_INCLUDE)
			message(FATAL_ERROR "Cannot find freetype include directory. Please indicate where the directory is and reconfigure.")
		endif(NOT FREETYPE_INCLUDE)
		if(NOT USE_NEW_TTF)
			set(FREETYPE_LIBS /glyph-agl /freetype /zlib)
		else(NOT USE_NEW_TTF)
			set(FREETYPE_LIBS /freetype /zlib)
		endif(NOT USE_NEW_TTF)
	endif(NOT OL_NO_TTF)
	set(OPENGL_LIBS /gdi32 /glu32 /opengl32)
	
	find_library(ALLEGRO NAMES alleg)
	if(NOT ALLEGRO)
		message(FATAL_ERROR "Cannot find allegro. Please indicate where the library is and reconfigure.")
	endif(NOT ALLEGRO)
	find_library(ALLEGROGL NAMES agl)
	if(NOT ALLEGROGL)
		message(FATAL_ERROR "Cannot find allegroGL. Please indicate where the library is and reconfigure.")
	endif(NOT ALLEGROGL)
	if(NOT OL_NO_TTF)
		find_library(FREETYPE NAMES freetype)
		if(NOT FREETYPE)
			message(FATAL_ERROR "Cannot find freetype. Please indicate where the library is and reconfigure.")
		endif(NOT FREETYPE)
		if(NOT USE_NEW_TTF)
			find_library(GLYPKEEPER NAMES glyph-agl)
			if(NOT GLYPHKEEPER)
				message(FATAL_ERROR "Cannot find GlyphKeeper built with allegrogl target. Please indicate where the library is and reconfigure else use the internal renderer.")
			endif(NOT GLYPHKEEPER)
		endif(NOT USE_NEW_TTF)
	endif(NOT OL_NO_TTF)
	if(NOT OL_NO_PNG)
		find_library(LIBPNG NAMES png libpng png12)
		if(NOT LIBPNG)
			message(FATAL_ERROR "Cannot find libpng. Please indicate where the library is and reconfigure else disable png support.")
		else(NOT LIBPNG)
			find_library(LOADPNG NAMES ldpng)
			if(NOT LOADPNG)
				message(FATAL_ERROR "Cannot find loadpng. Please indicate where the library is and reconfigure else disable png support.")
			endif(NOT LOADPNG)
		endif(NOT LIBPNG)
	endif(NOT OL_NO_PNG)
#	-lopenlayer -lglyph-agl -lfreetype -lldpng -lpng -lz -lagl -lalleg -luser32 #-lgdi32 -lglu32-lopengl32
endif(MSVC)

# -------------------------------------------------------
# TODO:
# 		* CGWIN
#		* MSVC
# -------------------------------------------------------

#if(CYGWIN)
#	find_path(CMAKE_INCLUDE_PATH NAMES stdio.h PATHS c:/mingw/include)
#	find_path(CMAKE_LIBRARY_PATH NAMES libstdc++.a PATHS c:/mingw/lib)
#	if(NOT CMAKE_INCLUDE_PATH AND NOT CMAKE_LIBRARY_PATH)
#		message(FATAL_ERROR "Cannot find your mingw include and library paths. #Please set them.")
#	endif(NOT CMAKE_INCLUDE_PATH AND NOT CMAKE_LIBRARY_PATH)
#endif(CYGWIN)



# -------------------------------------------------------
# Put the linked libraries together
# -------------------------------------------------------
set(REQUIRED_LIBS ${ALLEGROGL_LIBS} ${PNG_LIBS} ${FREETYPE_LIBS} ${ALLEGRO_LIBS} ${OPENGL_LIBS})

# -------------------------------------------------------
# Create the necessary configure file for useage
# -------------------------------------------------------
configure_file(build/OpenLayer.hpp.cmake ${CMAKE_BINARY_DIR}/include/OpenLayer.hpp)

# -------------------------------------------------------
# Source directory containing all the necessary .cpp files
# -------------------------------------------------------
aux_source_directory(src SOURCES)

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
include_directories(include include/OpenLayer ${FREETYPE_INCLUDE})

# -------------------------------------------------------
# OpenLayer Library
# -------------------------------------------------------
add_library (openlayer ${STATIC_LIB} ${SOURCES})

# -------------------------------------------------------
# Create the library
# -------------------------------------------------------
target_link_libraries(openlayer ${REQUIRED_LIBS})

# -------------------------------------------------------
# Create the examples
# -------------------------------------------------------
add_executable(collisiondemo ${CMAKE_SOURCE_DIR}/demos/collisiondemo/main.cpp)
target_link_libraries(collisiondemo openlayer)
add_executable(gamedemo ${CMAKE_SOURCE_DIR}/demos/gamedemo/Demo.cpp)
target_link_libraries(gamedemo openlayer)
add_executable(gameloop ${CMAKE_SOURCE_DIR}/demos/gameloop/Main.cpp)
target_link_libraries(gameloop openlayer)
add_executable(linestripdemo ${CMAKE_SOURCE_DIR}/demos/linestripdemo/Main.cpp)
target_link_libraries(linestripdemo openlayer)
add_executable(renderbitmap ${CMAKE_SOURCE_DIR}/demos/renderbitmap/Main.cpp)
target_link_libraries(renderbitmap openlayer)
add_executable(shapedemo ${CMAKE_SOURCE_DIR}/demos/shapedemo/Main.cpp)
target_link_libraries(shapedemo openlayer)
add_executable(textdemo ${CMAKE_SOURCE_DIR}/demos/textdemo/Main.cpp)
target_link_libraries(textdemo openlayer)

# -------------------------------------------------------
# openlayer-config for *nix based systems
# -------------------------------------------------------
if(UNIX AND NOT CGWIN)
	include(ol-config)
endif(UNIX AND NOT CGWIN)

# -------------------------------------------------------
# Installation
# -------------------------------------------------------
if(NOT MSVC)
	FILE(GLOB HEADERS ${CMAKE_SOURCE_DIR}/include/OpenLayer/*.hpp)
	if(NOT ${CMAKE_INSTALL_PREFIX} AND ${CMAKE_INSTALL_PREFIX} STREQUAL "")
		message(FATAL_ERROR "You need to set the base location where to install OpenLayer (ie /usr/local) - ${CMAKE_INSTALL_PREFIX}")
	endif(NOT ${CMAKE_INSTALL_PREFIX} AND ${CMAKE_INSTALL_PREFIX} STREQUAL "")
	install(FILES ${CMAKE_BINARY_DIR}/include/OpenLayer.hpp DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
	install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/OpenLayer)
	install(TARGETS openlayer DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
	if(UNIX AND NOT CGWIN)
		install(PROGRAMS ${CMAKE_BINARY_DIR}/bin/openlayer-config DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	endif(UNIX AND NOT CGWIN)
endif(NOT MSVC)
