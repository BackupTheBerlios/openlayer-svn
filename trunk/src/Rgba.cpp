#include "Rgba.hpp"
#include "Internal.hpp"

using namespace ol;
using namespace std;

// STATIC CLASS VARIABLES //

Rgba Rgba::BLACK = Rgba( 0.0, 0.0, 0.0, 1.0 );
Rgba Rgba::WHITE = Rgba( 1.0, 1.0, 1.0, 1.0 );
Rgba Rgba::RED = Rgba( 1.0, 0.0, 0.0, 1.0 );
Rgba Rgba::YELLOW = Rgba( 1.0, 1.0, 0.0, 1.0 );
Rgba Rgba::GREEN = Rgba( 0.0, 1.0, 0.0, 1.0 );
Rgba Rgba::BLUE = Rgba( 0.0, 0.0, 1.0, 1.0 );
Rgba Rgba::INVISIBLE = Rgba( 0.0, 0.0, 0.0, 0.0 );


static string HEX_CHARS = "0123456789abcdef";
static string HEX_CHARS_UPPER = "0123456789ABCDEF";


// GENERAL FUNCTIONS //


Rgba::
Rgba( const string &hex ) {
   if( hex.length() != 6 && hex.length() != 8 ) {
      OlError( "Cannot parse the color " + hex );
      return;
   }
   
   r = CompToF( parseHex( hex, 0 ));
   g = CompToF( parseHex( hex, 2 ));
   b = CompToF( parseHex( hex, 4 ));
   
   if( hex.length() == 8 ) {
      a = CompToF( parseHex( hex, 6 ));
   }
   else {
      a = 1.0;
   }
}


inline static unsigned int GetHexValue( char ch ) {
   string::size_type pos = HEX_CHARS.find( ch );
   
   if( pos != string::npos ) {
      return pos;
   }
   
   else return HEX_CHARS_UPPER.find( ch );
}


unsigned int Rgba::
parseHex( const string &hex, int pos ) {
   return ( GetHexValue( hex[pos] ) << 4 ) | GetHexValue( hex[pos+1] );
}


// Optimized version by Leniuch //

Rgba Rgba::
MixWith( const Rgba &other, float factor ) const {
   float red = r + (other.r - r) * factor;
   float green = g + (other.g - g) * factor;
   float blue = b + (other.b - b) * factor;
   float alpha = a + (other.a - a) * factor;
   
   return Rgba( red, green, blue, alpha );
}



int Rgba::
SpecialPacked() const {
   return (Rgba::CompToI(a) << 24) | (Rgba::CompToI(r) << 16) | (Rgba::CompToI(g) << 8) | Rgba::CompToI(b);
}



Rgba::
Rgba( int col, bool notUsed )
    : r( CompToF(( col >> 16 ) & 0xff)), g( CompToF(( col >> 8 ) & 0xff)),
      b( CompToF( col & 0xff)), a( CompToF(( col >> 24 ) & 0xff)) {
   notUsed = true;
}



Rgba::
Rgba( bool invalidiate )
    : r( -1.0 ), g( -1.0 ), b( -1.0 ), a( -1.0 ) {
   invalidiate = true;
}



bool Rgba::
IsValid() {
   return r >= 0.0 && g >= 0.0 && b >= 0.0 && a >= 0.0;
}




int Rgba::
Packed() const {
   return makeacol32( Rgba::CompToI(r), Rgba::CompToI(g), Rgba::CompToI(b), Rgba::CompToI(a));
}



