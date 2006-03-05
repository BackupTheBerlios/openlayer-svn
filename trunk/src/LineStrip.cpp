#define DEBUGMODE
#include "LineStrip.hpp"
#include "Bitmap.hpp"
#include "VertexListCollision.hpp"

static const float OL_NEAR_ZERO = 0.0001;

using namespace std;
using namespace ol;


Collision LineStrip::
DoCollisionTest( const ol::LineStrip &other, const Placement &thisPlacement,
                 const Placement &otherPlacement, bool getResults ) const {
   return LineStripCollision( vertices, other.GetVertices(), thisPlacement,
                              otherPlacement, getResults, false, false );
}



void LineStrip::
ExecDraw() const {
   Render( 0, 0 );
}


void LineStrip::
Draw( const Rgba &startColor, const Rgba &endColor ) const {
   Render( &startColor, &endColor );
}


void LineStrip::
Render( const Rgba *color1, const Rgba *color2 ) const {
   if( GetNumOfVertices() < 2 ) {
      return;
   }
   
#ifdef OL_NO_STATE_CHANGE
   GLboolean texturesEnabled;
   glGetBooleanv( GL_TEXTURE_2D, &texturesEnabled );
#endif
   if( texture ) {
      glEnable( GL_TEXTURE_2D );
      texture->Select();
   }
   else {
      glDisable( GL_TEXTURE_2D );
   }
   
   // The sum of lengths thus far //
   float sumLengths = 0.0;
   float texturePos = 0.0;
   
   list< float > ::const_iterator lengthIter = lengths.begin();
   
   // Find the normal of the beginning of the strip //
   
   Vec2D last = vertices.front();
   Vec2D lastS = GetVertex( 1 ) - last;
   lastS /= lastS.GetMagnitude();
   
   float lastUpperX = last.x + lastS.y * lineWidth;
   float lastUpperY = last.y - lastS.x * lineWidth;
   
   float lastLowerX = last.x - lastS.y * lineWidth;
   float lastLowerY = last.y + lastS.x * lineWidth;
   
   glPushMatrix();
   placement.Apply();
   
   // Render the begin of the strip //
   
   glBegin( GL_QUAD_STRIP );
      if( color1 ) {
         color1->Select();
      }
      
      glVertex2f( lastUpperX, lastUpperY );
      glVertex2f( lastLowerX, lastLowerY );
      
      list< Vec2D > ::const_iterator vertexIter = vertices.begin();
      vertexIter++;
      
      while( true ) {
         if( color1 ) {
            float factor = sumLengths / totalLength;
            sumLengths += *lengthIter;
            
            color1->InterpolateWith( *color2, factor ).Select();
         }
         
         float x = vertexIter->x;
         float y = vertexIter->y;
         
         vertexIter++;
         
         if( vertexIter == vertices.end()) {
            break;
         }
         
         if( fabs( x - last.x ) < OL_NEAR_ZERO && fabs( y - last.y ) < OL_NEAR_ZERO ) {
            continue;
         }
         
         // Find the direction vector from the next point to the current one //
         
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
            
            if(( diff1 * diff4 ) - ( diff2 * diff3 ) > 0 ) {
               cx = -cx;
               cy = -cy;
            }
            
            // Find the displacement multiplicator //
            
            float s = lastS.y * cx + (-lastS.x) * cy;
            
            if( fabs( s ) < OL_NEAR_ZERO ) {
               nx = -by;
               ny = bx;
               
               float nRatio = lineWidth/sqrt( nx * nx + ny * ny );
               
               nx *= nRatio;
               ny *= nRatio;
            }
            else {
               nx = cx * lineWidth / s;
               ny = cy * lineWidth / s;
            }
         }
         
         // Find the displaced coordinates //
         
         float upperX = x + nx;
         float upperY = y + ny;
         
         float lowerX = x - nx;
         float lowerY = y - ny;
         
         if( !texture ) {
            glVertex2f( upperX, upperY );
            glVertex2f( lowerX, lowerY );
         }
         else {
            const OlTextureInfo &textureInfo = texture->textureInfo;
            
            texturePos += *lengthIter / textureInfo.imgWidth;
            
            if( texturePos > textureInfo.rect.w ) {
               texturePos = 0.0;
            }
            
            glTexCoord2f( textureInfo.rect.x + texturePos, textureInfo.rect.y );
            glVertex2f( upperX, upperY );
            glTexCoord2f( textureInfo.rect.x + texturePos, textureInfo.rect.y + textureInfo.rect.h );
            glVertex2f( lowerX, lowerY );
            
            list< Vec2D > ::const_iterator nextIter = vertexIter;
            nextIter++;
            if( nextIter != vertices.end() ) {
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
         
         lastUpperX = upperX;
         lastUpperY = upperY;
         
         lastLowerX = lowerX;
         lastLowerY = lowerY;
         
         lengthIter++;
      }
      
      // Render the end of the strip //
      
      float x = vertices.back().x;
      float y = vertices.back().y;
      
      float upperX = x + lastS.y * lineWidth;
      float upperY = y - lastS.x * lineWidth;
      
      float lowerX = x - lastS.y * lineWidth;
      float lowerY = y + lastS.x * lineWidth;
      
      if( color2 )
         color2->Select();
      
      glVertex2f( upperX, upperY );
      glVertex2f( lowerX, lowerY );
   
   glEnd();
   
   glPopMatrix();
   
#ifdef OL_NO_STATE_CHANGE
   if( texturesEnabled ) 
      glEnable( GL_TEXTURE_2D );
   else
      glDisable( GL_TEXTURE_2D );
#endif
}


Vec2D LineStrip::
GetVertex( int index ) const {
   if( index >= (signed)vertices.size()) {
      OlError( "Tried to retrieve vertex #" + ToString( index ) + " from a LineStrip with only "
               + ToString( vertices.size() ) + " vertices!" );
      
      return Vec2D( 0.0, 0.0 );
   }
   
   list< Vec2D > ::const_iterator iter = vertices.begin();
   
   for( int i = 0; i < index && iter != vertices.end(); i++ ) {
      iter++;
   }
   
   return *iter;
}


