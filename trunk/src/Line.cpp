#include "Line.hpp"
#include "Internal.hpp"
#include "General.hpp"


using namespace ol;


void Line::
ExecDraw() const {
   glLineWidth( lineWidth );
   
   glBegin( GL_LINES );
      glVertex2f( start.x, start.y );
      glVertex2f( end.x, end.y );
   glEnd();
}



void Line::
Draw( const Rgba &color1, const Rgba &color2 ) const {
   glLineWidth( lineWidth );
   
#ifdef OL_NO_STATE_CHANGE
   GLboolean texturesEnabled;
   glGetBooleanv( GL_TEXTURE_2D, &texturesEnabled );
#endif
   glDisable( GL_TEXTURE_2D );
   
   glBegin( GL_LINES );
      color1.Select();
      glVertex2f( start.x, start.y );
      color2.Select();
      glVertex2f( end.x, end.y );
   glEnd();
   
#ifdef OL_NO_STATE_CHANGE
   if( texturesEnabled )
      glEnable( GL_TEXTURE_2D );
#endif
}
