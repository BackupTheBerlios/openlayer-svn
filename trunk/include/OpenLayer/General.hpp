#ifndef OL_GENERAL_HPP
#define OL_GENERAL_HPP


#include <sstream>
#include "Declspec.hpp"

namespace ol {


// ToString - Converts a variable to a string //

template< class Type >
OL_LIB_DECLSPEC std::string ToString( Type value ) {
   std::stringstream s;
   s << value;
   return s.str();
}


template< class Type >
OL_LIB_DECLSPEC std::string VarToString( Type value ) {
   return ToString( value );
}


OL_LIB_DECLSPEC int ToNextPowOfTwo( int num );


enum OL_LIB_DECLSPEC Axis {
   X_AXIS,
   Y_AXIS
};


enum OL_LIB_DECLSPEC OutlineTextureMode {
   SHRINK,
   STRETCH,
   OPTIMIZE
};


enum OL_LIB_DECLSPEC TextAlignment {
   LEFT,
   RIGHT,
   CENTER,
   JUSTIFY
};


}



#endif // OL_GENERAL_HPP

