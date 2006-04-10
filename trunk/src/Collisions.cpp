#include "Collisions.hpp"
#include "Line.hpp"
#include "Internal.hpp"
#include <string>

using namespace ol;


const Line Collision::DEFAULT_SEGMENT( Vec2D( 0, 0 ), Vec2D( 0, 0 ));
const Vec2D Collision::DEFAULT_NORMAL( 0, 0 );


Collision::
Collision( const Line &aSegment, const Line &bSegment )
   : isCollision( true ) {
   segmentLists.push_back( new std::pair< Line, Line >( aSegment, bSegment ));
   
	normals[(int) OBJ_A] = 0;
	normals[(int) OBJ_B] = 0;
   point = aSegment.GetIntersectionPoint( bSegment );
}


Collision::
Collision( const Collision& c ) {
	operator =( c );
}


Line Collision::
CreateVirtualSegment( const Vec2D &normal ) {
   Vec2D s = Vec2D( -normal.y, normal.x ) * 0.5;
   return Line( point - s, point + s );
}


const Line Collision::
GetSegment( CollidingObject objectID ) {
   if( !segmentLists.empty()) {
      switch( objectID ) {
         case OBJ_A:
            return segmentLists[0]->first;
          break;
         case OBJ_B:
            return segmentLists[0]->second;
          break;
         default:
            OlError(std::string( "Unknown objectID in GetSegment: " ) + ToString((int) objectID ));
      }
   }
   else {
      const Vec2D *normal = normals[(int) objectID];
      
      if( !normal ) {
         OlError( "Can't create a virtual collision segment without a normal!" );
         return DEFAULT_SEGMENT;
      }
      
      Vec2D s = Vec2D( -normal->y, normal->x ) * 0.5;
      return Line( point - s, point + s );
   }
}


Vec2D Collision::
GetNormal( CollidingObject objectID ) {
   Vec2D *normal = normals[(int) objectID];
   
   if( !normal ) {
      if( !segmentLists.empty()) {
         const Line &segment = GetSegment( objectID );
         normal = new Vec2D( segment.GetNormal() );
         normals[(int) objectID] = normal;
      }
      else {
         OlError( "Can't create a collision normal without a segment!" );
         return DEFAULT_NORMAL;
      }
   }
   
   return *normal;
}


Collision::
~Collision() {
   for( int i = 0; i < (int) NUM_OBJS; i++ ) {
      if( normals[i] ) {
         delete normals[i];
      }
   }
}


Collision& Collision::
operator =( const Collision& c ) {
	isCollision = c.isCollision;
	point = c.point;
	
	for( std::vector< std::pair< Line, Line > *> ::const_iterator iter = c.segmentLists.begin();
        iter != c.segmentLists.end(); iter++ ) {
      segmentLists.push_back( new std::pair< Line, Line > (*(*iter)));
   }
	
	normals[(int)OBJ_A] = ( c.normals[(int)OBJ_A] )? new Vec2D( *c.normals[(int)OBJ_A] ) : 0;
	normals[(int)OBJ_B] = ( c.normals[(int)OBJ_B] )? new Vec2D( *c.normals[(int)OBJ_B] ) : 0;
	return *this;
}

