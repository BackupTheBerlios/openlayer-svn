#ifdef USE_NEW_TTF

#ifndef OL_GLYPH_CPP
#define OL_GLYPH_CPP

#include "Glyph.hpp"

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
		size=0;
		currentFilename="";
		faceName="";
		currentChar = new character;
		workBitmap=NULL;
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
	
		FT_Load_Char(face, unicode, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT);
			
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
	void Glyph::createIndex(int i)
	{
		std::map<int, std::map<signed long, character> >::iterator p;
		p = fontTable.find(i);
		if(p==fontTable.end())
		{
			FT_Set_Pixel_Sizes(face, i, i);
			FT_UInt g;
			FT_ULong unicode = FT_Get_First_Char(face, &g);
			std::map<signed long, character>tempMap;
			while (g)
			{
				tempMap.insert(std::make_pair(unicode,extractGlyph(unicode)));
				unicode = FT_Get_Next_Char(face, unicode, &g);
			}
			fontTable.insert(std::make_pair(i,tempMap));
		}
	}
	
	// Render a character from the lookup table (utilizing the workBitmap)
	void Glyph::drawCharacter(signed long unicode, double &x1, double &y1, Bitmap *bitmap, Rgba col)
	{
		std::map<int, std::map<signed long, character> >::iterator ft;
		ft = fontTable.find(size);
		if(ft!=fontTable.end())
		{
			std::map<signed long, character>::iterator p;
			p = (ft->second).find(unicode);
			if(p!=(ft->second).end())
			{
				character tempChar = p->second;
				/*	
				if(workBitmap==NULL)
				{
					workBitmap = (resourceManager::getInstance())->resourceFactory->createGfx();
					systemName = (resourceManager::getInstance())->currentSystemName;
				}
				else if((resourceManager::getInstance())->currentSystemName!=systemName)
				{
					workBitmap = (resourceManager::getInstance())->resourceFactory->createGfx();
					systemName = (resourceManager::getInstance())->currentSystemName;
				}
					
				if((resourceManager::getInstance())->currentSystemName==systemName)
				{
					if(!workBitmap->isInitialized())workBitmap->createBmp(tempChar.width, tempChar.height);
					else if(workBitmap->getBmpWidth()<tempChar.width || workBitmap->getBmpHeight()<tempChar.height)
					{
						workBitmap->createBmp(tempChar.width, tempChar.height);
					}
				}*/
				//Rgba tempCol(255,0,255,255);
				//workBitmap->clearBmp(&tempCol);
				
				unsigned char *line = tempChar.line;
				for (int y = y1; y < tempChar.rows; y++)
				{
					unsigned char *buffer = line;
					for (int x = x1; x < tempChar.width; x++)
					{
						Rgba col = colorConvert(buffer++,tempChar.grays);
						Rgba tempCol(0,0,0,0);
						if(col.r != tempCol.r && col.g != tempCol.g && col.b != tempCol.b && col.a != tempCol.a)
						{
							//workBitmap->placePixel(x, y, color);
							ol::Point(float(x),float(y)).Draw( col );
						}
					}
					line += tempChar.pitch;
				}
					
				//workBitmap->renderBmpArea(0,0,x1,y1 - tempChar.top,tempChar.width,tempChar.height,bitmap);
				x1+=tempChar.right;
			}
		}
	}

	
	// Load font from memory
	bool Glyph::loadFromMemory(const unsigned char *memoryFont, unsigned int length, int index, unsigned int fontSize)
	{
		if(!FT_New_Memory_Face(ftLibrary,memoryFont, length,index,&face))
		{
			currentFilename = "memoryFont";
			currentIndex = index;
			faceLoaded = true;
			setSize(fontSize);
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
	bool Glyph::load(const std::string & filename, int index, unsigned int fontSize)
	{
		if(!FT_New_Face(ftLibrary,filename.c_str(),index,&face))
		{
			currentFilename = filename;
			currentIndex = index;
			faceLoaded = true;
			setSize(fontSize);
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
			for(unsigned int i = 0; i<fixedText.length();++i)
			{
				if(kerning)
				{
					next = FT_Get_Char_Index( face, fixedText[i] );
					FT_Vector delta;
					FT_Get_Kerning( face, previous, next, FT_KERNING_DEFAULT, &delta );
					rend_x += delta.x >> 6;
					previous = next;
				}
				drawCharacter(fixedText[i],rend_x, rend_y, bmp, col);
			}
		}
	}
	
	
	// Get text length
	double Glyph::getLength(const std::string & text)
	{
		double length=0;
		std::map<int, std::map<signed long, character> >::iterator ft;
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
	void Glyph::setSize(int s)
	{
		if(size<0 || size > 50)return;
		size=s;
		createIndex(size);
	}
	
	// Get Width
	int Glyph::getSize()
	{
		return size;
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
		
		temp->glyphFace = new Glyph();
		
		temp->glyphFace->load(filename,index);
		
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
		//rend->glyphFace->setItalics(italics);
	}
	
	void rend_set_size_pixels( GLYPH_REND* const rend, const unsigned height, const unsigned width)
	{
		//rend->glyphFace->setSize(height<<6, width<<6);
		rend->glyphFace->setSize(width<<6);
	}
	
	void rend_set_hinting_default( GLYPH_REND* const rend )
	{
		//rend->glyphFace->setHintingMode(FT_LOAD_DEFAULT);
	}
	
	void gk_rend_set_hinting_off( GLYPH_REND* const rend )
	{
		//rend->glyphFace->setHintingMode(FT_LOAD_NO_HINTING);
	}
	
	void rend_set_render_mode_normal( GLYPH_REND* const rend )
	{
		// What to do?
		
	}
	
	Rgba colorConvert(const unsigned char *c,short ext)
	{
		return Rgba(*c * 255 / (ext - 1), *c * 255 / (ext - 1),*c * 255 / (ext - 1),*c * 255 / (ext - 1));
	}
	
	void gk_rend_set_text_alpha_color( GLYPH_REND* const rend, const unsigned alpha_color)
	{
		//rend->glyphFace->color = colorConvert(alpha_color);
	}
	
	int gk_rend_ascender_pixels( GLYPH_REND* const rend )
	{
		return 0;//rend->glyphFace->ascenderPixels();
	}
	
	int rend_ascender_pixels( GLYPH_REND* const rend )
	{
		return 0;//rend->glyphFace->ascenderPixels();
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
		//texture->glyphFace->render(text,x,y);
		texture->glyphFace->render(x,y,Rgba::WHITE,NULL,0,text);
	}
	
	void gk_send_texture_to_gpu( GLYPH_TEXTURE *texture )
	{
		// Nothing to be done
	}

}

#endif /* OL_GLYPH_CPP */

#endif /* USE_NEW_TTF */
