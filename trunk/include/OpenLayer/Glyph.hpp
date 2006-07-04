#ifdef USE_NEW_TTF

#ifndef OL_GLYPH_HPP
#define OL_GLYPH_HPP

#include <math.h>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_INTERNAL_OBJECTS_H
#include "Rgba.hpp"
#include "Bitmap.hpp"
#include "Point.hpp"

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
	Rgba colorConvert(const unsigned char *c,short ext);
	void gk_rend_set_text_alpha_color( GLYPH_REND* const rend, const unsigned alpha_color);
	int gk_rend_ascender_pixels( GLYPH_REND* const rend );
	int rend_ascender_pixels( GLYPH_REND* const rend );
	int text_width_utf8(GLYPH_REND* const rend,const char* const text);
	GLYPH_TEXTURE *gk_create_texture( GLYPH_REND *rend, int rangeStart, int rangeLength );
	void gk_unload_texture_from_gpu( GLYPH_TEXTURE *texture );
	void gk_destroy_texture( GLYPH_TEXTURE *texture );
	void gk_render_line_gl_utf8( GLYPH_TEXTURE *texture, const char *text, int x, int y );
	void gk_send_texture_to_gpu( GLYPH_TEXTURE *texture );
	
	class character
	{
		public:
			//! Constructor
			character();
			
			//! Destructor
			~character();
			
			//! Unicode representation of character
			signed long unicode;
			
			//! Width of character
			int width;
			
			//! Height of character
			int height;
			
			//! Space on the left of a character (assists on positioning the character)
			int left;
			
			//! Space on top of the character (assists on positioning the character)
			int top;
			
			//! Space on the right of a character (assists on positioning the character)
			int right;
			
			//! Pitch of a character (assists on positioning the character)
			int pitch;
			
			//! Amount of shades of grays the FT_Bitmap holds
			int grays;
			
			//! Entire rows of the FT_Bitmap
			int rows;
			
			//! Entire length of the character with spacing and all
			int length;
			
			//! FT_Bitmap raw data
			unsigned char *line;
	};
	
	// This class handles face objects
	class Glyph
	{
		private:
			//! ID
			int ID;
			
			//! Comparison of IDs
			bool operator==(Glyph *g);

			//! Current file
			std::string currentFilename;
			
			//! Is the face loaded
			bool faceLoaded;
			
			//! Does the face have kerning
			bool kerning;
			
			//! Current index default 0
			int currentIndex;
			
			//! Font size
			FT_UInt size;
			
			//! Workspace bitmap
			Bitmap *workBitmap;
			
			//! Face
			FT_Face face;
			
			//! Face Name
			std::string faceName;
			
			//! Current character
			character *currentChar;
			
			//! Lookup Table by size
			std::map<int, std::map<signed long, character> >fontTable;
			
			//! Extract glyph
			character extractGlyph(signed long unicode);
			
			//! Create single index
			void createIndex(int i);
			
			//! Render a character from the lookup table (utilizing the workBitmap)
			void drawCharacter(signed long unicode, double &x1, double &y1, Bitmap *bitmap, Rgba col);
			
			friend class GLYPH_FACE;
			friend class GLYPH_REND;
			friend class GLYPH_TEXTURE;
		public:
			//! Constructor
			Glyph();
			
			//! Destructor
			~Glyph();
			
			//! Load font from memory
			bool loadFromMemory(const unsigned char *memoryFont, unsigned int length, int index=0, unsigned int fontSize=14);
			
			//! Load font from file
			bool load(const std::string & filename, int index=0, unsigned int fontSize=14);
			
			//! Get text length
			double getLength(const std::string & text);
			
			//! Render font to a bitmap
			void render(double x, double y, Rgba col, Bitmap *bmp, int alignment, const std::string & text, ...);
			
			//! Set size
			void setSize(int s);
			
			//! Get size
			int getSize();
			
			//! Color
			Rgba color;
			
			//! Enumerator for positioning of text when rendering
			enum
			{
				LEFT=0,
				CENTERED,
				RIGHT
			};
	};

}

#endif /* OL_GLYPH_HPP */

#endif /* USE_NEW_TTF */
