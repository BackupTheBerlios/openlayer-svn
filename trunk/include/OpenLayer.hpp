//     O p e n L a y e r      //
//                            //
// by Esa Tanskanen           //
//                            //
// 2D Graphics routines using //
// OpenGL acceleration        //
//
// Use it where ever you want,//
// as long as you dont claim //
// the code to be your own!   //

// Requires:  
// - Glyph Keeper 0.26.1 or later (Compile with -DGLYPH_TARGET=GLYPH_ALLEGGL)
// - Allegro 4.0.2 or later
// - AllegroGL 0.2.4 or later
// - Recommended: Loadpng + LibPNG + ZLib to load png images
// - Reading the manual or header files to know the functions
// - Linking your programs with: 
//   -lglyph -lfreetype -lloadpng -lpng -lz -lagl -lalleg -luser32 -lgdi32 -lglu32 -lopengl32

//#define FORTIFY
//#include <Fortify/Fortify.h>


#ifndef OPENLAYER_HPP
#define OPENLAYER_HPP


// Defines set accordingly to compiled settings
#define OL_DRIVER ALLEGRO_GL
#ifndef OL_NO_TTF
#define OL_NO_TTF
#endif /* OL_NO_TTF */

#include "OpenLayer/GarbageCollector.hpp"
#include "OpenLayer/Includes.hpp"

#include "OpenLayer/Animation.hpp"
#include "OpenLayer/Bitmap.hpp"
#include "OpenLayer/Blenders.hpp"
#include "OpenLayer/FpsCounter.hpp"
#include "OpenLayer/GarbageCollector.hpp"
#include "OpenLayer/General.hpp"

#include "OpenLayer/Rgba.hpp"
#include "OpenLayer/Settings.hpp"
#include "OpenLayer/Setup.hpp"
#include "OpenLayer/TextRenderer.hpp"
#include "OpenLayer/Transforms.hpp"
#include "OpenLayer/Canvas.hpp"
#include "OpenLayer/RenderModes.hpp"

#include "OpenLayer/Shape.hpp"
#include "OpenLayer/TexturedPoly.hpp"
#include "OpenLayer/Polygon.hpp"
#include "OpenLayer/Line.hpp"
#include "OpenLayer/Rectangle.hpp"
#include "OpenLayer/Circle.hpp"
#include "OpenLayer/LineStrip.hpp"
#include "OpenLayer/Point.hpp"

#include "OpenLayer/Vec2D.hpp"
#include "OpenLayer/TextureInfo.hpp"

// Providing backwards compatibility with apps made before 2.0
#include "OpenLayer/GfxRend.hpp"

#endif

