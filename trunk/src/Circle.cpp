#include "Circle.hpp"

using namespace ol;


static const float OL_NEAR_ZERO = 0.000001;


std::string ol::Ellipse::
ToString() const {
    std::ostringstream str;
    str << "Circle: Pos: ( " << pos.x << ", " << pos.y << " ) Radius: ( " << xRad << ", " << yRad
        << " ) Angle: " << angle;
    return str.str();
}


std::vector< Vec2D > ol::Ellipse::
ToPolygon( float startAngle, float angleSweep ) const {
   std::vector< Vec2D > vertices;
   vertices.reserve( int( 2.0 * AL_PI / angleIncrement ));
   
   float endAngle = startAngle - angleSweep; 
   
   if( endAngle < startAngle ) {
     float temp = startAngle;
     startAngle = endAngle;
     endAngle = temp;
   }
   
   
   for( float a = startAngle; a <= endAngle; a += angleIncrement ) {
      vertices.push_back( pos + Vec2D( cos(a) * xRad, sin(a) * yRad ));
   }
   
   if( angleSweep < 2.0 * AL_PI ) {
      vertices.push_back( pos + Vec2D( cos(endAngle) * xRad, sin(endAngle) * yRad ));
   }
   
   return vertices;
}


void ol::Ellipse::
ExecDraw() const {
   glPushMatrix();
   glTranslatef( pos.x, pos.y, 0.0 );
   RotateMatrix( angle );
   
   glBegin( GL_TRIANGLE_FAN );
      glVertex2f( 0.0, 0.0 );
      
      for( float a = 0.0; a <= 2.0 * AL_PI; a += angleIncrement ) {
         glVertex2f( cos(a) * xRad, sin(a) * yRad );
      }
      
      glVertex2f( xRad, 0.0 );
   glEnd();
   
   glPopMatrix();
}



void ol::Ellipse::
ExecDrawOutline() const {
   if( lineWidth > 1 + OL_NEAR_ZERO ) {
      DiskRender( Rgba::WHITE, Rgba::WHITE, xRad - lineWidth, yRad - lineWidth, false );
      return;
   }
   
   glLineWidth( lineWidth );
   
   glBegin( GL_LINE_LOOP );
      for( float a = 0.0; a < 2 * AL_PI; a += angleIncrement ) {
         glVertex2f( pos.x + cos(a) * xRad, pos.y + sin(a) * yRad );
      }
   glEnd();
}


void ol::Ellipse::
Draw( const Rgba &innerCol, const Rgba &outerCol ) const {
#ifdef OL_NO_STATE_CHANGE
   GLboolean texturesEnabled;
   glGetBooleanv( GL_TEXTURE_2D, &texturesEnabled );
#endif
   glDisable( GL_TEXTURE_2D );
   
   glPushMatrix();
   glTranslatef( pos.x, pos.y, 0.0 );
   RotateMatrix( angle );
   
   glBegin( GL_TRIANGLE_FAN );
      innerCol.Select();
      glVertex2f( 0.0, 0.0 );
      
      outerCol.Select();
      
      for( float a = 0.0; a < 2 * AL_PI; a += angleIncrement ) {
         glVertex2f( cos(a) * xRad, sin(a) * yRad );
      }
      
      glVertex2f( xRad, 0.0 );
   glEnd();
   
   glPopMatrix();
   
#ifdef OL_NO_STATE_CHANGE
   if( texturesEnabled )
      glEnable( GL_TEXTURE_2D );
#endif
}



void ol::Ellipse::
DrawSlice( const Rgba &color, float startAngle, float angleSweep ) const {
#ifdef OL_NO_STATE_CHANGE
   GLboolean texturesEnabled;
   glGetBooleanv( GL_TEXTURE_2D, &texturesEnabled );
#endif
   glDisable( GL_TEXTURE_2D );
   color.Select();
      
   glPushMatrix();
   glTranslatef( pos.x, pos.y, 0.0 );
   RotateMatrix( angle );
   
   float endAngle = startAngle - angleSweep; 
   
   if( endAngle < startAngle ){
     float temp = startAngle;
     startAngle = endAngle;
     endAngle = temp;
   }
   
   glBegin( GL_TRIANGLE_FAN );
      glVertex2f( 0.0, 0.0 );
       
      for( float a = startAngle; a < endAngle; a += angleIncrement ) {
         glVertex2f( cos(a) * xRad, sin(a) * yRad );
      }
      
      glVertex2f( cos( endAngle ) * xRad, sin( endAngle ) * yRad );
   glEnd();
   
   glPopMatrix();
   
#ifdef OL_NO_STATE_CHANGE
   if( texturesEnabled )
      glEnable( GL_TEXTURE_2D );
#endif
}


void ol::Ellipse::
DrawArc( const Rgba &color, float startAngle, float angleSweep, float innerXRad, float innerYRad ) const {
#ifdef OL_NO_STATE_CHANGE
   GLboolean texturesEnabled;
   glGetBooleanv( GL_TEXTURE_2D, &texturesEnabled );
#endif
   glDisable( GL_TEXTURE_2D );
   color.Select();
   
   float endAngle = startAngle - angleSweep; 
   
   if( endAngle < startAngle ) {
     float temp = startAngle;
     startAngle = endAngle;
     endAngle = temp;
   }
   
   glBegin( GL_QUADS );
      float prevIX = pos.x + cos( startAngle ) * innerXRad;
      float prevIY = pos.y + sin( startAngle ) * innerYRad;
      
      float prevOX = pos.x + cos( startAngle ) * xRad;
      float prevOY = pos.y + sin( startAngle ) * yRad;
      
      for( float a = startAngle; a < endAngle; a += angleIncrement ) {
         float cosa = cos(a);
         float sina = sin(a);
         
         float newIX = pos.x + cosa * innerXRad;
         float newIY = pos.y + sina * innerYRad;
         
         float newOX = pos.x + cosa * xRad;
         float newOY = pos.y + sina * yRad;
         
         glVertex2f( newIX, newIY );
         glVertex2f( prevIX, prevIY );
         
         glVertex2f( prevOX, prevOY );
         glVertex2f( newOX, newOY );
         
         prevIX = newIX;
         prevIY = newIY;
         
         prevOX = newOX;
         prevOY = newOY;
      }
      
      glVertex2f( pos.x + cos( endAngle ) * innerXRad, pos.y + sin( endAngle ) * innerYRad );
      glVertex2f( prevIX, prevIY );
      
      glVertex2f( prevOX, prevOY );
      glVertex2f( pos.x + cos( endAngle ) * xRad, pos.y + sin( endAngle ) * yRad ); 
   glEnd();
   
#ifdef OL_NO_STATE_CHANGE
   if( texturesEnabled )
      glEnable( GL_TEXTURE_2D );
#endif
}


void ol::Ellipse::
DrawArc( const Rgba &innerColor, const Rgba &outerColor, float startAngle, float angleSweep,
         float innerXRad, float innerYRad ) const {
#ifdef OL_NO_STATE_CHANGE
   GLboolean texturesEnabled;
   glGetBooleanv( GL_TEXTURE_2D, &texturesEnabled );
#endif
   glDisable( GL_TEXTURE_2D );
   
   float endAngle = startAngle - angleSweep; 
   
   if( endAngle < startAngle ) {
     float temp = startAngle;
     startAngle = endAngle;
     endAngle = temp;
   }
   
   glBegin( GL_QUADS );
      float prevIX = pos.x + cos( startAngle ) * innerXRad;
      float prevIY = pos.y + sin( startAngle ) * innerYRad;
      
      float prevOX = pos.x + cos( startAngle ) * xRad;
      float prevOY = pos.y + sin( startAngle ) * yRad;
      
      for( float a = startAngle; a < endAngle; a += angleIncrement ) {
         float cosa = cos(a);
         float sina = sin(a);
         
         float newIX = pos.x + cosa * innerXRad;
         float newIY = pos.y + sina * innerYRad;
         
         float newOX = pos.x + cosa * xRad;
         float newOY = pos.y + sina * yRad;
         
         innerColor.Select();
         glVertex2f( newIX, newIY );
         glVertex2f( prevIX, prevIY );
         
         outerColor.Select();
         glVertex2f( prevOX, prevOY );
         glVertex2f( newOX, newOY );
         
         prevIX = newIX;
         prevIY = newIY;
         
         prevOX = newOX;
         prevOY = newOY;
      }
      
      innerColor.Select();
      glVertex2f( pos.x + cos( endAngle ) * innerXRad, pos.y + sin( endAngle ) * innerYRad );
      glVertex2f( prevIX, prevIY );
      
      outerColor.Select();
      glVertex2f( prevOX, prevOY );
      glVertex2f( pos.x + cos( endAngle ) * xRad, pos.y + sin( endAngle ) * yRad ); 
   glEnd();
   
#ifdef OL_NO_STATE_CHANGE
   if( texturesEnabled )
      glEnable( GL_TEXTURE_2D );
#endif
}


void ol::Ellipse::
DiskRender( const Rgba &innerCol, const Rgba &outerCol, float innerXRad, float innerYRad, bool setCols ) const {
#ifdef OL_NO_STATE_CHANGE
   GLboolean texturesEnabled;
   glGetBooleanv( GL_TEXTURE_2D, &texturesEnabled );
#endif
   glDisable( GL_TEXTURE_2D );
   
   glBegin( GL_QUADS );
      float prevIX = pos.x + innerXRad;
      float prevIY = pos.y;
      
      float prevOX = pos.x + xRad;
      float prevOY = pos.y;
      
      for( float a = 0.0; a <= 2 * AL_PI; a += angleIncrement ) {
         float cosa = cos(a);
         float sina = sin(a);
         
         float newIX = pos.x + cosa * innerXRad;
         float newIY = pos.y + sina * innerYRad;
         
         float newOX = pos.x + cosa * xRad;
         float newOY = pos.y + sina * yRad;
         
         if( setCols )
            innerCol.Select();
         
         glVertex2f( newIX, newIY );
         glVertex2f( prevIX, prevIY );
         
         if( setCols )
            outerCol.Select();
         
         glVertex2f( prevOX, prevOY );
         glVertex2f( newOX, newOY );
         
         prevIX = newIX;
         prevIY = newIY;
         
         prevOX = newOX;
         prevOY = newOY;
      }
      
      if( setCols )
         innerCol.Select();
         
      glVertex2f( pos.x + innerXRad, pos.y );
      glVertex2f( prevIX, prevIY );
      
      if( setCols )
         outerCol.Select();
      
      glVertex2f( prevOX, prevOY );
      glVertex2f( pos.x + xRad, pos.y );
   glEnd();
   
#ifdef OL_NO_STATE_CHANGE
   if( texturesEnabled )
      glEnable( GL_TEXTURE_2D );
#endif
}
