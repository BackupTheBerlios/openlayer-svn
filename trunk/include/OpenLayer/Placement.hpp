#ifndef OL_TRANSFORM_MATRIX_HPP
#define OL_TRANSFORM_MATRIX_HPP


#include "Matrix.hpp"
#include "Vec2D.hpp"


namespace ol {


class Placement {
public:
   Placement( Vec2D position = Vec2D( 0.0, 0.0 ),
              float rotation = 0.0,
              float stretch = 1.0 )          : position( position ), rotation( rotation ), stretch( stretch ) {}
   
   
   inline float GetDistance( const Placement &other ) const{
      return (position - other.position).GetMagnitude();
   }
   
   
   inline void MoveBy( Vec2D value ) {
      position += value;
   }
   
   
   inline void RotateBy( float value ) {
      rotation += value;
   }
   
   
   inline void StretchBy( float factor ) {
      stretch *= factor;
   }
   
   
   inline void SetPosition( Vec2D position ) {
      this->position = position;
   }
   
   
   inline Vec2D GetPosition() const {
      return position;
   }
   
   
   inline void SetRotation( float rotation ) {
      this->rotation = rotation;
   }
   
   
   inline float GetRotation() const {
      return rotation;
   }
   
   
   inline void SetStretch( float stretch ) {
      this->stretch = stretch;
   }
   
   
   inline float GetStretch() const {
      return stretch;
   }   
   
   // Returns the rotation and stretch in a matrix //
   Matrix2D Get2DMatrix() const;
   
   
private:
   Vec2D position;
   float rotation;
   float stretch;
};



}



#endif // OL_TRANSFORM_MATRIX_HPP
