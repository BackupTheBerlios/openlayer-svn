#include "Polygon.hpp"
#include "Bitmap.hpp"
#include "Line.hpp"

using namespace std;
using namespace ol;

static const float OL_NEAR_ZERO = 0.000001;

ol::Poly::
Poly( const Vec2D *vertices, int numVertices, Vec2D rotationPivot )
   : rotationPivot( rotationPivot ), outlineTexture( 0 ) {
   for( int i = 0; i < numVertices; i++ ) {
      this->vertices.push_back( vertices[i] );
   }
}


void ol::Poly::
ExecDrawOutline() const {
   if( lineWidth <= 1.0 + OL_NEAR_ZERO ) {
      glBegin( GL_LINE_LOOP );

         for( std::vector< Vec2D > ::const_iterator iter = vertices.begin(); iter != vertices.end(); iter++ ) {
            glVertex2f( iter->x, iter->y );
         }

      glEnd();
   }
   else {
      float texturePos = 0.0;

      // Find the normal of the beginning of the strip //

      Vec2D last = vertices.back();

      Vec2D lastS = vertices.front() - last;
      lastS /= lastS.GetMagnitude();

      // Render the begin of the strip //

      if( outlineTexture ) {
         outlineTexture->Select();
         glEnable( GL_TEXTURE_2D );
         glBegin( GL_QUAD_STRIP );
      }
      else {
         glBegin( GL_QUAD_STRIP );
      }

         bool endNow = false;
         bool endNext = false;
         bool isFirst = true;

         vector< Vec2D > ::const_iterator vertexIter = vertices.begin();
         while( !endNow ) {
            if( endNext ) {
               endNow = true;
            }

            float x = vertexIter->x;
            float y = vertexIter->y;

            // Find the direction vector from the next point to the current one //
            vertexIter++;

            if( x == last.x && y == last.y ) {
               continue;
            }

            if( vertexIter == vertices.end()) {
               vertexIter = vertices.begin();
               endNext = true;
            }

            float bx = x - vertexIter->x;
            float by = y - vertexIter->y;

            float bLength = sqrt( bx * bx + by * by );

            if( bLength > -OL_NEAR_ZERO && bLength < OL_NEAR_ZERO ) {
               continue;
            }

            bx /= bLength;
            by /= bLength;

            // Find the direction vector of the displacement //

            float cx = lastS.x + bx;
            float cy = lastS.y + by;

            float cLength = sqrt( cx * cx + cy * cy );

            float nx, ny;

            if( cLength > -OL_NEAR_ZERO && cLength < OL_NEAR_ZERO ) {
               nx = -by;
               ny = bx;

               float nRatio = lineWidth/sqrt( nx * nx + ny * ny );

               nx *= nRatio;
               ny *= nRatio;
            }
            else {
               cx /= cLength;
               cy /= cLength;

               // Make sure that the displacement happens always in the same side //

               float diff1 = lastS.x - bx;
               float diff2 = lastS.y - by;
               float diff3 = cx - bx;
               float diff4 = cy - by;

               if(( diff1 * diff4 ) - ( diff2 * diff3 ) > 0) {
                  cx = -cx;
                  cy = -cy;
               }

               // Find the displacement multiplicator //

               float s = lastS.y * cx + (-lastS.x) * cy;

               nx = cx * lineWidth / s;
               ny = cy * lineWidth / s;
            }

            // Find the displaced coordinates //

            float upperX = x + nx;
            float upperY = y + ny;

            float lowerX = x - nx;
            float lowerY = y - ny;

            if( !outlineTexture ) {
               glVertex2f( upperX, upperY );
               glVertex2f( lowerX, lowerY );
            }
            else {
               OlTextureInfo &textureInfo = outlineTexture->textureInfo;

               texturePos += bLength / textureInfo.imgWidth;

               if( texturePos > textureInfo.rect.w ) {
                  texturePos = 0.0;
               }

               if( !isFirst ) {
                  glTexCoord2f( textureInfo.rect.x + texturePos, textureInfo.rect.y );
                  glVertex2f( upperX, upperY );
                  glTexCoord2f( textureInfo.rect.x + texturePos, textureInfo.rect.y + textureInfo.rect.h );
                  glVertex2f( lowerX, lowerY );
               }
               else {
                  isFirst = false;
               }

               if( !endNow ) {
                  glTexCoord2f( textureInfo.rect.x + texturePos, textureInfo.rect.y );
                  glVertex2f( upperX, upperY );
                  glTexCoord2f( textureInfo.rect.x + texturePos, textureInfo.rect.y + textureInfo.rect.h );
                  glVertex2f( lowerX, lowerY );
               }
            }

            // Store the information which can be used when calculating the next point //

            last.x = x;
            last.y = y;

            lastS.x = -bx;
            lastS.y = -by;
         }

      glEnd();
   }
}


void ol::Poly::
ExecDraw() const {
   glBegin( GL_POLYGON );

      for( std::vector< Vec2D > ::const_iterator iter = vertices.begin(); iter != vertices.end(); iter++ ) {
         glVertex2f( iter->x, iter->y );
      }

   glEnd();
}


void ol::Poly::
MoveBy( const Vec2D &amount ) {
   for( std::vector< Vec2D > ::iterator iter = vertices.begin(); iter != vertices.end(); iter++ ) {
      *iter += amount;
   }
}


Collision ol::Poly::
DoCollisionTest( const ol::Poly &other, const Placement &thisPlacement,
                 const Placement &otherPlacement, bool getResults ) const {
   if( vertices.size() < 2 || other.vertices.size() < 2 ) {
      OlError( "An empty Poly can't ever collide!" );
      return Collision( false );
   }

   Vec2D thisCo = thisPlacement.GetPosition();
   Vec2D otherCo = otherPlacement.GetPosition();

   Matrix2D thisTransform = thisPlacement.Get2DMatrix();
   Matrix2D otherTransform = otherPlacement.Get2DMatrix();

   std::vector< Vec2D > ::const_iterator thisIter = vertices.begin();

   Vec2D thisPrev = thisTransform.Transform( *thisIter - rotationPivot ) + thisCo + rotationPivot;
   thisIter++;

   // Loop through each vertex //
   while( true ) {
      bool breakNow = false;

      // Test if we've reached the last line segment //
      if( thisIter == vertices.end() ) {
         breakNow = true;
         thisIter = vertices.begin();
      }

      Vec2D thisVertex = thisTransform.Transform( *thisIter - rotationPivot ) + thisCo + rotationPivot;
      thisIter++;

      std::vector< Vec2D > ::const_iterator otherIter = other.vertices.begin();
      Vec2D otherPrev = otherTransform.Transform( *otherIter - other.rotationPivot ) + otherCo + other.rotationPivot;
      otherIter++;

      // Loop through each vertex of the other polygon //
      while( true ) {
         bool breakNow = false;

         // Test if we've reached the last line segment of the other polygon //
         if( otherIter == other.vertices.end() ) {
            breakNow = true;
            otherIter = other.vertices.begin();
         }

         Vec2D otherVertex = otherTransform.Transform( *otherIter - other.rotationPivot ) + otherCo + other.rotationPivot;
         otherIter++;

         // Test for collision //
         if( IsCounterClockwise( thisPrev, thisVertex, otherPrev )
             != IsCounterClockwise( thisPrev, thisVertex, otherVertex )
             &&
             IsCounterClockwise( otherPrev, otherVertex, thisPrev )
             != IsCounterClockwise( otherPrev, otherVertex, thisVertex )) {

            if( !getResults ) {
               return Collision( true );
            }
            else {
               Line thisLine( thisVertex, thisPrev );
               Line otherLine( otherVertex, otherPrev );

               return Collision( thisLine, otherLine );
            }
         }

         // Is last line segment of the other polygon processed? //
         if( breakNow ) {
            break;
         }

         // Advance to the next vertex of the other polygon //
         otherPrev = otherVertex;
      }

      // Is last line segment processed? //
      if( breakNow ) {
         break;
      }

      // Advance to the next vertex //
      thisPrev = thisVertex;
   }

   return Collision( false );
}

