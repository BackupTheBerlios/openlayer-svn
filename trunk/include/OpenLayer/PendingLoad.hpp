#ifndef PENDING_LOADING_HPP
#define PENDING_LOADING_HPP


#include "Includes.hpp"
#include "GarbageCollector.hpp"
#include "TextureInfo.hpp"


namespace ol {


class Bitmap;


class PendingBitmapLoad {
public:
   virtual OlLoadResult ExecuteLoading( Bitmap &bmp ) = 0;
};



class PendingFileLoad : public PendingBitmapLoad {
public:
   PendingFileLoad( const char *filename )
      : filename( filename ) {}
   
   virtual OlLoadResult ExecuteLoading( Bitmap &bmp );
   
private:
   const char *filename;
};



class PendingFileAlphaLoad : public PendingBitmapLoad {
public:
   PendingFileAlphaLoad( const char *filename, const char *alphaFilename )
      : filename( filename ), alphaFilename( alphaFilename ) {}
   
   virtual OlLoadResult ExecuteLoading( Bitmap &bmp );
   
private:
   const char *filename;
   const char *alphaFilename;
};



class PendingDataLoad : public PendingBitmapLoad {
public:
   PendingDataLoad( float *data, OlTextureInfo textureInfo )
      : data( data ), textureInfo( textureInfo ) {}
   
   virtual OlLoadResult ExecuteLoading( Bitmap &bmp );
   
private:
   float *data;
   OlTextureInfo textureInfo;
};



template< class Functor >
class PendingFunctorLoad : public PendingBitmapLoad {
public:
   PendingFunctorLoad( Functor functor, int width, int height )
      : functor( functor ), width( width ), height( height ) {}
   
   virtual OlLoadResult ExecuteLoading( Bitmap &bmp );
   
private:
   Functor functor;
   int width, height;
};



}



#endif // PENDING_LOADING_HPP
