#include "Line.hpp"
#include "Internal.hpp"
#include "General.hpp"


using namespace ol;


void Line::
ExecDraw() const {
   glLineWidth( lineWidth );
	glPushMatrix();
	glTranslatef( origin.x, origin.y, 0.0 );
   glBegin( GL_LINES );
      glVertex2f( start.x, start.y );
      glVertex2f( end.x, end.y );
   glEnd();
   glPopMatrix();
}



void Line::
Draw( const Rgba &color1, const Rgba &color2 ) const {
   glLineWidth( lineWidth );

#ifdef OL_NO_STATE_CHANGE
   GLboolean texturesEnabled;
   glGetBooleanv( GL_TEXTURE_2D, &texturesEnabled );
#endif
   glDisable( GL_TEXTURE_2D );
   glPushMatrix();
   glTranslatef( origin.x, origin.y, 0.0 );
   glBegin( GL_LINES );
      color1.Select();
      glVertex2f( start.x, start.y );
      color2.Select();
      glVertex2f( end.x, end.y );
   glEnd();
   glPopMatrix();

#ifdef OL_NO_STATE_CHANGE
   if( texturesEnabled )
      glEnable( GL_TEXTURE_2D );
#endif
}



Vec2D Line::
GetIntersectionPoint( const Line &other ) const {
	Vec2D absStart = start + origin;
	Vec2D absEnd = end + origin;
	Vec2D otherStart = other.start + other.origin;
	Vec2D otherEnd = other.end + other.origin;

	float ua = (( otherEnd.x - otherStart.x ) * ( absStart.y - otherStart.y )
		- ( otherEnd.y - otherStart.y ) * ( absStart.x - otherStart.x ))
		/ (( otherEnd.y - otherStart.y ) * ( absEnd.x - absStart.x )
		- ( otherEnd.x - otherStart.x ) * ( absEnd.y - absStart.y ));
	return Vec2D( absEnd.x + ua * ( absEnd.x - absStart.x ),
						absEnd.y + ua * ( absEnd.y - absStart.y ));
}


// Tests if the two line segments collide //
bool Line::
Collides( const Vec2D &thisStart, const Vec2D &thisEnd, const Vec2D &otherOrigin,
          const Vec2D &otherStart, const Vec2D &otherEnd ) const {
	Vec2D absStart = thisStart + origin;
	Vec2D absEnd = thisEnd + origin;
	Vec2D otherAbsStart = otherStart + otherOrigin;
	Vec2D otherAbsEnd = otherEnd + otherOrigin;

	return IsCounterClockwise( absStart, absEnd, otherAbsStart )
		 != IsCounterClockwise( absStart, absEnd, otherAbsEnd )
			&&
			 IsCounterClockwise( otherAbsStart, otherAbsEnd, absStart )
		 != IsCounterClockwise( otherAbsStart, otherAbsEnd, absEnd );
}


bool Line::
Collides( const Line &other, const Placement &thisPlacement,
              const Placement &otherPlacement ) const {
   Matrix2D thisTransform = thisPlacement.Get2DMatrix();
   Matrix2D otherTransform = otherPlacement.Get2DMatrix();
   
   Vec2D thisStart = thisTransform.Transform( start - thisPlacement.GetRotationPivot() );
   Vec2D thisEnd = thisTransform.Transform( end - thisPlacement.GetRotationPivot() );
   
   Vec2D otherStart = otherTransform.Transform( other.start - otherPlacement.GetRotationPivot() );
   Vec2D otherEnd = otherTransform.Transform( other.end - otherPlacement.GetRotationPivot() );
   
   return Collides( thisStart, thisEnd, other.origin, otherStart, otherEnd );
}



