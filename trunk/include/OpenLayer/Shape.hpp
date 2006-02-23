#ifndef OL_SHAPE_HPP
#define OL_SHAPE_HPP


#include "Includes.hpp"
#include "Rgba.hpp"
#include "Vec2D.hpp"
#include "Collisions.hpp"


#ifdef OL_NO_STATE_CHANGE
   #define OL_SHAPE_START_RENDERING() \
      GLboolean texturesEnabled; \
      glGetBooleanv( GL_TEXTURE_2D, &texturesEnabled ); \
      glDisable( GL_TEXTURE_2D );
      
#else // OL_NO_STATE_CHANGE
   
   #define OL_SHAPE_START_RENDERING() glDisable( GL_TEXTURE_2D );

#endif // OL_NO_STATE_CHANGE


#ifdef OL_NO_STATE_CHANGE
   #define OL_SHAPE_FINISH_RENDERING() \
      if( texturesEnabled ) \
         glEnable( GL_TEXTURE_2D );
   
#else // OL_NO_STATE_CHANGE
   
   #define OL_SHAPE_FINISH_RENDERING() (void) 0;
   
#endif // OL_NO_STATE_CHANGE

namespace ol {


// The base class of all Shapes //

class Shape {
public:
   Shape( float lineWidth = 1.0 )
      : lineWidth( lineWidth ), displayList( 0 ) {}
   
   void Draw( const Rgba &color ) const {
      OL_SHAPE_START_RENDERING()
      
      color.Select();
      ExecDraw();
      
      OL_SHAPE_FINISH_RENDERING()
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
   
   
   // Returns true if this and the other Shape collide //
   bool Collides( const Shape &other, const Placement &thisPlacement, const Placement &otherPlacement );
   
   
   // Returns true if this and the other Shape collide //
   Collision GetCollision( const Shape &other, const Placement &thisPlacement,
                           const Placement &otherPlacement );
   
   
   // Records the results of the drawing function instead of actually drawing the primitive //
   inline void RecordDraw() {
      StartRecording();
      ExecDraw();
      FinishRecording();
   }
   
   // Same as above but with outlines //
   inline void RecordDrawOutline() {
      StartRecording();
      ExecDrawOutline();
      FinishRecording();
   }
   
   inline void DrawRecord( const Rgba &color ) const {
   #ifdef OL_NO_STATE_CHANGE
      GLboolean texturesEnabled;
      glGetBooleanv( GL_TEXTURE_2D, &texturesEnabled );
   #endif
      glDisable( GL_TEXTURE_2D );
      color.Select();
      
      glCallList( displayList );
      
   #ifdef OL_NO_STATE_CHANGE
      if( texturesEnabled )
         glEnable( GL_TEXTURE_2D );
   #endif
   }
   
   inline void DrawRecord( const Rgba &color, const Vec2D &displacement ) const {
      glPushMatrix();
      glTranslatef( displacement.x, displacement.y, 0.0 );
      DrawRecord( color );
      glPopMatrix();
   }
   
   // Moves the shape by the specified amount //
   virtual void MoveBy( const Vec2D &amount ) = 0;
   
   // Sets the line width of the shape //
   inline void SetLineWidth( float lineWidth ) {
      this->lineWidth = lineWidth;
   }
   
   // Returns the line width of the shape //
   inline float GetLineWidth() {
      return lineWidth;
   }
   
protected:
   virtual void ExecDraw() const = 0;
   
   virtual void ExecDrawOutline() const = 0;
   
   inline void StartRecording() {
      displayList = glGenLists( 1 );
      glNewList( displayList, GL_COMPILE );
   }
   
   
   inline void FinishRecording() {
      glEndList();
   }
   
   inline void RotateMatrix( float angle ) const {
      #ifdef OL_ANGLES_IN_DEGREES
         glRotatef( angle, 0.0, 0.0, 1.0 );
      #else // OL_ANGLES_IN_DEGREES
         glRotatef( angle * ( 180.0/AL_PI ), 0.0, 0.0, 1.0 );
      #endif // OL_ANGLES_IN_DEGREES
   }
   
   float lineWidth;
   int displayList;
};


}



#endif // OL_SHAPES_HPP
