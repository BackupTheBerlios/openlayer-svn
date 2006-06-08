#ifdef USE_NEW_TTF

#ifndef OL_GLYPH_HPP
#define OL_GLYPH_HPP

#include <math.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_INTERNAL_OBJECTS_H
#include "Includes.hpp"
#include "Rgba.hpp"
#include "Bitmap.hpp"
#include "Internal.hpp"

#define GLYPH_PI	3.14159265358979323846
#define GLYPH_SQRT2	1.41421356237309504880

namespace ol
{
	
	// Forward decleration of Glyph for libFreeType
	class Glyph;
	
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
	};
	
	class GLYPH_REND
	{
		public:
			GLYPH_REND();
			~GLYPH_REND();
			
			Glyph *glyphFace;
				
	};
	
	class GLYPH_TEXTURE
	{
		public:
			GLYPH_TEXTURE();
			~GLYPH_TEXTURE();
			
			Glyph *glyphFace;
			
	};
	
	GLYPH_FACE *load_face_from_file(const char *filename, int index);
	GLYPH_REND *create_renderer( GLYPH_FACE* const face, int index );
	void rend_set_italic( GLYPH_REND* const rend, int italics );
	void rend_set_size_pixels( GLYPH_REND* const rend, const unsigned height, const unsigned width);
	void rend_set_hinting_default( GLYPH_REND* const rend );
	void gk_rend_set_hinting_off( GLYPH_REND* const rend );
	void rend_set_render_mode_normal( GLYPH_REND* const rend );
	Rgba colorConvert(const unsigned col);
	void gk_rend_set_text_alpha_color( GLYPH_REND* const rend, const unsigned alpha_color);
	int gk_rend_ascender_pixels( GLYPH_REND* const rend );
	int rend_ascender_pixels( GLYPH_REND* const rend );
	int text_width_utf8(GLYPH_REND* const rend,const char* const text);
	GLYPH_TEXTURE *gk_create_texture( GLYPH_REND *rend, int rangeStart, int rangeLength );
	void gk_unload_texture_from_gpu( GLYPH_TEXTURE *texture );
	void gk_destroy_texture( GLYPH_TEXTURE *texture );
	void gk_render_line_gl_utf8( GLYPH_TEXTURE *texture, const char *text, int x, int y );
	void gk_send_texture_to_gpu( GLYPH_TEXTURE *texture );
	
	// This class handles face objects
	class Glyph
	{
		private:
			// FT_Library
			FT_Library library;
			
			// Current filename
			std::string currentFilename;
			
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
			
			int		hsize,vsize;
			int		lineSpacing;
			int		lineSpacingPixels;
			int		textHeight;
			int		textHeightPixels;
			int		hintingMode;
			int		hintingTarget;
			int		loadFlags;
			
			/*
			*	These items are so that we can maintain compatibility with Glyph Keeper
			*	until TextRender adopts the new system and replaces references to
			*	GlyphKeeper in the future.
			*/
			
			friend class GLYPH_FACE;
			friend class GLYPH_REND;
			friend class GLYPH_TEXTURE;
			
		protected:
			
			// Translate the face on its matrix
			void updateMatrix();
			Bitmap getCharBitmap(FT_Face face, FT_ULong unicode);
			
		public:
			// Constructors
			Glyph();
			Glyph(const std::string filename,int faceIndex = 0);
			
			// Destructor
			~Glyph();
			// Face handling
			
			// Loads a new face into Glyph
			bool load(const std::string filename, int faceIndex);
			
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
			
			int ascender();
			
			int ascenderPixels();
			
			int descender();
			
			int descenderPixels();
			
			int textWidthUTF8(const std::string text);
			
			void render(std::string text, float x, float y);
			
			// Variables
			Rgba color;
	};

}

#endif /* OL_GLYPH_HPP */

#endif /* USE_NEW_TTF */
