#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP


#include "Vec2D.hpp"
#include <utility>
#include <vector>

namespace ol {


class Line;


enum CollidingObject {
   OBJ_A = 0,
   OBJ_B = 1,
   NUM_OBJS = 2
};


class Collision {
public:
   Collision( bool isCollision = false )
      : isCollision( isCollision ) {
      normals[(int) OBJ_A] = 0;
      normals[(int) OBJ_B] = 0;
	}
   
   Collision( const Line &aSegment, const Line &bSegment );


   Collision( const Vec2D &aNormal, const Vec2D &bNormal, const Vec2D &collisionPoint )
      : isCollision( true ), point( collisionPoint ) {
      normals[(int) OBJ_A] = new Vec2D( aNormal );
      normals[(int) OBJ_B] = new Vec2D( bNormal );
   }
   

   Collision( const Collision& c );
   ~Collision();


   // Returns true if a collision occured //
   inline bool IsCollision() {
      return isCollision;
   }


   // Same as above using a conversion to bool //
   inline operator bool() {
      return IsCollision();
   }


   // Returns the exact point of collision //
   inline Vec2D GetPoint() {
      return point;
   }

   
   // Returns a colliding line segment for OBJ_A or OBJ_B //
   const Line GetSegment( CollidingObject objectID );
   
   inline const std::vector< std::pair< Line, Line > *> &GetAllCollidingSegments() {
      return segmentLists;
   }
   
   // Returns the normal of the collision point for OBJ_A or OBJ_B //
   Vec2D GetNormal( CollidingObject objectID );

   Collision& operator =( const Collision& c );

private:
   Line CreateVirtualSegment( const Vec2D &normal );
   
   bool isCollision;
   Vec2D point;
   
   std::vector< std::pair< Line, Line > *> segmentLists;
   Vec2D *normals[(int) NUM_OBJS];

   const static Line DEFAULT_SEGMENT;
   const static Vec2D DEFAULT_NORMAL;
};



}





#endif // COLLISIONS_HPP
