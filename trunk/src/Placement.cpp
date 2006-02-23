#include "Placement.hpp"
#include "math.h"


using namespace ol;


Matrix2D Placement::
Get2DMatrix() const {
   Matrix2D returnVal;
   
   float sinA = sin( rotation );
   float cosA = cos( rotation );
   
   returnVal.Set( 0, 0, stretch * cosA );
   returnVal.Set( 1, 0, stretch * sinA );
   returnVal.Set( 0, 1, stretch * -sinA );
   returnVal.Set( 1, 1, stretch * cosA );
   
   return returnVal;
}
