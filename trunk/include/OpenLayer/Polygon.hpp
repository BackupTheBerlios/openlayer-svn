#ifndef OL_POLYGON_HPP
#define OL_POLYGON_HPP

#include <vector>
#include "Vec2D.hpp"
#include "Rgba.hpp"
#include "Internal.hpp"
#include "General.hpp"
#include "Shape.hpp"
#include "Placement.hpp"
#include "Collisions.hpp"


namespace ol {

class Bitmap;

class Poly : public Shape {
public:
   Poly( Vec2D rotationPivot = Vec2D( 0.0, 0.0 ))
      : rotationPivot( rotationPivot ), outlineTexture( 0 ) {}

   // Construct the polygon from a list of vertices //
   Poly( const std::vector< Vec2D > &vertices, Vec2D rotationPivot = Vec2D( 0.0, 0.0 ))
      : vertices( vertices ), rotationPivot( rotationPivot ), outlineTexture( 0 ) {}

   // Construct the polygon from a list of vertices //
   Poly( const Vec2D *vertices, int numVertices, Vec2D rotationPivot = Vec2D( 0.0, 0.0 ));

   virtual ~Poly(){}

   // Adds a vertex to the polygon //
   inline void Add( Vec2D vec ) {
      vertices.push_back( vec );
   }

   // Returns a reference to the vertex with the given index //
   inline const Vec2D &GetVertex( unsigned int index ) const {
      static Vec2D dummyValue;

      if( index < 0 || index >= vertices.size()) {
         OlError( "Invalid vertex index: " + ToString( index )
            + " ( Number of vertices: " + ToString( vertices.size()) + " )" );
         return dummyValue;
      }

      return vertices[index];
   }


   inline Vec2D &GetVertex( unsigned int index ) {
      static Vec2D dummyValue;

      if( index < 0 || index >= vertices.size()) {
         OlError( "Invalid vertex index: " + ToString( index )
            + " ( Number of vertices: " + ToString( vertices.size()) + " )" );
         return dummyValue;
      }

      return vertices[index];
   }

   inline int GetNumberOfVertices() {
      return vertices.size();
   }

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

   // Tests if two polygons collide //
   inline bool Collides( const Poly &other, const Placement &thisPlacement,
                  const Placement &otherPlacement ) const {
      return DoCollisionTest( other, thisPlacement, otherPlacement, false ).IsCollision();
   }

   // Tests if two polygons collide and gives detailed information about the collision //
   inline Collision GetCollision( const Poly &other, const Placement &thisPlacement,
                  const Placement &otherPlacement ) const {
      return DoCollisionTest( other, thisPlacement, otherPlacement );
   }

   // Returns a constant reference to the list of the vertices //
   inline const std::vector< Vec2D > &GetVertices() const { return vertices; }

   // Moves the polygon by the specified amount //
   virtual void MoveBy( const Vec2D &amount );

   // Sets the rotation pivot //
   inline void SetPivot( Vec2D rotationPivot ) {
      this->rotationPivot = rotationPivot;
   }

   // Returns the rotation pivot //
   inline Vec2D GetPivot() {
      return rotationPivot;
   }

   // Sets the outline texture of the polygon, pass zero to get rid of the outline texture //
   inline void SetOutlineTexture( ol::Bitmap *texture, OutlineTextureMode mode = OPTIMIZE ) {
      outlineTexture = texture;
   }

protected:
   Collision DoCollisionTest( const Poly &other, const Placement &thisPlacement,
                  const Placement &otherPlacement, bool getResults = true ) const;

   // Draws the polygon to the active canvas //
   virtual void ExecDraw() const;

   // Draws the outline of the polygon to the active canvas //
   virtual void ExecDrawOutline() const;

   std::vector< Vec2D > vertices;
   Vec2D rotationPivot;
   Bitmap *outlineTexture;
   OutlineTextureMode outlineMode;
};



}


#endif // OL_POLYGON_HPP
