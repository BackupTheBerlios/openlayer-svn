#ifndef OL_LINE_HPP
#define OL_LINE_HPP


#include "Shape.hpp"
#include "Vec2D.hpp"
#include "Settings.hpp"


namespace ol {


class Line : public Shape {
public:
   Line()
      : Shape( 1.0 ) {}

   Line( float x1, float y1, float x2, float y2, float lineWidth = 1.0 )
      : Shape( lineWidth ), start( x1, y1 ), end( x2, y2 ) {}

   Line( Vec2D start, Vec2D end, float lineWidth = 1.0 )
      : Shape( lineWidth ), start( start ), end( end ) {}

   virtual ~Line() {}

   inline void Draw( const Rgba &color ) const {
   #ifdef OL_NO_STATE_CHANGE
      GLboolean texturesEnabled;
      glGetBooleanv( GL_TEXTURE_2D, &texturesEnabled );
   #endif
      glDisable( GL_TEXTURE_2D );
      color.Select();

      ExecDraw();

   #ifdef OL_NO_STATE_CHANGE
      if( texturesEnabled )
         glEnable( GL_TEXTURE_2D );
   #endif
   }

   void DrawOutline( const Rgba &color ) const {
   #ifdef OL_NO_STATE_CHANGE
      GLboolean texturesEnabled;
      glGetBooleanv( GL_TEXTURE_2D, &texturesEnabled );
   #endif
      glDisable( GL_TEXTURE_2D );
      color.Select();

      ExecDrawOutline();

   #ifdef OL_NO_STATE_CHANGE
      if( texturesEnabled )
         glEnable( GL_TEXTURE_2D );
   #endif
   }

   // Draws the line with the color smoothly changing from start of the line to the end //
   virtual void Draw( const Rgba &startColor, const Rgba &endColor ) const;

   // Moves the line by the specified amount //
   virtual void MoveBy( const Vec2D &amount ) {
      origin += amount;
   }
   virtual void MoveTo( const Vec2D &position ) {
		origin = position;
   }

   // Returns the intersection point of two lines //
   Vec2D GetIntersectionPoint( const Line &other ) const;

   // Tests if the two line segments collide //
   bool Collides( const Line &other ) const;

   Vec2D GetNormal() const {
      Vec2D s = end - start;
      Vec2D normal( -s.y, s.x );
      return normal.Normalized();
   }
   
   
   Collision GetCollision( const Line &other ) const {
      if( Collides( other )) {
         return Collision( *this, other );
      }
      else {
         return Collision( false );
      }
   }
   
   
   Collision GetCollision( const Line &other, const Placement &thisPlacement,
                           const Placement &otherPlacement ) const;
   
   Vec2D start, end;
	Vec2D origin;
protected:
   
   // Draws the line with the specified color //
   void ExecDraw() const;

   // Draws the line with the specified color //
   void ExecDrawOutline() const {
      ExecDraw();
   }
};



}



#endif // OL_LINE_HPP
