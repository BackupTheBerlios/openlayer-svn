#ifndef OL_INTERNAL_HPP
#define OL_INTERNAL_HPP

#include "Includes.hpp"
#include <string>
#include <math.h>
#include <fstream>
#include <iostream>


namespace ol {


static inline void OlLog( std::string logStr, bool append = true ) {
#ifdef OL_TO_STDOUT
   std::cout << logStr << "\n";
#else
   std::ofstream file( "openlayer.log", append? std::ios::app : std::ios::trunc );
   file << logStr << "\n";
   file.close();
#endif
}



static inline void OlError( std::string errorStr ) {
   std::string msg = std::string("ERROR: ") + errorStr;
   OlLog( msg );
}



//TTD - change OlAssert to do FILE and LINE correctly



#define OlAssert( condition ) \
if( !(condition) ) { \
  char linechars[64]; \
  sprintf( linechars, "%i", __LINE__ ); \
  OlError( "Assertion failed in \nFile:     " + std::string( __FILE__ ) \
			+ ", line " + std::string( linechars ) \
			+ "\nFunction: " + std::string( __PRETTY_FUNCTION__ ) \
			+ "\nProgram shutting down" ); \
  exit( -1 ); \
}



static inline float RadiansToDegrees( float angle ) {
   return angle * (180.0/AL_PI);
}




static inline void RotateMatrix( float angle ) {
   #ifdef OL_ANGLES_IN_DEGREES
      glRotatef( angle, 0.0, 0.0, 1.0 );
   #else // OL_ANGLES_IN_DEGREES
      glRotatef( RadiansToDegrees( angle ), 0.0, 0.0, 1.0 );
   #endif // OL_ANGLES_IN_DEGREES
}


}



#endif // OL_INTERNAL_HPP
