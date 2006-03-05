#ifndef OL_LINE_STRIP_HPP
#define OL_LINE_STRIP_HPP

#include "Shape.hpp"
#include "Vec2D.hpp"
#include "Placement.hpp"
#include <list>


namespace ol {


class Bitmap;

// A series of lines clued together //

class LineStrip : public Shape {
public:
   LineStrip( float lineWidth = 1.0, const Bitmap *texture = 0 )
      : Shape( lineWidth ), totalLength( 0.0 ), texture( texture ) {}

   template< class std_container >
   LineStrip( const std_container &vertices, float lineWidth = 1.0, const Bitmap *texture = 0 );

   virtual ~LineStrip() {}
   
   // Draw the line strip filled with a color //
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

      ExecDraw();

   #ifdef OL_NO_STATE_CHANGE
      if( texturesEnabled )
         glEnable( GL_TEXTURE_2D );
   #endif
   }

   // Draw the line strip with a gradient //
   virtual void Draw( const Rgba &startColor, const Rgba &endColor ) const;

   // Moves the line strip by the specified amount //
   virtual void MoveBy( const Vec2D &amount ) {
		placement.MoveBy( amount );
   }

   virtual void MoveTo( const Vec2D &position ) {
   	this->placement.SetPosition( position );
   }
   
   inline void SetPlacement( const Placement &placement ) {
      this->placement = placement;
   }
   
   inline const Placement &GetPlacement() const {
      return placement;
   }

   // Sets the filler texture of the line strip //
   inline void SetTexture( const Bitmap &texture ) {
      this->texture = &texture;
   }

   // Disables the filler texture //
   inline void DisableTexture() {
      texture = 0;
   }

   // Add a vertex to the end of the line strip //
   inline void AddToEnd( Vec2D vertex ) {
      if( vertices.size() > 1 ) {
         float length = ( vertex - vertices.back() ).GetMagnitude();
         lengths.push_back( length );
         totalLength += length;
      }

      vertices.push_back( vertex );
   }

   // Add a vertex to the beginning of the line strip //
   inline void AddToBegin( Vec2D vertex ) {
      if( vertices.size() > 1 ) {
         float length = ( vertices.front() - vertex ).GetMagnitude();
         lengths.push_front( length );
         totalLength += length;
      }

      vertices.push_front( vertex );
   }

   // Delete the first vertex of the line strip //
   inline void DeleteFirst() {
      if( !vertices.empty() ) {
         vertices.pop_front();

         totalLength -= lengths.front();
         lengths.pop_front();
      }
   }

   // Delete the first last of the line strip //
   inline void DeleteLast() {
      if( !vertices.empty() ) {
         vertices.pop_back();

         totalLength -= lengths.back();
         lengths.pop_back();
      }
   }
   
   // Returns the specified vertex //
   Vec2D GetVertex( int index ) const;
   
   // Returns the number of vertices //
   inline int GetNumOfVertices() const {
      return vertices.size();
   }
   
   const std::list< Vec2D > &GetVertices() const {
      return vertices;
   }
   
   // The following functions could be derived from Set/GetPlacement
   
	inline void SetRotationAngle( float angle ) {
		placement.SetRotation( angle );
	}

	inline float GetRotationAngle() {
		return placement.GetRotation();
	}

	inline void RotateBy( float angle ) {
		placement.RotateBy( angle );
	}
	
	// Collision routines
   
   inline bool Collides( const LineStrip &other, const Placement &thisPlacement,
                  const Placement &otherPlacement ) const {
      return DoCollisionTest( other, thisPlacement, otherPlacement, false ).IsCollision();
   }
   
   
   inline Collision GetCollision( const LineStrip &other, const Placement &thisPlacement,
                           const Placement &otherPlacement ) const {
      return DoCollisionTest( other, thisPlacement, otherPlacement, true );
   }
   
private:
   Collision DoCollisionTest( const LineStrip &other, const Placement &thisPlacement,
                              const Placement &otherPlacement, bool getResults = true ) const;
   
   // Draws the line strip with the specified color //
   void ExecDraw() const;

   // Draws the line strip with the specified color //
   void ExecDrawOutline() const {
      ExecDraw();
   }

   // Raw line strip rendering function //
   void Render( const Rgba *color1, const Rgba *color2 ) const;

   std::list< Vec2D > vertices;
   std::list< float > lengths;
   float totalLength;
   
   Placement placement;

   const Bitmap *texture;
};


// TEMPLATES //

template< class std_container >
LineStrip::
LineStrip( const std_container &theVertices, float lineWidth, const Bitmap *texture )
   : Shape( lineWidth ), totalLength( 0.0 ), texture( texture ) {
   for( typename std_container::const_iterator iter = theVertices.begin(); iter != theVertices.end(); iter++ ) {
      AddToEnd( *iter );
   }
}


}

#endif // OL_LINE_STRIP_HPP
