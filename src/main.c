#include "cutscene.h"

#define SCREENWIDTH 800
#define SCREENHEIGHT 450
#define TITLE "cutscene"

int main(void)
{
	InitWindow( SCREENWIDTH, SCREENHEIGHT, TITLE );
	
	Image assets[] = { GenImageGradientRadial( 64, 64, 0, RED, BLUE ), 
	       GenImagePerlinNoise( 120, 40, 0, 0, 2 ) };

	SetTargetFPS(FPS);
	setDefaultTextbox( "image/icecream.png" );
	while( !WindowShouldClose() )
	{
		if(IsKeyPressed( KEY_SPACE ))
			loadCutscene(1, assets, ARR_SIZE(assets));

		proccessTweens();

		BeginDrawing();
		ClearBackground(RAYWHITE);

		drawCutscene();

		EndDrawing();
	};

	UnloadImage( assets[0] );
	UnloadImage( assets[1] );

	unloadTextboxTextures();
	CloseWindow();
	return 0;
}
