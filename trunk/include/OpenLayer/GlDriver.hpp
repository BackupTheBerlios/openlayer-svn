#ifndef GL_DRIVER_HPP
#define GL_DRIVER_HPP

#include "Includes.hpp"
#include "Internal.hpp"
#include "Vec2D.hpp"
#include "Setup.hpp"

#include <string>


// The OpenGL driver routines //


namespace ol {


class OlTextureInfo;


class GlDriver {
public:
   virtual bool SetupProgram( bool setupKeyboard, bool setupMouse, bool setupTimer ) = 0;
   virtual bool SetupScreen( int width, int height, bool fullscreen,
                             int colorDepth, int zDepth, int refreshRate ) = 0;
   
   virtual OlTextureInfo UploadTexture( OL_MEMORY_IMG *bmp, bool isSolid ) = 0;
   virtual OlTextureInfo UploadTexture( float *data, OlTextureInfo textureInfo ) = 0;
   
   virtual OL_MEMORY_IMG *GetMemoryBitmap( int width, int height, int bpp, unsigned char *pixelData ) = 0;
   virtual int GetImageColorDepth( OL_MEMORY_IMG *bmp ) = 0;
   virtual bool SaveMemoryBitmap( OL_MEMORY_IMG *bmp, std::string filename ) = 0;
   virtual void DestroyMemoryBitmap( OL_MEMORY_IMG *bmp ) = 0;
   
   virtual std::string GetExecutablePath() = 0;
   virtual std::string ToAbsolutePathname( std::string pathname ) = 0;
   
   static void Set( GlDriver *driver );
   static GlDriver *Get();
   
   static bool IsExtensionAlvailable( std::string extensionName );
   
   //static glEnum GetBytesPerPixel( glEnum textureFormat, int colorDepth = Setup::GetColorDepth() );
   
protected:
   
   const static std::string TEXTURE_NOT_POWER_OF_TWO_EXT;
   
private:
   static GlDriver *activeDriver;
};



class AllegroGLDriver : public GlDriver {
public:
   virtual bool SetupProgram( bool setupKeyboard, bool setupMouse, bool setupTimer );
   virtual bool SetupScreen( int width, int height, bool fullscreen,
                             int colorDepth, int zDepth, int refreshRate );
   virtual OlTextureInfo UploadTexture( OL_MEMORY_IMG *bmp, bool isSolid );
   virtual OlTextureInfo UploadTexture( GLfloat *data, OlTextureInfo textureInfo );
   
   virtual OL_MEMORY_IMG *GetMemoryBitmap( int width, int height, int bpp, unsigned char *pixelData );
   virtual int GetImageColorDepth( OL_MEMORY_IMG *bmp );
   virtual bool SaveMemoryBitmap( OL_MEMORY_IMG *bmp, std::string filename );
   virtual void DestroyMemoryBitmap( OL_MEMORY_IMG *bmp );
   
   virtual std::string GetExecutablePath();
   virtual std::string ToAbsolutePathname( std::string pathname );
};



}

#endif
