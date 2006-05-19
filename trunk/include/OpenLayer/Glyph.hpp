#ifdef USE_NEW_TTF

#ifndef OL_GLYPH_HPP
#define OL_GLYPH_HPP

#include <math.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_INTERNAL_OBJECTS_H

#define GLYPH_PI	3.14159265358979323846
#define GLYPH_PI2	GLYPH_PI * 2
#define GLYPH_SQRT2	1.41421356237309504880

namespace ol
{
	
	// Forward decleration of Glyph for libFreeType
	class Glyph;
	
	// A singleton of the freetype library to use with existing glyphs
	class libFreeType
	{
		private:
			// Used to setup the FreeType library
			static FT_Library libInstance;
			static int totalClients;
			
			// Allow Glyph to access the library
			friend class Glyph;
		protected:
			libFreeType();
			~libFreeType();
	};
	
	
	// This class handles face objects
	class Glyph
	{
		private:
			// FT_Library
			libFreeType library;
			
			// Current filename
			char *currentFilename;
			
			// Face
			FT_Face fontFace;
			bool faceLoaded;
			
			// Slot
			FT_GlyphSlot	slot;
			FT_Matrix	matrix;
			FT_Size		size;
			FT_UInt		currentIndex;
			FT_Vector	coord;
			
			// Font specifics
			
			// Italics
			bool 		italicized;
			double 		italics;
			
			// Angle
			bool 		angled;
			double 		angle;
			
			bool 		kerning;
			bool 		monoSpacing;
			
			unsigned	hsize,vsize;
			unsigned	lineSpacing;
			unsigned	lineSpacingPixels;
			unsigned	textHeight;
			unsigned	textHeightPixels;
			unsigned	hintingMode;
			unsigned	hintingTarget;
			unsigned	loadFlags;
			
		protected:
			
			// Translate the face on its matrix
			void updateMatrix();
			int ascender();
			int ascenderPixels();
			int descender();
			int descenderPixels();
			
		public:
			// Constructors
			Glyph();
			Glyph(char const *filename,int faceIndex = 0);
			
			// Destructor
			~Glyph();
			// Face handling
			
			// Loads a new face into Glyph
			bool load(char const *filename, int faceIndex);
			
			// This returns how many font faces avialable
			long totalFaces();
			
			// Set font face index return true if successfull
			bool setFaceIndex(int index);
			
			// Set width and height in pixels return true if successfull
			bool setFontSize(int width=0, int height=0);
			
			// Italicized check
			bool isItalicized();
			
			// Angled check
			bool isAngled();
			
			void setItalics(int i);
			
			void setAngle(double a);
			
			bool setPixelSize(const unsigned height, const unsigned width);
			
			void setHintingMode(const unsigned mode);
			
			// Variables
	};
	
	/*
	*	These items are so that we can maintain compatibility with Glyph Keeper
	*	until TextRender adopts the new system and replaces references to
	*	GlyphKeeper in the future.
	*/
	
	class GLYPH_FACE
	{
		public:
			GLYPH_FACE();
			~GLYPH_FACE();
			
			Glyph *glyphFace;
			
			friend GLYPH_FACE *load_face_from_file(const char *filename, int index);
	};
	
	class GLYPH_REND
	{
		public:
			GLYPH_REND();
			~GLYPH_REND();
			
			Glyph *glyphFace;
			
			friend GLYPH_REND *create_renderer( GLYPH_FACE* const face, int index );
			friend void rend_set_italic( GLYPH_REND* const rend, int italics );
			friend void rend_set_size_pixels( GLYPH_REND* const rend, const unsigned height, const unsigned width);
			friend void rend_set_hinting_default( GLYPH_REND* const rend );
			friend void gk_rend_set_hinting_off( GLYPH_REND* const rend );
			friend void rend_set_render_mode_normal( GLYPH_REND* const rend );
			friend void gk_rend_set_text_alpha_color( GLYPH_REND* const rend, const unsigned alpha_color);
			friend int gk_rend_ascender_pixels( GLYPH_REND* const rend );
			friend int rend_ascender_pixels( GLYPH_REND* const rend );
			friend int text_width_utf8(GLYPH_REND* const rend,const char* const text);
	};
	
	class GLYPH_TEXTURE
	{
		public:
			GLYPH_TEXTURE();
			~GLYPH_TEXTURE();
			
			Glyph *glyphFace;
			
			friend GLYPH_TEXTURE *gk_create_texture( GLYPH_REND *rend, int rangeStart, int rangeLength );
			friend void gk_unload_texture_from_gpu( GLYPH_TEXTURE *texture );
			friend void gk_destroy_texture( GLYPH_TEXTURE *texture );
			friend void gk_render_line_gl_utf8( GLYPH_TEXTURE *texture, const char *text, int x, int y );
			friend void gk_send_texture_to_gpu( GLYPH_TEXTURE *texture );
	};

}

#endif /* OL_GLYPH_HPP */

#endif /* USE_NEW_TTF */
