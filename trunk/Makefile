# OpenLayer Makefile (Deprecated please use the cbuild script)
OVERRIDEDEPRICATED=0

ifeq ($(OVERRIDEDEPRICATED) ,0)
TARGET=LINUX
DEPREQ=deprecated
endif

ifeq ($(TARGET) ,WINDOWS)
ENVIRONMENT=WINDOWS
endif
ifeq ($(TARGET) ,windows)
ENVIRONMENT=WINDOWS
endif
ifeq ($(TARGET) ,LINUX)
ENVIRONMENT=LINUX
endif
ifeq ($(TARGET) ,linux)
ENVIRONMENT=LINUX
endif
ifeq ($(TARGET) ,MACOSX)
ENVIRONMENT=MACOSX
endif
ifeq ($(TARGET) ,macosx)
ENVIRONMENT=MACOSX
endif
ifeq ($(TARGET) ,MSYS)
ENVIRONMENT=MSYS
endif
ifeq ($(TARGET) ,msys)
ENVIRONMENT=MSYS
endif

olENVIRONMENTS=WINDOWS LINUX MACOSX MSYS
olENVIRONMENT=$(filter $(olENVIRONMENTS), $(ENVIRONMENT))

#If the current ENVIRONMENT isn't in our environments list, then quit
ifeq ($(strip $(olENVIRONMENT)),)
$(error Unknown Environment. \
Specify target with either TARGET=WINDOWS, TARGET=LINUX, TARGET=MACOSX, or TARGET=MSYS )
endif

#Preproccessors
ifeq ($(NOTTF) ,1)
PREPROC+=-DOL_NO_TTF
endif

ifeq ($(NOSTATECHANGE) ,1)
PREPROC+=-DOL_NO_STATE_CHANGE
endif

ifeq ($(NOOLDAPI) ,1)
PREPROC+=-DOL_NO_OLD_API
endif

ifeq ($(NOPNG) ,1)
PREPROC+=-DOL_NO_PNG
endif

#Compiler and flags
CC=g++
CFLAGS=-O2 -Wall -Iinclude -Iinclude/OpenLayer -fexpensive-optimizations $(PREPROC)
LDFLAGS=
LIBFLAGS=-lglyph-agl -lfreetype -lldpng -lpng -lz -lagl `allegro-config --libs` -lGL -lGLU

#Archiver
AR=ar rcs

#Remove
RM=-rm -f
RMDIR=-rmdir

#Copy utility
CP=-cp
SLASH=/

#Library name and files 
LIB=libopenlayer.a
LIB_SOURCES=src/Animation.cpp src/FpsCounter.cpp src/LineStrip.cpp src/Settings.cpp \
	src/Bitmap.cpp src/Framebuffer.cpp src/Matrix.cpp src/Setup.cpp \
	src/Blenders.cpp src/GarbageCollector.cpp src/PendingLoad.cpp src/Shape.cpp \
	src/Canvas.cpp src/General.cpp src/Placement.cpp src/TextRenderer.cpp \
	src/Circle.cpp src/GfxRend.cpp src/Polygon.cpp src/TextureInfo.cpp \
	src/Collisions.cpp src/GlDriver.cpp src/Rectangle.cpp src/Transforms.cpp \
	src/Internal.cpp src/RenderModes.cpp src/Effects.cpp src/Line.cpp src/Rgba.cpp

LIB_OBJECTS=$(subst src, obj,$(LIB_SOURCES:.cpp=.o))
LIB_DIRS=obj lib

#Demo names and files
TEXTDEMO=demos/textdemo/textdemo
RENDERBITMAP=demos/renderbitmap/renderbitmap
LINESTRIPDEMO=demos/linestripdemo/linestripdemo
GAMELOOP=demos/gameloop/gameloop
GAMEDEMO=demos/gamedemo/demo
TEXTDEMO_SOURCES=demos/textdemo/Main.cpp 
RENDERBITMAP_SOURCES=demos/renderbitmap/Main.cpp
LINESTRIPDEMO_SOURCES=demos/linestripdemo/Main.cpp
GAMELOOP_SOURCES=demos/gameloop/Main.cpp
GAMEDEMO_SOURCES=demos/gamedemo/Demo.cpp


#Setup Windows Make Environment
ifeq "$(ENVIRONMENT)" "WINDOWS"
LIBFLAGS=-lglyph-agl -lfreetype -lldpng -lpng -lz -lagl -lalleg -luser32 -lgdi32 -lglu32 -lopengl32
RM=del /q
RMDIR=rmdir /s /q
MKDIR=mkdir
CP=-copy
SLASH=\\
endif

#Setup MSYS Make Environment
ifeq "$(ENVIRONMENT)" "MSYS"
LIBFLAGS=-lglyph-agl -lfreetype -lldpng -lpng -lz -lagl -lalleg -luser32 -lgdi32 -lglu32 -lopengl32
RM=-rm -f
RMDIR=-rmdir
MKDIR=-mkdir -p
CP=-cp
SLASH=/
endif

#Setup Linux Make Environment
ifeq "$(ENVIRONMENT)" "LINUX"
LIBFLAGS=-lglyph-agl -lfreetype -lldpng -lpng -lz -lagl `allegro-config --libs` -lGL -lGLU
RM=-rm -f
RMDIR=-rmdir
MKDIR=-mkdir -p
CP=-cp
SLASH=/
endif

#Setup OSX Make Environment
ifeq "$(ENVIRONMENT)" "MACOSX"
LIBFLAGS=-lglyph-agl -lfreetype -lldpng -lpng -lz -lagl `allegro-config --libs` -framework OpenGL -framework Carbon
RM=-rm -f
RMDIR=-rmdir
MKDIR=-mkdir
CP=-cp
SLASH=/
endif

#installation data
INSTALL_LOC=You must specify a location for install omitting the trailing slash ie:
INSTALL_MSG=make install LOCATION=/path or make install LOCATION=c:\mypath
DO_INSTALL=noinstall

UNINSTALL_LOC=Specify the path where OpenLayer is installed omitting the trailing slash ie:
UNINSTALL_MSG=make uninstall INSTALL_LOCATION=/path or make uninstall INSTALL_LOCATION=c:\mypath 
DO_UNINSTALL=nouninstall

#checks for doing installation
ifdef LOCATION
INSTALL_LOC=****ERROR: Invalid install location
INSTALL_MSG=LOCATION must have a lib and include directory
ifeq ($(ENVIRONMENT) ,LINUX)
ifeq "$(findstring lib, $(shell ls $(LOCATION)))" "lib"
ifeq "$(findstring include, $(shell ls $(LOCATION)))" "include"
INSTALL_LOC=****ERROR: You must compile the library first
INSTALL_MSG=Try running make
ifeq "$(findstring $(LIB), $(shell ls lib/$(LIB)))" "$(LIB)"
DO_INSTALL=installme
INSTALL_LOC=$(LOCATION)
INSTALL_MSG=To uninstall run make uninstall
endif
endif
endif
endif
ifeq ($(ENVIRONMENT) ,WINDOWS)
ifeq "$(findstring lib, $(shell dir $(LOCATION)))" "lib"
ifeq "$(findstring include, $(shell dir $(LOCATION)))" "include"
NSTALL_LOC=****ERROR: You must compile the library first
INSTALL_MSG=Try running make
ifeq "$(findstring $(LIB), $(shell dir lib\\$(LIB)))" "$(LIB)"
DO_INSTALL=installme
INSTALL_LOC=$(LOCATION)
INSTALL_MSG=To uninstall run make uninstall
endif
endif
endif
endif
endif

#checks for doing uninstallation
ifdef INSTALL_LOCATION
UNINSTALL_LOC=****ERROR: Invalid installation location
UNINSTALL_MSG=INSTALL_LOCATION does not seem to have OpenLayer files
ifeq ($(ENVIRONMENT) ,LINUX)
ifeq "$(findstring $(LIB), $(shell ls $(INSTALL_LOCATION)/lib/$(LIB)))" "$(LIB)"
UNINSTALL_LOC=$(INSTALL_LOCATION)
UNINSTALL_MSG=OpenLayer has been successfully uninstalled
DO_UNINSTALL=uninstallme
endif
endif
ifeq ($(ENVIRONMENT) ,WINDOWS)
ifeq "$(findstring $(LIB), $(shell dir $(INSTALL_LOCATION)\\lib\\$(LIB)))" "$(LIB)"
UNINSTALL_LOC=$(INSTALL_LOCATION)
UNINSTALL_MSG=OpenLayer has been successfully uninstalled
DO_UNINSTALL=uninstallme
endif
endif
endif

#LoadPNG Library
LDPNG_DIR=utils$(SLASH)loadpng
LDPNG_SOURCES=$(LDPNG_DIR)$(SLASH)regpng.c $(LDPNG_DIR)$(SLASH)loadpng.c $(LDPNG_DIR)$(SLASH)savepng.c
LDPNG_OBJECTS=$(LDPNG_SOURCES:.c=.o)
LDPNG_LIB=libldpng.a

LDPNG_LOC=You must specify a location for install omitting the trailing slash ie:
LDPNG_MSG=make install-ldpng LDPNG_IN=/path or make install-ldpng LDPNG_IN=c:\mypath
LDPNG_INSTALL=noldpngin

ifdef LDPNG_IN
LDPNG_LOC=****ERROR: Invalid installation location
LDPNG_MSG=LOCATION must have a lib and include directory
ifeq ($(ENVIRONMENT) ,LINUX)
ifeq "$(findstring lib, $(shell ls $(LDPNG_IN)))" "lib"
ifeq "$(findstring include, $(shell ls $(LDPNG_IN)))" "include"
LDPNG_LOC=****ERROR: You must compile the library first
LDPNG_MSG=Try running make loadpng
ifeq "$(findstring $(LDPNG_LIB), $(shell ls $(LDPNG_DIR)/$(LDPNG_LIB)))" "$(LDPNG_LIB)"
LDPNG_INSTALL=ldpngin
LDPNG_LOC=$(LDPNG_IN)
LDPNG_MSG=Done.
endif
endif
endif
endif
ifeq ($(ENVIRONMENT) ,WINDOWS)
ifeq "$(findstring lib, $(shell dir $(LDPNG_IN)))" "lib"
ifeq "$(findstring include, $(shell dir $(LDPNG_IN)))" "include"
LDPNG_LOC=****ERROR: You must compile the library first
LDPNG_MSG=Try running make loadpng
ifeq "$(findstring $(LDPNG_LIB), $(shell dir $(LDPNG_DIR)\\$(LDPNG_LIB)))" "$(LDPNG_LIB)"
LDPNG_INSTALL=ldpngin
LDPNG_LOC=$(LDPNG_IN)
LDPNG_MSG=Done.
endif
endif
endif
endif
endif

#Build the library and the demos
all: $(DEPREQ) $(LIB_SOURCES) lib/$(LIB) demos
	@echo 
	@echo *****************************[Compilation complete]*****************************
	@echo Run "make install" to install the library.
	@echo Demos are available in the demos directory.
	@echo Done.
	
lib/$(LIB): libdirs $(LIB_OBJECTS)
	$(RM) lib$(SLASH)$(LIB)
	$(AR) lib/$(LIB) $(LIB_OBJECTS)

obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c -ansi $< -o $@
	


.PHONY: all clean demos install installme noinstall uninstall uninstallme nouninstall textdemo renderbitmap linestripdemo gameloop gamedemo loadpng install-loadpng deprecated notify

clean: removefiles
	@echo Removing unused directories
	$(RMDIR) $(LIB_DIRS)
	cd demos && make clean;
	@echo All clean
	@echo Finished.
	
removefiles:
	@echo Removing object files
	$(RM) obj$(SLASH)*
	$(RM) $(LDPNG_DIR)$(SLASH)*.o
	@echo Removing Library
	$(RM) lib$(SLASH)$(LIB)
	$(RM) $(LDPNG_DIR)$(SLASH)$(LDPNG_LIB)
	
export
demos: lib/$(LIB) 
	@cd ./demos && make

	
libdirs:
	@$(MKDIR) $(LIB_DIRS)
	
install: $(DO_INSTALL)
	
installme:
	@echo installing to $(INSTALL_LOC)...
	$(CP) lib$(SLASH)$(LIB) $(INSTALL_LOC)$(SLASH)lib
	$(CP) include$(SLASH)OpenLayer.hpp $(INSTALL_LOC)$(SLASH)include
	-mkdir $(INSTALL_LOC)$(SLASH)include$(SLASH)OpenLayer
	$(CP) include$(SLASH)OpenLayer$(SLASH)* $(INSTALL_LOC)$(SLASH)include$(SLASH)OpenLayer
	@echo $(INSTALL_MSG)
	
noinstall:
	@echo $(INSTALL_LOC)
	@echo $(INSTALL_MSG)
	
uninstall: $(DO_UNINSTALL)

uninstallme:
	@echo uninstalling from $(UNINSTALL_LOC)...
	$(RM) $(UNINSTALL_LOC)$(SLASH)lib$(SLASH)$(LIB)
	$(RM) $(UNINSTALL_LOC)$(SLASH)include$(SLASH)OpenLayer.hpp
	$(RM) $(UNINSTALL_LOC)$(SLASH)include$(SLASH)OpenLayer$(SLASH)*
	$(RMDIR) $(UNINSTALL_LOC)$(SLASH)include$(SLASH)OpenLayer
	@echo $(UNINSTALL_MSG)

nouninstall:
	@echo $(UNINSTALL_LOC)
	@echo $(UNINSTALL_MSG)
	

loadpng: $(LDPNG_SOURCES) $(LDPNG_DIR)$(SLASH)$(LDPNG_LIB)
	@echo Done. Run make install-loadpng to install.

$(LDPNG_DIR)$(SLASH)$(LDPNG_LIB): $(LDPNG_OBJECTS)
	$(RM) $(LDPNG_DIR)$(SLASH)$(LDPNG_LIB)
	$(AR) $(LDPNG_DIR)/$(LDPNG_LIB) $(LDPNG_OBJECTS)
	
$(LDPNG_DIR)$(SLASH)%.o: $(LDPNG_DIR)$(SLASH)%.c
	$(CC) $(CFLAGS) -c -ansi $< -o $@

install-loadpng: $(LDPNG_INSTALL)

ldpngin:
	@echo installing to $(LDPNG_LOC)...
	$(CP) $(LDPNG_DIR)$(SLASH)$(LDPNG_LIB) $(LDPNG_IN)$(SLASH)lib
	$(CP) $(LDPNG_DIR)$(SLASH)loadpng.h $(LDPNG_IN)$(SLASH)include
	@echo $(LDPNG_MSG)
	
noldpngin:
	@echo $(LDPNG_LOC)
	@echo $(LDPNG_MSG)

deprecated: notify
	$(error Finished.)

notify:
	@echo The makefile has been deprecated.
	@echo Please use the cbuild script that is provided.
	@echo For more information please read INSTALL.
	@echo If you insist on using the makefile edit Makefile and
	@echo change OVERRIDEDEPRICATED to equal 1
	
