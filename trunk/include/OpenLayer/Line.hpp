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
      : Shape( lineWidth ), start( x1, y1 ), end( x2, y2 ),
        p_start( start ), p_end( end ) {}

   Line( Vec2D start, Vec2D end, float lineWidth = 1.0 )
      : Shape( lineWidth ), start( start ), end( end ),
        p_start( start ), p_end( end ) {}

   virtual ~Line(){}

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
      start += amount;
      end += amount;
   }
   virtual void MoveTo( const Vec2D &position ) {
		start = p_start + position;
		end = p_end + position;
   }

   // Returns the intersection point of two lines //
   Vec2D GetIntersectionPoint( const Line &other ) const {
      float ua = (( other.end.x - other.start.x ) * ( start.y - other.start.y )
         - ( other.end.y - other.start.y ) * ( start.x - other.start.x ))
         / (( other.end.y - other.start.y ) * ( end.x - start.x )
         - ( other.end.x - other.start.x ) * ( end.y - start.y ));
      return Vec2D( start.x + ua * ( end.x - start.x ),
                    start.y + ua * ( end.y - start.y ));
   }

   // Tests if the two line segments collide //
   bool Collides( const Line &other ) const {
      return IsCounterClockwise( start, end, other.start )
             != IsCounterClockwise( start, end, other.end )
             &&
             IsCounterClockwise( other.start, other.end, start )
             != IsCounterClockwise( other.start, other.end, end );
   }

   Vec2D GetNormal() const {
      Vec2D s = end - start;
      Vec2D normal( -s.y, s.x );
      return normal.Normalized();
   }

   Vec2D start, end;

protected:
   // Draws the line with the specified color //
   void ExecDraw() const;

   // Draws the line with the specified color //
   void ExecDrawOutline() const {
      ExecDraw();
   }


private:
// Could either keep the initial 2 points and the current 2 points, or just one
// pair of points with an absolute position.  Based on the assumption that the
// line will be drawn more often than its position is changed, I went for
// two sets of points
	Vec2D p_start, p_end;
};



}



#endif // OL_LINE_HPP
