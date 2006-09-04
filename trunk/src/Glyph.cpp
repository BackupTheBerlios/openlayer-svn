#ifdef USE_NEW_TTF

#ifndef OL_GLYPH_CPP
#define OL_GLYPH_CPP

#include "Glyph.hpp"
#include "GfxRend.hpp"

namespace ol
{
	// Static count of instances of fonts to track library
	static int instances=0;
	static FT_Library ftLibrary = NULL;

	// Character ctor
	character::character()
	{
	}
	character::~character()
	{
		//if(line)delete [] line;
	}
	// Constructor
	Glyph::Glyph()
	{
		//Load library
		if(ftLibrary==NULL)
		{
			FT_Init_FreeType(&ftLibrary);
			instances++;
		}
		ID = instances;
		faceLoaded = kerning = false;
		currentIndex=0;
		currentFilename="";
		faceName="";
		currentChar = new character;
		workBitmap=NULL;
		color = Rgba::WHITE;
		hintingFlag = FT_LOAD_FORCE_AUTOHINT;
		renderFlag = FT_LOAD_TARGET_NORMAL;
	}
	// Destructor
	/*! Nothing yet */
	Glyph::~Glyph()
	{
		FT_Done_Face(face);
		
		if(instances>0)instances--;
		if(instances==0)
		{
			FT_Done_FreeType(ftLibrary);
		}
		if(currentChar)delete currentChar;
	}
	
	// Comparison of IDs
	bool Glyph::operator==(Glyph *f)
	{
		if(ID==f->ID)return true;
		return false;
	}
	
				
	// Extract glyph
	character Glyph::extractGlyph(signed long unicode)
	{
		int w, h, ew;
		character tempChar;
		
		// Enable below whenever able to get angles to work properly
		//FT_Matrix a, matrix;
		FT_Matrix matrix;
		matrix.xx = 0x10000;
		matrix.xy = (FT_Fixed)(sin(size.italics)*(GLYPH_SQRT2*0x10000));
		matrix.yx = 0;
		matrix.yy = 0x10000;
		/*a.xx = (FT_Fixed)( cos(0)*0x10000);
		a.xy = (FT_Fixed)(-sin(0)*0x10000);
		a.yx = (FT_Fixed)( sin(0)*0x10000);
		a.yy = (FT_Fixed)( cos(0)*0x10000);
		FT_Matrix_Multiply(&a,&matrix);*/
		
		if(size.italics!=0)FT_Set_Transform( face, &matrix, 0 );
	
		FT_Load_Char(face, unicode, renderFlag | FT_LOAD_RENDER | hintingFlag);
		
		w = face->glyph->bitmap.width;
		h = face->glyph->bitmap.rows;
		ew = 0;
		if (!w)ew = 1;
		if (!h)h = 1;
			
		tempChar.width = (w + ew);
		tempChar.height = h;
			
		tempChar.rows = face->glyph->bitmap.rows;
		tempChar.grays = face->glyph->bitmap.num_grays;
		tempChar.pitch = face->glyph->bitmap.pitch;
		tempChar.line = new unsigned char[tempChar.rows * tempChar.pitch];
		memcpy(tempChar.line,face->glyph->bitmap.buffer,tempChar.rows * tempChar.pitch);
			
		tempChar.left = face->glyph->bitmap_left;
		tempChar.top = face->glyph->bitmap_top;
		tempChar.right = face->glyph->advance.x >> 6;
		tempChar.unicode = unicode;
			
		tempChar.length = (w + ew)+face->glyph->advance.x >> 6;
			
		return tempChar;
	}
	
	// Create single index
	void Glyph::createIndex()
	{
		std::map<dimension, std::map<signed long, character> >::iterator p;
		p = fontTable.find(size);
		if(p==fontTable.end())
		{
			FT_Set_Pixel_Sizes(face, size.width, size.height);
			
			FT_UInt g;
			FT_ULong unicode = FT_Get_First_Char(face, &g);
			std::map<signed long, character>tempMap;
			while (g)
			{
				tempMap.insert(std::make_pair(unicode,extractGlyph(unicode)));
				unicode = FT_Get_Next_Char(face, unicode, &g);
			}
			fontTable.insert(std::make_pair(size,tempMap));
		}
	}
	
	// Render a character from the lookup table (utilizing the workBitmap)
	void Glyph::drawCharacter(signed long unicode, double &x1, double &y1, Bitmap *bitmap, const Rgba & col)
	{
		std::map<dimension, std::map<signed long, character> >::iterator ft;
		ft = fontTable.find(size);
		if(ft!=fontTable.end())
		{
			std::map<signed long, character>::iterator p;
			p = (ft->second).find(unicode);
			if(p!=(ft->second).end())
			{
				character tempChar = p->second;
				
				unsigned char *line = tempChar.line;
				for (int y = (int)y1; y < (int)(y1)+tempChar.rows; y++)
				{
					unsigned char *buffer = line;
					for (int x = (int)x1; x < (int)(x1)+tempChar.width; x++)
					{
						Rgba checkCol = colorConvert(buffer++,tempChar.grays);
						if(checkCol.r==0 && checkCol.g==0 && checkCol.b==0 && checkCol.a==0)continue;
						double intensity = ( checkCol.r * 0.30 + checkCol.g * 0.59 + checkCol.b * 0.11);
						checkCol.r = col.r * intensity;
						checkCol.g = col.g * intensity;
						checkCol.b = col.b * intensity;
						checkCol.a = col.a * intensity;
						
						ol::Point(float(x + tempChar.left),float(y - tempChar.top)).Draw( checkCol );
					}
					line += tempChar.pitch;
				}
				x1+=tempChar.right;
			}
		}
	}

	
	// Load font from memory
	bool Glyph::loadFromMemory(const unsigned char *memoryFont, unsigned int length, int index, unsigned int width, unsigned int height)
	{
		if(!FT_New_Memory_Face(ftLibrary,memoryFont, length,index,&face))
		{
			currentFilename = "memoryFont";
			currentIndex = index;
			faceLoaded = true;
			setSize(width, height);
			if(FT_HAS_GLYPH_NAMES(face))
			{
				char buff[1024];
				if(!FT_Get_Glyph_Name(face,currentIndex,buff,sizeof(buff)))
				{
					faceName = currentFilename;
				}
				else faceName = std::string(buff);
			}
			else
			{
				faceName = currentFilename;
			}
			if(FT_HAS_KERNING(face))kerning=true;
			else kerning = false;
		}
		else 
		{
			faceLoaded=false;
			std::cout << "Load system font failed\n";
		}
					
		return faceLoaded;
	}
			
	// Load font from file
	bool Glyph::load(const std::string & filename, int index, unsigned int width, unsigned int height)
	{
		if(!FT_New_Face(ftLibrary,filename.c_str(),index,&face))
		{
			currentFilename = filename;
			currentIndex = index;
			faceLoaded = true;
			setSize(width, height);
			if(FT_HAS_GLYPH_NAMES(face))
			{
				char buff[1024];
				if(!FT_Get_Glyph_Name(face,currentIndex,buff,sizeof(buff)))
				{
					faceName = currentFilename;
				}
				else faceName = std::string(buff);
			}
			else
			{
				faceName = currentFilename;
			}
			if(FT_HAS_KERNING(face))kerning=true;
			else kerning = false;
		}
		else 
		{
			faceLoaded=false;
		}
		
		return faceLoaded;
	}
	
	void Glyph::render(double x, double y, Rgba col, Bitmap *bmp, int alignment, const std::string & text, ...)
	{
		if(faceLoaded)
		{
			double rend_x=0;
			double rend_y=0;
			std::ostringstream str;
					
					// Get extra arguments
			va_list ap;
			va_start(ap, text);
			for(unsigned int i = 0; i<text.length();++i)
			{
				if(text[i]=='%')
				{
					++i;
					if(text[i]=='s')
					{
						str << va_arg(ap,char *);
					}
					else if(text[i]=='d'||text[i]=='i')
					{
						str << va_arg(ap,signed int);
					}
					else if(text[i]=='c')
					{
						str << (char)va_arg(ap,int);
					}
				}
				else
				{
					str << text[i];
				}
			}
			va_end(ap);
					
			std::string fixedText(str.str());
			switch(alignment)
			{
				case 0:
					rend_x=x;
					rend_y=y;
					break;
				case 1:
					rend_x = x - getLength(fixedText)/2;
					rend_y=y;
					break;
				case 2:
					rend_x = x - getLength(fixedText);
					rend_y=y;
					break;
			}
			int previous = 0;
			int next = 0;
			
			// Get the point size and set it to the primary size in order to avoid messed up fonts
				
			GLfloat psGrab, psizes[2];
			glGetFloatv(GL_POINT_SIZE_RANGE, psizes);
			glGetFloatv(GL_POINT_SIZE, &psGrab);
			glPointSize(psizes[0]);
			
			// Save the old transform state
			ol::Transforms::PushPlacement();
				
			// Also set the projection mode and switch back if necessary
			ol::Settings::SetOrthographicProjection();
			
			for(unsigned int i = 0; i<fixedText.length();++i)
			{
				if(kerning && previous && next)
				{
					next = FT_Get_Char_Index( face, fixedText[i] );
					FT_Vector delta;
					FT_Get_Kerning( face, previous, next, FT_KERNING_DEFAULT, &delta );
					rend_x += delta.x >> 6;
					previous = next;
				}
				drawCharacter(fixedText[i],rend_x, rend_y, bmp, col);
			}
			
			// Set previous pointsize
			glPointSize(psGrab);
			
			// Restore last mode
			ol::Settings::RestoreOldProjection();
			
			// Restore transform
			ol::Transforms::PopPlacement();
			
		}
	}
	
	
	// Get text length
	double Glyph::getLength(const std::string & text)
	{
		double length=0;
		std::map<dimension, std::map<signed long, character> >::iterator ft;
		ft = fontTable.find(size);
		if(ft!=fontTable.end())
		{
			for(unsigned int i = 0; i<text.length();++i)
			{
				std::map<signed long, character>::iterator p;
				p = (ft->second).find(text[i]);
				if(p!=(ft->second).end())
				{
					if(p!=fontTable[size].end())
					{
						length+=(p->second).length;
					}
				}
			}
		}
		return length;
	}
	
	// Set size
	void Glyph::setSize(int w, int h)
	{
		size.width=w;
		size.height=h;
		createIndex();
	}
	
	// Set italics
	void Glyph::setItalics(int i)
	{
		if(i < -45)i=-45;
		else if(i > 45)i=45;
		
		size.italics = (double)(i)*GLYPH_PI/180;
		createIndex();
	}
	
	// Set FreeType LoadFlags
	void Glyph::setHinting(bool on)
	{
		if(on)
		{
			hintingFlag = FT_LOAD_FORCE_AUTOHINT;
		}
		else
		{
			hintingFlag = FT_LOAD_NO_HINTING;
		}
		fontTable.clear();
		setSize(size.width, size.height);
	}
	
	// Set FreeType LoadFlags
	void Glyph::setAntialias(bool on)
	{
		if(on)
		{
			renderFlag = FT_LOAD_TARGET_NORMAL;
		}
		else
		{
			renderFlag = FT_LOAD_TARGET_MONO;
		}
		fontTable.clear();
		setSize(size.width, size.height);
	}
	
	// Get Width
	int Glyph::getWidth()
	{
		return size.width;
	}
	
	// Get Height
	int Glyph::getHeight()
	{
		return size.height;
	}
	
	// Get Italics
	int Glyph::getItalics()
	{
		return (int)size.italics;
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
		if(glyphFace)delete glyphFace;	
	}
	
	GLYPH_FACE *load_face_from_file(const char *filename, int index)
	{
		GLYPH_FACE *temp = new GLYPH_FACE();
		
		temp->glyphFace = new Glyph();
		
		if(!temp->glyphFace->load(filename,index))return 0;
		
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
	
	void rend_set_size_pixels( GLYPH_REND* const rend, const unsigned int width, const unsigned int height)
	{
		rend->glyphFace->setSize(width, height);
	}
	
	void rend_set_hinting_default( GLYPH_REND* const rend )
	{
		rend->glyphFace->setHinting(true);
	}
	
	void gk_rend_set_hinting_off( GLYPH_REND* const rend )
	{
		rend->glyphFace->setHinting(false);
	}
	
	void rend_set_render_mode_normal( GLYPH_REND* const rend )
	{
		rend->glyphFace->setAntialias(true);
	}
	
	Rgba colorConvert(const unsigned char *c,short ext)
	{
		return Rgba(*c * 255 / (ext - 1), *c * 255 / (ext - 1),*c * 255 / (ext - 1),*c * 255 / (ext - 1));
	}
	
	Rgba colorConvert(const unsigned int c)
	{
		return Rgba((float)(((c >> 16) & 0xff )/255.0),(float)(((c >> 8) & 0xff )/255.0),(float)(((c) & 0xff )/255.0),(float)(((c >> 24) & 0xff )/255.0));
	}
	
	void gk_rend_set_text_alpha_color( GLYPH_REND* const rend, const unsigned alpha_color)
	{
		rend->glyphFace->color = colorConvert(alpha_color);
	}
	
	int gk_rend_ascender_pixels( GLYPH_REND* const rend )
	{
		return rend->glyphFace->getHeight();
	}
	
	int rend_ascender_pixels( GLYPH_REND* const rend )
	{
		return rend->glyphFace->getHeight();
	}
	
	int text_width_utf8(GLYPH_REND* const rend,const char* const text)
	{
		return (int)rend->glyphFace->getLength(text);
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
		texture->glyphFace->render(x,y,texture->glyphFace->color,NULL,0,text);
	}
	
	void gk_send_texture_to_gpu( GLYPH_TEXTURE *texture )
	{
		// Nothing to be done
	}

	}

#endif /* OL_GLYPH_CPP */

#endif /* USE_NEW_TTF */
