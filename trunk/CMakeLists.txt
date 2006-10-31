# -------------------------------------------------------
# project name
# -------------------------------------------------------
project (OPENLAYER)

# -------------------------------------------------------
# Directory in which extra macros can be found
# -------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/build)

# -------------------------------------------------------
# Including needed macros
include(CMakeMacros)

# -------------------------------------------------------
# User setable options
# -------------------------------------------------------
option(SHARED_LIB "Make the library shared?" off)
option(OL_NO_TTF "Disable TTF" off)
if(NOT OL_NO_TTF)
	set(TTF_OK 1)
endif(NOT OL_NO_TTF)
dependent_option(USE_NEW_TTF "Use internal font renderer" on "TTF_OK" on)
option(OL_NO_PNG "Disable PNG support" off)
option(OL_NO_OLD_API "Disable backwards compatibility with 2.0 and below" off)
option(OL_NO_STATE_CHANGE "Disable state changes" off)

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

# -------------------------------------------------------
# Paths and system setup
# -------------------------------------------------------

# -------------------------------------------------------
# UNIX BASED SYSTEMS
# -------------------------------------------------------
if(UNIX AND NOT CYGWIN)
	#set(CMAKE_INCLUDE_PATH "/usr/include;/usr/local/include")
	#set(CMAKE_LIBRARY_PATH "/usr/lib;/usr/local/lib")
	
	#libraries and stuff
	set(ALLEGRO_LIBS `allegro-config --libs`)
	set(ALLEGROGL_LIBS -lagl)
	if(NOT OL_NO_PNG)
		set(PNG_LIBS -lldpng -lpng)
	endif(NOT OL_NO_PNG)
	if(NOT OL_NO_TTF)
		add_definitions(`freetype-config --cflags`)
		if(NOT USE_NEW_TTF)
			set(FREETYPE_LIBS -lglyph-agl `freetype-config --libs`)
		else(NOT USE_NEW_TTF)
			set(FREETYPE_LIBS `freetype-config --libs`)
		endif(NOT USE_NEW_TTF)
	endif(NOT OL_NO_TTF)
	set(OPENGL_LIBS -lgl -lglu)
endif(UNIX AND NOT CYGWIN)

# -------------------------------------------------------
# MINGW
# -------------------------------------------------------

if(MINGW)
	set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/mingw-build)
	#set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_SOURCE_DIR}/mingw-build)
	#set(PROJECT_BINARY_DIR ${CMAKE_SOURCE_DIR}/mingw-build)
	set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
	
	find_path(CMAKE_INCLUDE_PATH NAMES stdio.h PATHS c:/mingw/include)
	find_path(CMAKE_LIBRARY_PATH NAMES libstdc++.a PATHS c:/mingw/lib)
	if(NOT CMAKE_INCLUDE_PATH AND NOT CMAKE_LIBRARY_PATH)
		message(FATAL_ERROR "Cannot find your mingw include and library paths. Please set them.")
	endif(NOT CMAKE_INCLUDE_PATH AND NOT CMAKE_LIBRARY_PATH)
	
	#libraries and stuff
	set(ALLEGRO_LIBS -lalleg)
	set(ALLEGROGL_LIBS -lagl)
	if(NOT OL_NO_PNG)
		set(PNG_LIBS -lldpng -lpng)
	endif(NOT OL_NO_PNG)
	if(NOT OL_NO_TTF)
		find_path(FREETYPE_INCLUDE NAMES freetype PATHS c:/mingw/include/freetype2)
		if(NOT FREETYPE_INCLUDE)
			message(FATAL_ERROR "Cannot find freetype include directory. Please indicate where the directory is and reconfigure.")
		else(NOT FREETYPE_INCLUDE)
			add_definitions(-I${FREETYPE_INCLUDE})
		endif(NOT FREETYPE_INCLUDE)
		if(NOT USE_NEW_TTF)
			set(FREETYPE_LIBS -lglyph-agl -lfreetype -lz)
		else(NOT USE_NEW_TTF)
			set(FREETYPE_LIBS -lfreetype -lz)
		endif(NOT USE_NEW_TTF)
	endif(NOT OL_NO_TTF)
	set(OPENGL_LIBS -lgdi32 -lglu32 -lopengl32)
#	-lopenlayer -lglyph-agl -lfreetype -lldpng -lpng -lz -lagl -lalleg -luser32 #-lgdi32 -lglu32-lopengl32
endif(MINGW)

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
# library checks
# Finding all the required libs needed to compile openlayer
# List:
#		* allegro
#		* allegroGL
#		* freetype (glyphkeeper depending on user settings)
#		* libpng
# -------------------------------------------------------
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
include_directories(include include/OpenLayer)

# -------------------------------------------------------
# Check whether to create a shared or static library
# -------------------------------------------------------
if(NOT SHARED_LIB)
	set(SHARED_LIB STATIC)
else(NOT SHARED_LIB)
	set(SHARED_LIB SHARED)
endif(NOT SHARED_LIB)

# -------------------------------------------------------
# OpenLayer Library
# -------------------------------------------------------
add_library (openlayer ${SHARED_LIB} ${SOURCES})

# -------------------------------------------------------
# Link it up
# -------------------------------------------------------
target_link_libraries(openlayer ${REQUIRED_LIBS})

# -------------------------------------------------------
# TODO:
#		* Build example executables
#		* Create openlayer-config script on *nix based systems
#		* Do install
# -------------------------------------------------------

# -------------------------------------------------------
# Written by: juvinious
# -------------------------------------------------------
