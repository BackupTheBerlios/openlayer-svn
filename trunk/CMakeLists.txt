# -------------------------------------------------------
# OpenLayer cmake build script. Creates makefiles for unix 
# based systems and/or
# project files for a given environment like MSVC.
#
# TODO:
#	* Create CYGWIN TARGET
#	* Fix MSVC
#	* Seperate into modules?
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
# -------------------------------------------------------
include(CMakeMacros)

# -------------------------------------------------------
# User setable options
# -------------------------------------------------------
option(CREATE_STATIC_LIB "Make the library static?" on)
option(DISABLE_TTF "Disable TTF" off)
if(NOT DISABLE_TTF)
	set(TTF_OK 1)
endif(NOT DISABLE_TTF)
dependent_option(ENABLE_INTERNAL_FONT "Use internal font renderer" on "TTF_OK" on)
option(DISABLE_PNG "Disable PNG support" off)
option(DISABLE_OLD_API "Disable backwards compatibility with 2.0 and below" off)
option(DISABLE_STATE_CHANGES "Disable state changes" off)

if(NOT MSVC)
	set(CXXFLAGS "-O2 -Wall -fexpensive-optimizations")
else(NOT MSVC)
	set(CXXFLAGS "-O2")
endif(NOT MSVC)

add_definitions(${CXXFLAGS})

# -------------------------------------------------------
# Check whether to create a shared or static library
# -------------------------------------------------------
if(CREATE_STATIC_LIB)
	set(CREATE_STATIC_LIB STATIC)
else(CREATE_STATIC_LIB)
	set(CREATE_STATIC_LIB SHARED)
endif(CREATE_STATIC_LIB)

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
	if(NOT DISABLE_TTF)
		if(ENABLE_INTERNAL_FONT)
			add_definitions(-DUSE_NEW_TTF)
			set(USE_NEW_TTF 1)
		endif(ENABLE_INTERNAL_FONT)
	else(NOT DISABLE_TTF)
		add_definitions(-DOL_NO_TTF)
		set(OL_NO_TTF 1)
	endif(NOT DISABLE_TTF)
	if(DISABLE_PNG)
		add_definitions(-DOL_NO_PNG)
		set(OL_NO_PNG 1)
	endif(DISABLE_PNG)
	if(DISABLE_OLD_API)
		add_definitions(-DOL_NO_OLD_API)
		set(OL_NO_OLD_API 1)
	endif(DISABLE_OLD_API)
	if(DISABLE_STATE_CHANGES)
		add_definitions(-DOL_NO_STATE_CHANGE)
		set(OL_NO_STATE_CHANGE 1)
	endif(DISABLE_STATE_CHANGES)
	set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
	
	#libraries and stuff
	exec_program(allegro-config ARGS --libs OUTPUT_VARIABLE ALLEGRO_LIB)
	set(ALLEGROGL_LIB -lagl)
	if(NOT DISABLE_PNG)
#		set(LOADPNG_LIB -lldpng)
		set(PNG_LIB -lpng)
	endif(NOT DISABLE_PNG)
	if(NOT DISABLE_TTF)
		add_definitions("`freetype-config --cflags`")
		exec_program(freetype-config ARGS --cflags OUTPUT_VARIABLE FREETYPE_INCLUDE)
		exec_program(freetype-config ARGS --libs OUTPUT_VARIABLE FT_LIBS)
		if(NOT ENABLE_INTERNAL_FONT)
			set(GLYPHKEEPER_LIB -lglyph-agl)
		endif(NOT ENABLE_INTERNAL_FONT)
		set(FREETYPE_LIB ${FT_LIBS})
	endif(NOT DISABLE_TTF)
	set(OPENGL_LIB -lGL\ -lGLU)
endif(UNIX AND NOT CYGWIN)

# -------------------------------------------------------
# MINGW
# -------------------------------------------------------

if(MINGW)
	# -------------------------------------------------------
	# Take those user options and set the necessary compile time preprocessors
	# -------------------------------------------------------
	if(NOT DISABLE_TTF)
		if(ENABLE_INTERNAL_FONT)
			add_definitions(-DUSE_NEW_TTF)
			set(USE_NEW_TTF 1)
		endif(ENABLE_INTERNAL_FONT)
	else(NOT DISABLE_TTF)
		add_definitions(-DOL_NO_TTF)
		set(OL_NO_TTF 1)
	endif(NOT DISABLE_TTF)
	if(DISABLE_PNG)
		add_definitions(-DOL_NO_PNG)
		set(OL_NO_PNG 1)
	endif(DISABLE_PNG)
	if(DISABLE_OLD_API)
		add_definitions(-DOL_NO_OLD_API)
		set(OL_NO_OLD_API 1)
	endif(DISABLE_OLD_API)
	if(DISABLE_STATE_CHANGES)
		add_definitions(-DOL_NO_STATE_CHANGE)
		set(OL_NO_STATE_CHANGE 1)
	endif(DISABLE_STATE_CHANGES)
	
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
	set(ALLEGRO_LIB -lalleg)
	set(ALLEGROGL_LIB -lagl)
	if(NOT DISABLE_PNG)
#		set(LOADPNG_LIB -lldpng)
		set(PNG_LIB -lpng)
	endif(NOT DISABLE_PNG)
	if(NOT DISABLE_TTF)
		find_path(FREETYPE_INCLUDE NAMES freetype PATHS c:/mingw/include/freetype2)
		if(NOT FREETYPE_INCLUDE)
			message(FATAL_ERROR "Cannot find freetype include directory. Please indicate where the directory is and reconfigure.")
		endif(NOT FREETYPE_INCLUDE)
		if(NOT ENABLE_INTERNAL_FONT)
			set(GLYPHKEEPER_LIB -lglyph-agl)
		endif(NOT ENABLE_INTERNAL_FONT)
		set(FREETYPE_LIB -lfreetype)
		set(Z_LIB -lz)
	endif(NOT DISABLE_TTF)
	set(OPENGL_LIB -lgdi32\ -lglu32\ -lopengl32)
	
	find_library(ALLEGRO NAMES alleg)
	if(NOT ALLEGRO)
		message(FATAL_ERROR "Cannot find allegro. Please indicate where the library is and reconfigure.")
	endif(NOT ALLEGRO)
	find_library(ALLEGROGL NAMES agl)
	if(NOT ALLEGROGL)
		message(FATAL_ERROR "Cannot find allegroGL. Please indicate where the library is and reconfigure.")
	endif(NOT ALLEGROGL)
	if(NOT DISABLE_TTF)
		find_library(FREETYPE NAMES freetype)
		if(NOT FREETYPE)
			message(FATAL_ERROR "Cannot find freetype. Please indicate where the library is and reconfigure.")
		endif(NOT FREETYPE)
		if(NOT ENABLE_INTERNAL_FONT)
			find_library(GLYPHKEEPER NAMES glyph-agl)
			if(NOT GLYPHKEEPER)
				message(FATAL_ERROR "Cannot find GlyphKeeper built with allegrogl target. Please indicate where the library is and reconfigure else use the internal renderer.")
			endif(NOT GLYPHKEEPER)
		endif(NOT ENABLE_INTERNAL_FONT)
	endif(NOT DISABLE_TTF)
	if(NOT DISABLE_PNG)
		find_library(LIBPNG NAMES png png12)
		if(NOT LIBPNG)
			message(FATAL_ERROR "Cannot find libpng. Please indicate where the library is and reconfigure else disable png support.")
		else(NOT LIBPNG)
#			find_library(LOADPNG NAMES ldpng)
			if(NOT LOADPNG)
				message(FATAL_ERROR "Cannot find loadpng. Please indicate where the library is and reconfigure else disable png support.")
			endif(NOT LOADPNG)
		endif(NOT LIBPNG)
	endif(NOT DISABLE_PNG)
#	-lopenlayer -lglyph-agl -lfreetype -lldpng -lpng -lz -lagl -lalleg -luser32 #-lgdi32 -lglu32-lopengl32
endif(MINGW)

# -------------------------------------------------------
# MSVC
# -------------------------------------------------------

if(MSVC)
	# -------------------------------------------------------
	# Take those user options and set the necessary compile time preprocessors
	# -------------------------------------------------------
	SET(CMAKE_CXX_FLAGS "/nologo /W3 /Gy")
    SET(CMAKE_CXX_FLAGS_DEBUG "/MTd /Z7 /Od")
    SET(CMAKE_CXX_FLAGS_RELEASE "/MT /O2")
    SET(CMAKE_CXX_FLAGS_MINSIZEREL "/MT /O2")
    SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MTd /Z7 /Od")
	
	set(LIB_NAME_ALLEGRO "alleg" CACHE STRING "Allegro Library lib filename")
	set(LIB_NAME_ALLEGROGL "agl" CACHE STRING "AllegroGL Library lib filename")
	set(LIB_NAME_FREETYPE "freetype" CACHE STRING "Freetype Library lib filename")
	if(NOT ENABLE_INTERNAL_FONT)
		set(LIB_NAME_GLYPHKEEPER "glyph-agl" CACHE STRING "GlyphKeeper Library lib filename")
	endif(NOT ENABLE_INTERNAL_FONT)
	set(LIB_NAME_FREETYPE "freetype" CACHE STRING "Freetype Library lib filename")
	set(LIB_NAME_PNG "libpng" CACHE STRING "PNG Library lib filename")
#	set(LIB_NAME_LOADPNG "ldpng" CACHE STRING "LoadPNG Library lib filename")
	set(LIB_NAME_ZLIB "zlib" CACHE STRING "ZLIB Library lib filename")
	
	set(CMAKE_CONFIGURATION_TYPES Release)
	if(NOT DISABLE_TTF)
		if(ENABLE_INTERNAL_FONT)
			add_definitions(/DUSE_NEW_TTF)
			set(USE_NEW_TTF 1)
		endif(ENABLE_INTERNAL_FONT)
	else(NOT DISABLE_TTF)
		add_definitions(/DOL_NO_TTF)
		set(OL_NO_TTF 1)
	endif(NOT DISABLE_TTF)
	if(DISABLE_PNG)
		add_definitions(/DOL_NO_PNG)
		set(OL_NO_PNG 1)
	endif(DISABLE_PNG)
	if(DISABLE_OLD_API)
		add_definitions(/DOL_NO_OLD_API)
		set(OL_NO_OLD_API 1)
	endif(DISABLE_OLD_API)
	if(DISABLE_STATE_CHANGES)
		add_definitions(/DOL_NO_STATE_CHANGE)
		set(OL_NO_STATE_CHANGE 1)
	endif(DISABLE_STATE_CHANGES)
	
	set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
	
	find_path(CMAKE_LIBRARY_PATH NAMES msvcrt.lib PATHS "C:\\Program Files\\Microsoft Visual Studio 8\\VC\\lib")
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
	set(LIB_LOCATION_ALLEGRO ALLEGRO-NOTFOUND)
	find_library(LIB_LOCATION_ALLEGRO NAMES ${LIB_NAME_ALLEGRO})
	if(NOT LIB_LOCATION_ALLEGRO)
		message(FATAL_ERROR "Cannot find ${LIB_NAME_ALLEGRO}. Please indicate where the library is and reconfigure.")
	endif(NOT LIB_LOCATION_ALLEGRO)
	set(LIB_LOCATION_ALLEGROGL ALLEGROGL-NOTFOUND)
	find_library(LIB_LOCATION_ALLEGROGL NAMES ${LIB_NAME_ALLEGROGL})
	if(NOT LIB_LOCATION_ALLEGROGL)
		message(FATAL_ERROR "Cannot find ${LIB_NAME_ALLEGROGL}. Please indicate where the library is and reconfigure.")
	endif(NOT LIB_LOCATION_ALLEGROGL)
	if(NOT DISABLE_TTF)
		set(LIB_LOCATION_ZLIB ZLIB-NOTFOUND)
		find_library(LIB_LOCATION_ZLIB NAMES ${LIB_NAME_ZLIB})
		if(NOT LIB_LOCATION_ZLIB)
			message(FATAL_ERROR "Cannot find ${LIB_NAME_ZLIB}. Please indicate where the library is and reconfigure.")
		endif(NOT LIB_LOCATION_ZLIB)
		set(LIB_LOCATION_FREETYPE FREETYPE-NOTFOUND)
		find_library(LIB_LOCATION_FREETYPE NAMES ${LIB_NAME_FREETYPE})
		if(NOT LIB_LOCATION_FREETYPE)
			message(FATAL_ERROR "Cannot find ${LIB_NAME_FREETYPE}. Please indicate where the library is and reconfigure.")
		endif(NOT LIB_LOCATION_FREETYPE)
		if(NOT ENABLE_INTERNAL_FONT)
			set(LIB_LOCATION_GLYPHKEEPER GLYPHKEEPER-NOTFOUND)
			find_library(LIB_LOCATION_GLYPHKEEPER NAMES ${LIB_NAME_GLYPHKEEPER})
			if(NOT LIB_LOCATION_GLYPHKEEPER)
				message(FATAL_ERROR "Cannot find ${LIB_NAME_GLYPHKEEPER} built with allegrogl target. Please indicate where the library is and reconfigure else use the internal renderer.")
			endif(NOT LIB_LOCATION_GLYPHKEEPER)
		endif(NOT ENABLE_INTERNAL_FONT)
	endif(NOT DISABLE_TTF)
	if(NOT DISABLE_PNG)
		set(LIB_LOCATION_LIBPNG LIBPNG-NOTFOUND)
		find_library(LIB_LOCATION_LIBPNG NAMES ${LIB_NAME_PNG})
		if(NOT LIB_LOCATION_LIBPNG)
			message(FATAL_ERROR "Cannot find ${LIB_NAME_PNG}. Please indicate where the library is and reconfigure else disable png support.")
		else(NOT LIB_LOCATION_LIBPNG)
#			set(LIB_LOCATION_LOADPNG LOADPNG-NOTFOUND)
#			find_library(LIB_LOCATION_LOADPNG NAMES ${LIB_NAME_LOADPNG})
#			if(NOT LIB_LOCATION_LOADPNG)
#				message(FATAL_ERROR "Cannot find ${LIB_NAME_LOADPNG}. Please indicate where the library is and reconfigure else disable png support.")
#			endif(NOT LIB_LOCATION_LOADPNG)
		endif(NOT LIB_LOCATION_LIBPNG)
	endif(NOT DISABLE_PNG)
	
	set(ALLEGRO_LIB /${LIB_NAME_ALLEGRO})
	set(ALLEGROGL_LIB /${LIB_NAME_ALLEGROGL})
	if(NOT DISABLE_PNG)
#		set(LOADPNG_LIB /${LIB_NAME_LOADPNG})
		set(PNG_LIB /${LIB_NAME_PNG})
	endif(NOT DISABLE_PNG)
	if(NOT DISABLE_TTF)
		find_path(FREETYPE_INCLUDE NAMES freetype PATHS ${CMAKE_INCLUDE_PATH}/freetype2)
		if(NOT FREETYPE_INCLUDE)
			message(FATAL_ERROR "Cannot find freetype include directory. Please indicate where the directory is and reconfigure.")
		endif(NOT FREETYPE_INCLUDE)
		if(NOT ENABLE_INTERNAL_FONT)
			set(GLYPHKEEPER_LIB /${LIB_NAME_GLYPHKEEPER})
		endif(NOT ENABLE_INTERNAL_FONT)
		set(FREETYPE_LIB /${LIB_NAME_FREETYPE})
		set(Z_LIB /${LIB_NAME_ZLIB})
	endif(NOT DISABLE_TTF)
	set(OPENGL_LIB /gdi32 /glu32 /opengl32)
endif(MSVC)

# -------------------------------------------------------
# Put the linked libraries together
# -------------------------------------------------------
if(NOT MSVC)
	set(REQUIRED_LIBS ${ALLEGROGL_LIB}\ ${PNG_LIB}\ ${GLYPHKEEPER_LIB}\ ${FREETYPE_LIB}\ ${Z_LIB}\ ${ALLEGRO_LIB}\ ${OPENGL_LIB})
else(NOT MSVC)
	set(REQUIRED_LIBS ${ALLEGROGL_LIB} ${PNG_LIB} ${GLYPHKEEPER_LIB} ${FREETYPE_LIB} ${Z_LIB} ${ALLEGRO_LIB} ${OPENGL_LIB})
endif(NOT MSVC)
# -------------------------------------------------------
# Create the necessary header file for the library
# -------------------------------------------------------
configure_file(build/OpenLayer.hpp.in ${CMAKE_BINARY_DIR}/include/OpenLayer.hpp)

# -------------------------------------------------------
# openlayer-config for *nix based systems
# -------------------------------------------------------
if(UNIX AND NOT CYGWIN)
	configure_file(build/openlayer-config.in ${CMAKE_BINARY_DIR}/bin/openlayer-config ESCAPE_QUOTES)
endif(UNIX AND NOT CYGWIN)

# -------------------------------------------------------
# Source directory containing all the necessary .cpp files
# -------------------------------------------------------
aux_source_directory(src SOURCES)

# -------------------------------------------------------
# Include directory
# -------------------------------------------------------
include_directories(include ${CMAKE_BINARY_DIR}/include include/OpenLayer ${FREETYPE_INCLUDE})

# -------------------------------------------------------
# OpenLayer Library
# -------------------------------------------------------
add_library (openlayer ${CREATE_STATIC_LIB} ${SOURCES})

# -------------------------------------------------------
# Create the library
# -------------------------------------------------------
target_link_libraries(openlayer ${REQUIRED_LIBS})

# -------------------------------------------------------
# Create the examples
# -------------------------------------------------------
add_executable(collisiondemo ${CMAKE_SOURCE_DIR}/demos/collisiondemo/main.cpp)
target_link_libraries(collisiondemo openlayer)
add_dependencies(collisiondemo openlayer)

add_executable(gamedemo ${CMAKE_SOURCE_DIR}/demos/gamedemo/Demo.cpp)
target_link_libraries(gamedemo openlayer)
add_dependencies(gamedemo openlayer)
configure_file(${CMAKE_SOURCE_DIR}/demos/gamedemo/Fonts/Neuropol.ttf ${CMAKE_BINARY_DIR}/bin/Fonts/Neuropol.ttf COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/demos/gamedemo/Gfx/Background.png ${CMAKE_BINARY_DIR}/bin/Gfx/Background.png COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/demos/gamedemo/Gfx/Ball.png ${CMAKE_BINARY_DIR}/bin/Gfx/Ball.png COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/demos/gamedemo/Gfx/Blocks/Block01.png ${CMAKE_BINARY_DIR}/bin/Gfx/Blocks/Block01.png COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/demos/gamedemo/Gfx/Blocks/Block02.png ${CMAKE_BINARY_DIR}/bin/Gfx/Blocks/Block02.png COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/demos/gamedemo/Gfx/Blocks/Block03.png ${CMAKE_BINARY_DIR}/bin/Gfx/Blocks/Block03.png COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/demos/gamedemo/Gfx/Blocks/Block04.png ${CMAKE_BINARY_DIR}/bin/Gfx/Blocks/Block04.png COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/demos/gamedemo/Gfx/Paddle.png ${CMAKE_BINARY_DIR}/bin/Gfx/Paddle.png COPYONLY)

add_executable(gameloop ${CMAKE_SOURCE_DIR}/demos/gameloop/Main.cpp)
target_link_libraries(gameloop openlayer)
add_dependencies(gameloop openlayer)
configure_file(${CMAKE_SOURCE_DIR}/demos/gameloop/Gfx/Bitmap.png ${CMAKE_BINARY_DIR}/bin/Gfx/Bitmap.png COPYONLY)

add_executable(linestripdemo ${CMAKE_SOURCE_DIR}/demos/linestripdemo/Main.cpp)
target_link_libraries(linestripdemo openlayer)
add_dependencies(linestripdemo openlayer)

add_executable(renderbitmap ${CMAKE_SOURCE_DIR}/demos/renderbitmap/Main.cpp)
target_link_libraries(renderbitmap openlayer)
add_dependencies(renderbitmap openlayer)

add_executable(shapedemo ${CMAKE_SOURCE_DIR}/demos/shapedemo/Main.cpp)
target_link_libraries(shapedemo openlayer)
add_dependencies(shapedemo openlayer)
configure_file(${CMAKE_SOURCE_DIR}/demos/shapedemo/Gfx/PointerAlpha.bmp ${CMAKE_BINARY_DIR}/bin/Gfx/PointerAlpha.bmp COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/demos/shapedemo/Gfx/PointerAlpha.png ${CMAKE_BINARY_DIR}/bin/Gfx/PointerAlpha.png COPYONLY)

add_executable(textdemo ${CMAKE_SOURCE_DIR}/demos/textdemo/Main.cpp)
target_link_libraries(textdemo openlayer)
add_dependencies(textdemo openlayer)

# -------------------------------------------------------
# Installation
# -------------------------------------------------------
if(NOT MSVC)
	FILE(GLOB HEADERS ${CMAKE_SOURCE_DIR}/include/OpenLayer/*.hpp)
	if(NOT ${CMAKE_INSTALL_PREFIX} AND ${CMAKE_INSTALL_PREFIX} STREQUAL "")
		message(FATAL_ERROR "You need to set the base location where to install OpenLayer (ie /usr/local)")
	endif(NOT ${CMAKE_INSTALL_PREFIX} AND ${CMAKE_INSTALL_PREFIX} STREQUAL "")
	install(FILES ${CMAKE_BINARY_DIR}/include/OpenLayer.hpp DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
	install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/OpenLayer)
	install(TARGETS openlayer DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
	if(UNIX AND NOT CYGWIN)
		install(PROGRAMS ${CMAKE_BINARY_DIR}/bin/openlayer-config DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
	endif(UNIX AND NOT CYGWIN)
endif(NOT MSVC)
