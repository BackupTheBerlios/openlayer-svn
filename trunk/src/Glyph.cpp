#ifdef USE_NEW_TTF

#ifndef OL_GLYPH_CPP
#define OL_GLYPH_CPP

#include "Glyph.hpp"

namespace ol
{

	// FreeType Library
	
	int libFreeType::totalClients = 0;
	
	libFreeType::libFreeType()
	{
		if(totalClients==0)
		{
			FT_Init_FreeType(&libInstance);
			++totalClients;
		}
		else ++totalClients;
	}
	
	libFreeType::~libFreeType()
	{
		if(totalClients>1)--totalClients;
		else
		{
			FT_Done_FreeType(libInstance);
			--totalClients;
		}
	}
	
	
	// Face handler
	
	Glyph::Glyph() : library()
	{
		faceLoaded = false;
		italicized = false;
		angled = false;
		kerning = false;
		monoSpacing = false;
		
		currentFilename = "";
		currentIndex = 0;
		italics = 0;
		angle = 0;
	}
	
	Glyph::Glyph(char const *filename,int faceIndex) : library()
	{
		if(FT_New_Face(library.libInstance,filename,faceIndex,&fontFace)==0)
		{
			strcpy(currentFilename, filename);
			currentIndex = faceIndex;
			faceLoaded = true;
		}
		else faceLoaded=false;
				
		italicized = false;
		angled = false;
		kerning = false;
		monoSpacing = false;
		
		italics = 0;
		angle = 0;
	}
	
	Glyph::~Glyph()
	{
	}
	
	// Loads a new face
	bool Glyph::load(char const *filename, int faceIndex)
	{
		if(faceLoaded)
		{
			FT_Face tmpFace;
			if(FT_New_Face(library.libInstance,filename,faceIndex,&tmpFace)==0)
			{
				strcpy(currentFilename, filename);
				currentIndex = faceIndex;
				FT_Done_Face(fontFace);
				//fontFace = tmpFace;
				memcpy(fontFace,tmpFace,sizeof &tmpFace);
				return true;
			}
			else
			{
				FT_Done_Face(tmpFace);
				return false;
			}
		}
		else 
		{
			if(FT_New_Face(library.libInstance,filename,faceIndex,&fontFace)==0)
			{
				strcpy(currentFilename, filename);
				currentIndex = faceIndex;
				faceLoaded = true;
				return true;
			}
			else
			{
				faceLoaded=false;
				return false;
			}
		}
		
		return false;
	}
	
	
	// This returns how many font faces avialable
	long Glyph::totalFaces()
	{
		if(faceLoaded)
		{
			return fontFace->num_glyphs;
		}
		else return 0;
	}
	
	// Set font face index return true if successfull
	bool Glyph::setFaceIndex(int index)
	{
		if(faceLoaded)
		{
			FT_Face tmpFace;
			if(FT_New_Face(library.libInstance,currentFilename,index,&tmpFace)==0)
			{
				currentIndex =  index;
				FT_Done_Face(fontFace);
				//fontFace = tmpFace;
				memcpy(fontFace,tmpFace,sizeof &tmpFace);
				return true;
			}
			else
			{
				FT_Done_Face(tmpFace);
				return false;
			}
		}
		else return false;
	}
	
	// Set width and height in pixels return true if successfull
	bool Glyph::setFontSize(int width, int height)
	{
		if(faceLoaded)
		{
			if(FT_Set_Char_Size( fontFace, width, height,100, 0 )==0)
			{
				return true;
			}
			else return false;
		}
		
		return false;
	}
	
	void Glyph::updateMatrix()
	{
		if(italics!=0)italicized=true;
		else italicized=false;
		if(angle!=0)angled=true;
		else angled=false;
		
		if(italicized || angled)
		{
			FT_Matrix temp;
			matrix.xx = 0x10000;
			matrix.xy = (FT_Fixed)(sin(italics)*(GLYPH_SQRT2*0x10000));
			matrix.yx = 0;
			matrix.yy = 0x10000;
			temp.xx = (FT_Fixed)( cos(angle)*0x10000);
			temp.xy = (FT_Fixed)(-sin(angle)*0x10000);
			temp.yx = (FT_Fixed)( sin(angle)*0x10000);
			temp.yy = (FT_Fixed)( cos(angle)*0x10000);
			FT_Matrix_Multiply(&temp,&matrix);
		}
	}
	
	int Glyph::ascender()
	{
		return (size) ? size->metrics.ascender : 0;
	}

	int Glyph::ascenderPixels()
	{
		return (size) ? (size->metrics.ascender+63)>>6 : 0;
	}

	int Glyph::descender()
	{
		return (size) ? size->metrics.descender : 0;
	}

	int Glyph::descenderPixels()
	{
		return (size) ? (size->metrics.descender+63)>>6 : 0;
	}
	
	// Italicized check
	bool Glyph::isItalicized()
	{
		return italicized;
	}
	
	// Angled check
	bool Glyph::isAngled()
	{
		return angled;
	}
	
	void Glyph::setItalics(int i)
	{
		if (i < -45 || i > 45) return;
		italics = (double)(i)*GLYPH_PI/180;
		updateMatrix();
	}
	
	void Glyph::setAngle(double a)
	{
		
	}
	
	bool Glyph::setPixelSize(const unsigned height, const unsigned width)
	{
		if (fontFace || size) return false;
		FT_Activate_Size(size);
		if (FT_Set_Char_Size(fontFace,height,width,72,72))
		{
			/* Send error to logs
			"Error: setPixelSize(): Can't set font size to %.3fx%.3f pixels\n",((double)height)/64,((double)width)/64);*/
			return false;
		}
		hsize = height;
		vsize = width;
		lineSpacing = fontFace->size->metrics.height;
		lineSpacingPixels = (lineSpacing+63)>>6;
		textHeight = ascender() - descender();
		textHeightPixels = ((ascender()+63)>>6) - ((descender()+63)>>6);

		return true;
	}
	
	// Set hinting mode
	void Glyph::setHintingMode(const unsigned mode)
	{
		hintingMode =  mode;
		loadFlags = FT_LOAD_NO_BITMAP | hintingMode | hintingTarget;
	}
	
	
	/*
	*	These items are so that we can maintain compatibility with Glyph Keeper
	*	until TextRender adopts the new system and replaces references to
	*	GlyphKeeper in the future.
	*/
	
	GLYPH_FACE::GLYPH_FACE()
	{
		// Nothing to do
	}
	
	GLYPH_FACE::~GLYPH_FACE()
	{
		// Nothing to do	
	}
	
	GLYPH_FACE *load_face_from_file(const char *filename, int index)
	{
		GLYPH_FACE *temp = new GLYPH_FACE();
		
		temp->glyphFace = new Glyph(filename, index);
		
		return temp;
	}
	
	GLYPH_REND::GLYPH_REND()
	{
		// Nothing to do
	}
	
	GLYPH_REND::~GLYPH_REND()
	{
		// Nothing to do	
	}
	
	GLYPH_REND *create_renderer( GLYPH_FACE* const face, int index )
	{
		GLYPH_REND *temp = new GLYPH_REND();
		
		temp->glyphFace = face->glyphFace;
		
		return temp;
	}
	
	void rend_set_italic( GLYPH_REND* const rend, int italics )
	{
		rend->glyphFace->setItalics(italics);
	}
	
	void rend_set_size_pixels( GLYPH_REND* const rend, const unsigned height, const unsigned width)
	{
		rend->glyphFace->setPixelSize(height<<6, width<<6);
	}
	
	void rend_set_hinting_default( GLYPH_REND* const rend )
	{
		rend->glyphFace->setHintingMode(FT_LOAD_DEFAULT);
	}
	
	void gk_rend_set_hinting_off( GLYPH_REND* const rend )
	{
		rend->glyphFace->setHintingMode(FT_LOAD_NO_HINTING);
	}
	
	void rend_set_render_mode_normal( GLYPH_REND* const rend )
	{
	
	}
	
	void gk_rend_set_text_alpha_color( GLYPH_REND* const rend, const unsigned alpha_color)
	{
		
	}
	
	int gk_rend_ascender_pixels( GLYPH_REND* const rend )
	{
		return 0;
	}
	
	int rend_ascender_pixels( GLYPH_REND* const rend )
	{
		return 0;
	}
	
	int text_width_utf8(GLYPH_REND* const rend,const char* const text)
	{
		return 0;
	}
	
	GLYPH_TEXTURE::GLYPH_TEXTURE()
	{
		// Nothing to do
	}
	
	GLYPH_TEXTURE::~GLYPH_TEXTURE()
	{
		// Nothing to do	
	}
	
	GLYPH_TEXTURE *gk_create_texture( GLYPH_REND *rend, int rangeStart, int rangeLength )
	{
		GLYPH_TEXTURE *temp = new GLYPH_TEXTURE();
		
		temp->glyphFace = rend->glyphFace;
		
		return temp;
	}
	
	void gk_unload_texture_from_gpu( GLYPH_TEXTURE *texture )
	{
	
	}
	
	void gk_destroy_texture( GLYPH_TEXTURE *texture )
	{
	
	}		
	
	void gk_render_line_gl_utf8( GLYPH_TEXTURE *texture, const char *text, int x, int y )
	{
	
	}
	
	void gk_send_texture_to_gpu( GLYPH_TEXTURE *texture )
	{
	
	}

}

#endif /* OL_GLYPH_CPP */

#endif /* USE_NEW_TTF */
