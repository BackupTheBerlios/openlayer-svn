/***
OpenLayer Collision Demo
---
Purpose:    This demo demonstrates polygon collision detection in OpenLayer
Created:		2006/02/16
Author: 		Brady Eidson

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
***/

#include <OpenLayer.hpp>

// A helper function to return a random number between min and max //
float FRand( float min, float max ) {
   return min + float( rand() )/float( RAND_MAX ) * ( max - min );
}

using namespace ol;


class Rectagon : public Poly
{

public:
	Rectagon( const Vec2D& topright, const Vec2D& _size )
		: tr( topright ), bl( topright + _size )
	{
		Add( tr );
		Add( Vec2D( bl.x, tr.y ));
		Add( bl );
		Add( Vec2D( tr.x, bl.y ));
	}

	bool Contains( Poly& p )
	{
		unsigned int i;
		const std::vector< Vec2D > v = p.GetVertices();
		for( i = 0; i < v.size(); i++ )
		{
			if( v[i].x > bl.x || v[i].x < tr.x ||
			    v[i].y > bl.y || v[i].y < tr.y )
				return false;
		}
		return true;
	}


private:

	Vec2D tr;
	Vec2D bl;
};




class MotionPoly : public Poly
{

public:
	MotionPoly(  )
	{}

	void UpdatePosition( float deltaTime )
	{
		MoveBy( mVelocity * deltaTime );
		//mPosition += ( mVelocity * deltaTime );
		/*

		if( mPosition .x < 0 )
		{
			mVelocity.x *= -1;
			mPosition .x = 0 - mPosition .x;
		}
		if( mPosition .x >= SCREEN_W )
		{
			mVelocity.x *= -1;
			mPosition .x = SCREEN_W - ( mPosition .x - SCREEN_W );
		}

		if( mPosition .y < 0 )
		{
			mVelocity.y *= -1;
			mPosition .y = 0 - mPosition .y;
		}
		if( mPosition .y >= SCREEN_H )
		{
			mPosition .y = SCREEN_H - ( mPosition .y - SCREEN_H );
			mVelocity.y *= -1;
		}
		*/
	}

	void Draw()
	{
		Poly::Draw( Rgba::WHITE );
	}

	Placement GetPlacement()
	{
		return Placement();
	}

	Vec2D		mVelocity;
	Vec2D		mPosition;

};




int main() {
   // SETUP //

   // Set up the program with all possible drivers //
   Setup::SetupProgram();
	printf( "Setup\n" );

   // Set up the screen in windowed mode with the window size of 800 x 600 //
   Setup::SetupScreen( 800, 600, WINDOWED );
	printf( "SetupScreen\n" );


	MotionPoly player;
	player.Add( Vec2D( 0.0, 0.0 ));
	player.Add( Vec2D( 20.0, 0.0 ));
	player.Add( Vec2D( 25.0, 10.0 ));
	player.Add( Vec2D( 20.0, 20.0 ));
	player.Add( Vec2D( 0.0, 20.0 ));



	player.mVelocity.x = 1.0;
	player.mVelocity.y = 1.0;

	MotionPoly wall;
	wall.Add( Vec2D( 0.0, 0.0 ));
	wall.Add( Vec2D( SCREEN_W, 0.0 ));
	wall.Add( Vec2D( SCREEN_W, SCREEN_H ));
	wall.Add( Vec2D( 0.0, SCREEN_H ));


	MotionPoly enemy;
	enemy.Add( Vec2D( 15, 0 ));
	enemy.Add( Vec2D( 30, 10 ));
	enemy.Add( Vec2D( 25, 15 ));
	enemy.Add( Vec2D( 30, 20 ));
	enemy.Add( Vec2D( 15, 30 ));
	enemy.Add( Vec2D( 0, 20 ));
	enemy.Add( Vec2D( 5, 15 ));
	enemy.Add( Vec2D( 0, 10 ));
	enemy.mPosition = Vec2D( 700,500 );
	enemy.MoveBy( Vec2D( 700,500 ));

	//enemy.speed.x = -2.0;
	//enemy.speed.y = -1.5;


   // Set up the delta time routines with the default fps of 100.0 //
	const float FRAMERATE = 100.0;
   FpsCounter::Start( FRAMERATE );

	bool collisionzoom = false;
	Collision collision;


	Rect testr( Vec2D( 100, 100 ), Vec2D( 200, 100 ), 1.0, 70 );

	Rectagon rr( Vec2D( 100, 100 ), Vec2D( 200, 100 ));

   // Run the game loop until esc is pressed //
   while( !key[KEY_ESC] ) {
      // LOGIC //

		if( collisionzoom )
		{
			FpsCounter::Pause();
			while( !key[KEY_SPACE] )
			{
				Transforms::SetPosition( collision.GetPoint());
				Transforms::SetStretch( 2.0, 2.0 );

				Canvas::Fill( Rgba::BLACK );
				player.Draw();
				enemy.Draw();
				Canvas::Refresh();
				rest(1);
			}
			//Transforms::ResetTransforms();
			Transforms::SetPosition( Vec2D( 0.0, 0.0 ));
			Transforms::SetStretch( 1.0, 1.0 );
			collisionzoom = false;
			FpsCounter::Resume();
		}


      // Inform the fps counter that a new frame has started //
      FpsCounter::NewFrameStarted();

      // Get the delta time //
      float deltaTime = FpsCounter::GetDeltaTime();

      // Move the bitmap with the arrow keys //


      if( key[KEY_LEFT] ) player.mVelocity.x -= 4 * deltaTime / FRAMERATE;
      if( key[KEY_RIGHT] ) player.mVelocity.x += 4 * deltaTime / FRAMERATE;
      if( key[KEY_UP] ) player.mVelocity.y -= 4 * deltaTime / FRAMERATE;
      if( key[KEY_DOWN] ) player.mVelocity.y += 4 * deltaTime / FRAMERATE;

		float xAccel, yAccel;

		/*xAccel = FRand( -.1, .1 );
		yAccel = FRand( -.1, .1 );
		player.mVelocity.x += xAccel * deltaTime;
		player.mVelocity.y += yAccel * deltaTime;*/
		player.UpdatePosition( deltaTime );



		xAccel = FRand( -.1, .1 );
		yAccel = FRand( -.1, .1 );
		enemy.mVelocity.x += xAccel * deltaTime;
		enemy.mVelocity.y += yAccel * deltaTime;
		enemy.UpdatePosition( deltaTime );



		collision = player.GetCollision( wall, player.GetPlacement(), wall.GetPlacement());
		if( collision.IsCollision())
		{
			Vec2D surfaceNormal = collision.GetSegment( OBJ_B ).GetNormal();
			float speedAgainstSurface = -player.mVelocity * surfaceNormal;
			player.mVelocity += 2 * speedAgainstSurface * surfaceNormal;
		}

		collision = enemy.GetCollision( wall, enemy.GetPlacement(), wall.GetPlacement());
		if( collision.IsCollision())
		{
			Vec2D surfaceNormal = collision.GetSegment( OBJ_B ).GetNormal();
			float speedAgainstSurface = -enemy.mVelocity * surfaceNormal;
			enemy.mVelocity += 2 * speedAgainstSurface * surfaceNormal;
		}

		collision = player.GetCollision( enemy, player.GetPlacement(), enemy.GetPlacement());
		if( collision.IsCollision())
		{
			Vec2D surfaceNormal = collision.GetSegment( OBJ_B ).GetNormal();
			float speedAgainstSurface = -player.mVelocity * surfaceNormal;
			player.mVelocity += 2 * speedAgainstSurface * surfaceNormal;

			surfaceNormal = collision.GetSegment( OBJ_A ).GetNormal();
			speedAgainstSurface = -enemy.mVelocity * surfaceNormal;
			enemy.mVelocity += 2 * speedAgainstSurface * surfaceNormal;
		}

/*
		collision = player.GetCollision( enemy, player.GetPlacement(), enemy.GetPlacement());

		if( collision.IsCollision())
		{
			player.mVelocity.x *= -1;
			player.mVelocity.y *= -1;
			enemy.mVelocity.x *= -1;
			enemy.mVelocity.y *= -1;
			printf( "Collision at %f,%f\n", collision.GetPoint().x, collision.GetPoint().y );
			collisionzoom = true;
		}
*/
      // RENDERING //

      // Clear the screen to white //
      Canvas::Fill( Rgba::BLACK );

      // Draw the bitmap to the screen with the top-left coordinates //
		player.Draw();
		enemy.Draw();
		//testr.Draw( Rgba::GREEN );
      rr.Draw( Rgba::RED );

      if( rr.Contains( player ))
      {
			printf( "Player is inside rectangle\n" );
      }

      // Refresh the screen contents to show this frame //
      Canvas::Refresh();
      rest(1);
   }
   return 0;
}
END_OF_MAIN()


