#ifdef USE_NEW_TTF

#ifndef OL_GLYPH_CPP
#define OL_GLYPH_CPP

#include "Glyph.hpp"

namespace ol
{
	// Face handler
	
	Glyph::Glyph()
	{
		// Load up the library
		FT_Init_FreeType(&library);
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
	
	Glyph::Glyph(const std::string filename,int faceIndex)
	{
		// Load up the library
		FT_Init_FreeType(&library);
		int error = FT_New_Face(library,filename.c_str(),faceIndex,&fontFace);
		if(!error)
		{
			currentFilename = filename;
			currentIndex = faceIndex;
			faceLoaded = true;
			OlLog( "Glyph::fontFace loaded properly" );
		}
		else 
		{
			faceLoaded=false;
			char buf[20];
			sprintf(buf, "%d", error);
			OlLog( std::string("Glyph::fontFace is " + filename + " is bad! Could not load. Error: " + buf));
		}
				
		italicized = false;
		angled = false;
		kerning = false;
		monoSpacing = false;
		
		italics = 0;
		angle = 0;
	}
	
	Glyph::~Glyph()
	{
		// Destroy the library
		FT_Done_FreeType(library);
	}
	
	// Loads a new face
	bool Glyph::load(const std::string filename, int faceIndex)
	{
		if(faceLoaded)
		{
			FT_Face tmpFace;
			if(FT_New_Face(library,filename.c_str(),faceIndex,&tmpFace)==0)
			{
				currentFilename = filename;
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
			if(FT_New_Face(library,filename.c_str(),faceIndex,&fontFace)==0)
			{
				currentFilename = filename;
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
			if(FT_New_Face(library,currentFilename.c_str(),index,&tmpFace)==0)
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
	
	Bitmap Glyph::getCharBitmap(FT_Face face, FT_ULong unicode)
	{
		int w, h, ew;
		BITMAP *bmp;
		int x, y;
		unsigned char *line;
		
		FT_Load_Char(face, unicode, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT);

		w = face->glyph->bitmap.width;
		h = face->glyph->bitmap.rows;
		ew = 0;

		if (!w)
			ew = 1;

		if (!h)
			h = 1;

		bmp = create_bitmap_ex(8, w + ew, h);
		clear_to_color(bmp, bitmap_mask_color(bmp));

		line = face->glyph->bitmap.buffer;
		for (y = 0; y < face->glyph->bitmap.rows; y++)
		{
			unsigned char *buffer = line;
			for (x = 0; x < face->glyph->bitmap.width; x++)
			{
				putpixel(bmp, x, y, *buffer++);
			}
			line += face->glyph->bitmap.pitch;
		}
		
		return Bitmap(bmp);
		
		/*
		class loader {
			private:
				unsigned char *img;
   
			public:
				loader( FT_Bitmap bmp) {
					img = bmp.buffer;
					width = bmp.width;
					height = bmp.rows;
				}
   
				Rgba operator() ( int x, int y ) {
     					// The image is guranteed to be loaded top-to-bottom, left-to-right
					return Rgba( (int)gets( (char *)img ), (int)gets( (char *)img ), (int)gets( (char *)img ), (int)gets( (char *)img ));
				}
   
				int width, height;
		};
		
		loader ld(face->glyph->bitmap);
		
		return Bitmap(ld.width,ld.height,ld);*/
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
		angle = ( a / 360 ) * GLYPH_PI * 2; 
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
	
	int Glyph::textWidthUTF8(const std::string text)
	{
		return 0;
	}
	
	void Glyph::render(std::string text, float x, float y)
	{
		float next_x = x;
		for(unsigned int i=0;i<text.length();++i)
		{
			Bitmap temp = getCharBitmap(fontFace,text[i]);
			temp.Blit(next_x,y);
			next_x+=temp.Width();
		}
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
		// What to do?
		
	}
	
	Rgba colorConvert(const unsigned col)
	{
		return Rgba(((float)(( col >> 16 ) & 0xff))/255.0, ((float)(( col >> 8 ) & 0xff))/255.0,
			      ((float)( col & 0xff))/255.0, ((float)(( col >> 24 ) & 0xff))/255.0);
	}
	
	void gk_rend_set_text_alpha_color( GLYPH_REND* const rend, const unsigned alpha_color)
	{
		rend->glyphFace->color = colorConvert(alpha_color);
	}
	
	int gk_rend_ascender_pixels( GLYPH_REND* const rend )
	{
		return rend->glyphFace->ascenderPixels();
	}
	
	int rend_ascender_pixels( GLYPH_REND* const rend )
	{
		return rend->glyphFace->ascenderPixels();
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
		// Nothing
	}
	
	void gk_destroy_texture( GLYPH_TEXTURE *texture )
	{
		// De-Init the texture ? nothing yet
	}		
	
	void gk_render_line_gl_utf8( GLYPH_TEXTURE *texture, const char *text, int x, int y )
	{
		texture->glyphFace->render(text,x,y);
	}
	
	void gk_send_texture_to_gpu( GLYPH_TEXTURE *texture )
	{
		// Nothing to be done
	}

}

#endif /* OL_GLYPH_CPP */

#endif /* USE_NEW_TTF */
