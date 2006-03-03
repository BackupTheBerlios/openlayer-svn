#include "Collisions.hpp"
#include "Line.hpp"
#include "Internal.hpp"

using namespace ol;


const Line Collision::DEFAULT_SEGMENT( Vec2D( 0, 0 ), Vec2D( 0, 0 ));
const Vec2D Collision::DEFAULT_NORMAL( 0, 0 );


Collision::
Collision( const Line &aSegment, const Line &bSegment )
   : isCollision( true ) {

   segments[(int) OBJ_A] = new Line( aSegment );
   segments[(int) OBJ_B] = new Line( bSegment );
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


const Line &Collision::
GetSegment( CollidingObject objectID ) {
   Line *segment = segments[(int) objectID];

   if( !segment ) {
      const Vec2D *normal = normals[(int) objectID];

      if( !normal ) {
         OlError( "Can't create a virtual collision segment without a normal!" );
         return DEFAULT_SEGMENT;
      }

      Vec2D s = Vec2D( -normal->y, normal->x ) * 0.5;
      segment = new Line( point - s, point + s );
   }

   return *segment;
}


Vec2D Collision::
GetNormal( CollidingObject objectID ) {
   Vec2D *normal = normals[(int) objectID];

   if( !normal ) {
      const Line *segment = segments[(int) objectID];

      if( !segment ) {
         OlError( "Can't create a collision normal without a segment!" );
         return DEFAULT_NORMAL;
      }

      *normal = segment->GetNormal();
   }

   return *normal;
}


Collision::
~Collision() {

   for( int i = 0; i < (int) NUM_OBJS; i++ ) {

      if( segments[i] ) {
         delete segments[i];
      }

      if( normals[i] ) {
         delete normals[i];
      }
   }
}

Collision& Collision::
operator =( const Collision& c ) {
	isCollision = c.isCollision;
	point = c.point;
	if( c.segments[(int)OBJ_A] )
		segments[(int)OBJ_A] = new Line( *c.segments[(int)OBJ_A] );
	if( c.segments[(int)OBJ_B] )
		segments[(int)OBJ_B] = new Line( *c.segments[(int)OBJ_B] );
	if( c.normals[(int)OBJ_A] )
		normals[(int)OBJ_A] = new Vec2D( *c.normals[(int)OBJ_A] );
	if( c.normals[(int)OBJ_B] )
		normals[(int)OBJ_B] = new Vec2D( *c.normals[(int)OBJ_B] );
	return *this;
}




