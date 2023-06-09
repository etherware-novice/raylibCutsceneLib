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

	newSpritesheet.cache = malloc(sizeof(cacheInfo));
	memset( newSpritesheet.cache, 0, sizeof(cacheInfo) );


	loadSpritesheetOffset( &newSpritesheet, 0 );
	return newSpritesheet;
}

static Texture2D generateSpritesheetOffsetTexture( spritesheet *sheet, unsigned id )
{
	Image cutSprite;
	Texture2D returnedSprite;
	Rectangle slicer;
	unsigned x, y;
	id *= (sheet->tileWidth);

	x = id % (sheet->fullSheet.width);
	y = (id / (sheet->fullSheet.width)) * sheet->tileHeight;
	if( y + sheet->tileWidth  > sheet->fullSheet.height ) y = sheet->fullSheet.height - sheet->tileHeight;  // too far

	TraceLog( LOG_INFO, TextFormat( "SPRITESHEET: Loading pos %u (%u x %u) with size %u x %u", id, x, y, sheet->tileWidth, sheet->tileHeight ) );

	slicer = (Rectangle) { x, y, sheet->tileWidth, sheet->tileHeight };
	cutSprite = ImageFromImage( sheet->fullSheet, slicer );

	if( sheet->flipped & 1 ) ImageFlipHorizontal( &cutSprite );
	if( sheet->flipped & 2 ) ImageFlipVertical( &cutSprite );

	returnedSprite = LoadTextureFromImage(cutSprite);
	UnloadImage(cutSprite);
	return returnedSprite;
}

static Texture2D *cachedSpritesheetOffset( spritesheet *sheet, unsigned id )
{
	unsigned i, freedId, lowestUsage;
	cacheInfo *cacheTable = sheet->cache;
	Texture2D **fillSpot = NULL;
	
	for( i = 0; i < CACHESIZE; i++ )
	{
		if( cacheTable->cacheAssocData[i] == id ) 
		{
			cacheTable->usageData[i]++;
			return cacheTable->cachedData[i];
		}
		if( cacheTable->usageData[i] > lowestUsage )
		{
			freedId = i;
			lowestUsage = cacheTable->usageData[i];
			fillSpot = cacheTable->cachedData + i;
		}
	}
	
	if( *fillSpot != NULL )
	{
		TraceLog(LOG_INFO, TextFormat( "SPRITESHEET: Unloading offset %d from cache", cacheTable->cacheAssocData[freedId] ));
		UnloadTexture( **fillSpot );
		free( *fillSpot );
	}
	else
	{
		*fillSpot = malloc(sizeof(Texture2D));
	}
	**fillSpot = generateSpritesheetOffsetTexture( sheet, id );
	cacheTable->cacheAssocData[freedId] = id;
	cacheTable->usageData[freedId]++;

	return *fillSpot;
}

void loadSpritesheetOffset( spritesheet *sheet, unsigned id )
{
	Texture2D *newTexture = NULL;

	if( !sheet ) return;
	if( sheet->currentID == id ) return;
	sheet->currentID = id;

	TraceLog( LOG_INFO, TextFormat( 
				"SPRITESHEET: Loading offset %u from image %d x %d", 
				id, sheet->fullSheet.width, sheet->fullSheet.height 
				) );


	newTexture = cachedSpritesheetOffset( sheet, id );
	if( newTexture ) sheet->currentDisplay = newTexture;
}

void spritesheetFlip( spritesheet *sheet, unsigned flipped )
{
	if( flipped == sheet->flipped ) return;
	sheet->flipped = flipped;
	sheet->currentID++;

	freeSpritesheetCache( sheet );
	loadSpritesheetOffset( sheet, sheet->currentID - 1 );
}

void drawSpritesheet( spritesheet sheet )
{
	if( sheet.currentDisplay == NULL ) return;
	DrawTextureEx( *(sheet.currentDisplay), sheet.pos, sheet.rotation, sheet.scale, WHITE );
}

void freeSpritesheetCache( spritesheet *sheet )
{
	TraceLog( LOG_INFO, TextFormat("SPRITESHEET: Clearing cache for spritesheet %p", sheet) );
	
	unsigned i;
	cacheInfo *cacheTable = sheet->cache;
	Texture2D **currentCacheIndex = NULL;

	if( cacheTable == NULL ) return;

	for( i = 0; i < CACHESIZE; i++ )
	{
		currentCacheIndex = cacheTable->cachedData + i;
		if( *currentCacheIndex == NULL ) continue;
		UnloadTexture(**currentCacheIndex);
		free(*currentCacheIndex);
		*currentCacheIndex = NULL;

		cacheTable->usageData[i] = 0;
	}

}

void freeSpritesheetData( spritesheet *usedSheet )
{
	freeSpritesheetCache( usedSheet );
	UnloadImage(usedSheet->fullSheet);
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

