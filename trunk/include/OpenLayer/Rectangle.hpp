#ifndef OL_RECT_HPP
#define OL_RECT_HPP


#include "Shape.hpp"
#include "Settings.hpp"
#include <cmath>


namespace ol {


class Rect : public Shape {
public:
   Rect()
      : Shape( Settings::GetCircleAccuracy()) {}

   Rect( Vec2D pos, Vec2D size, float lineWidth = 1.0,
         float roundness = 0.0, float accuracy = Settings::GetCircleAccuracy() )
      : Shape( lineWidth ), pos( pos ), size( size ), roundness( roundness ), rotationAngle( 0.0 )
        { SetAccuracy( accuracy ); }

   Rect( float x, float y, float w, float h, float lineWidth = 1.0,
         float roundness = 0.0, float accuracy = Settings::GetCircleAccuracy() )
      : Shape( lineWidth ), pos( x, y ), size( w, h ), roundness( roundness ), rotationAngle( 0.0 )
        { SetAccuracy( accuracy ); }

	virtual ~Rect(){}

   //virtual ~Rect(){}

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

   // Draws a gradient filled rectangle //
   void Draw( const Rgba *colors ) const;

   // Moves the rectangle by the specified amount //
   virtual void MoveBy( const Vec2D &amount ) {
      pos += amount;
   }

   virtual void MoveTo( const Vec2D &position ) {
   	pos = position;
   }
   
   virtual void RotateBy( float angle ) {
      rotationAngle += angle;
   }

   // Sets the visual accuracy of the rounded rectangle //
   inline void SetAccuracy( float accuracy ) {
      angleIncrement = std::min(( 2.0 * asin( 1.0/roundness )/accuracy ), 0.35 * AL_PI );
   }

   void SetRoundness( float roundness ) {
      this->roundness = roundness;
      SetAccuracy( Settings::GetCircleAccuracy() );
   }

   // Returns the common area of this rectangle and an another one //
   Rect ClippedTo( const Rect &other ) const;

   Vec2D pos;
   Vec2D size;

protected:
   // Draws a filled rectangle //
   void ExecDraw() const;

   // Draws a non-filled rectangle //
   void ExecDrawOutline() const;

   void RenderCircleQuarter( float angleStart ) const;
   void RenderCircleOutlineQuarter( float angleStart ) const;
   void RenderThickCircleOutlineQuarter( float angleStart ) const;

   float roundness;
   float angleIncrement;
   float rotationAngle;
};


}



#endif // OL_RECT_HPP
