#include <stdio.h>
#include "cutscene.h"


spritesheet loadNewSpritesheet( const char *filePath, unsigned tWidth, unsigned tHeight )
{

	TraceLog(LOG_INFO, TextFormat( "SPRITESHEET: Loading spritesheet from %s", filePath ));

	spritesheet newSpritesheet;

	newSpritesheet.fullSheet = LoadImage( filePath );
	newSpritesheet.tileWidth = tWidth;
	newSpritesheet.tileHeight = tHeight;
	
	newSpritesheet.pos = (Vector2) {0, 0};
	newSpritesheet.scale = 1;
	newSpritesheet.rotation = 0;
	newSpritesheet.flipped = 0;

	loadSpritesheetOffset( &newSpritesheet, 0 );
	return newSpritesheet;
}

void loadSpritesheetOffset( spritesheet *sheet, unsigned id )
{
	if( !sheet ) return;
	if( sheet->currentID == id ) return;
	sheet->currentID = id;

	TraceLog( LOG_INFO, TextFormat( 
				"SPRITESHEET: Loading offset %u from image %d x %d", 
				id, sheet->fullSheet.width, sheet->fullSheet.height 
				) );


	Image cutSprite;
	Rectangle slicer;
	unsigned x, y;
	id *= (sheet->tileWidth);

	x = id % (sheet->fullSheet.width);
	y = (id / (sheet->fullSheet.width)) * sheet->tileHeight;
	if( y + sheet->tileWidth  > sheet->fullSheet.height ) return;  // too far
								       //
	TraceLog( LOG_INFO, TextFormat( "SPRITESHEET: Loading pos %u (%u x %u) with size %u x %u", id, x, y, sheet->tileWidth, sheet->tileHeight ) );

	slicer = (Rectangle) { x, y, sheet->tileWidth, sheet->tileHeight };
	cutSprite = ImageFromImage( sheet->fullSheet, slicer );

	if( sheet->flipped & 1 ) ImageFlipHorizontal( &cutSprite );
	if( sheet->flipped & 2 ) ImageFlipVertical( &cutSprite );

	UnloadTexture(sheet->currentDisplay);
	sheet->currentDisplay = LoadTextureFromImage(cutSprite);

	UnloadImage(cutSprite);
}

void spritesheetFlipInPlace( spritesheet *sheet, unsigned flipped )
{
	if( flipped == sheet->flipped ) return;
	sheet->flipped = flipped;
	sheet->currentID++;
	loadSpritesheetOffset( sheet, sheet->currentID - 1 );
}

void drawSpritesheet( spritesheet sheet )
{
	DrawTextureEx( sheet.currentDisplay, sheet.pos, sheet.rotation, sheet.scale, WHITE );
}

void freeSpritesheetData( spritesheet *usedSheet )
{
	UnloadImage(usedSheet->fullSheet);
	UnloadTexture(usedSheet->currentDisplay);
}


/*
int main()
{
	InitWindow( 800, 450, "spritetest" );
	SetTargetFPS(60);

	spritesheet mainChar = loadNewSpritesheet( "icecream.png", 16, 16 );
	mainChar.pos = (Vector2) { 32, 32 };
	mainChar.scale = 5;
	unsigned offset = 0;

	while( !WindowShouldClose() )
	{
		if(IsKeyPressed(KEY_UP) && offset > 0 )
			offset--;
		if(IsKeyPressed(KEY_DOWN))
			offset++;

		loadSpritesheetOffset( &mainChar, offset );

		BeginDrawing();
		ClearBackground(RAYWHITE);

		DrawText( TextFormat( "%u", offset ), 0, 0, 20, DARKGRAY );
		drawSpritesheet( mainChar );

		EndDrawing();

	};

	freeSpritesheetData( &mainChar );
	CloseWindow();
}
*/
