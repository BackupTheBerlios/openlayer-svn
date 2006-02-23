#include "Rgba.hpp"
using namespace ol;

// STATIC CLASS VARIABLES //

Rgba Rgba::BLACK = Rgba( 0.0, 0.0, 0.0, 1.0 );
Rgba Rgba::WHITE = Rgba( 1.0, 1.0, 1.0, 1.0 );
Rgba Rgba::RED = Rgba( 1.0, 0.0, 0.0, 1.0 );
Rgba Rgba::YELLOW = Rgba( 1.0, 1.0, 0.0, 1.0 );
Rgba Rgba::GREEN = Rgba( 0.0, 1.0, 0.0, 1.0 );
Rgba Rgba::BLUE = Rgba( 0.0, 0.0, 1.0, 1.0 );
Rgba Rgba::INVISIBLE = Rgba( 0.0, 0.0, 0.0, 0.0 );


// GENERAL FUNCTIONS //


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



