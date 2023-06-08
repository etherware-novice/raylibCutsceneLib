#include "cutscene.h"
#include "cutsceneData.h"
#include <raymath.h>

#define MAXCUTSCENEASSET 10


/* static const loadInfo cutsceneInfo[][MAXCUTSCENEASSET] = {
	{{ "scientist.png", 32, 32 }, { "icecream.png", 16, 16}},
	{{ "scientist.png", 32, 32 }}
}; */

static spritesheet loadedAssets[MAXCUTSCENEASSET];
static userAsset userAssetTable[MAXCUTSCENEASSET];

static unsigned loadedCount, cutsceneIndex;
static unsigned long cutsceneFrames, proccessedFrames;

static unsigned visibleFlags = ( 1 << MAXCUTSCENEASSET ) - 1;  // fills bits
static unsigned userVisibleFlags = 0;

static bool wasPaused = false;


static spritesheet loadFromInfo( loadInfo data )
{
	return loadNewSpritesheet( data.path, data.tileWidth, data.tileHeight );
}

void showAsset( unsigned id )
{
	unsigned bit = 1 << id;
	visibleFlags |= bit;
}

void hideAsset( unsigned id )
{
	unsigned bit = 1 << id;
	visibleFlags &= ~bit;
}

void showUserAsset( unsigned id )
{
	unsigned bit = 1 << id;
	userVisibleFlags |= bit;
}

void hideUserAsset( unsigned id )
{
	unsigned bit = 1 << id;
	userVisibleFlags &= ~bit;
}

static void loadUserData( Image *userDataArray, size_t entries )
{
	unsigned i;
	const Image missing = GenImageChecked( 32, 32, 4, 4, PURPLE, BLACK );
	Texture2D *target;
	for( i = 0; i < MAXCUTSCENEASSET; i++ )
	{
		target = &(userAssetTable[i].imageDisplay);

		if( i < entries )
			*target = LoadTextureFromImage( userDataArray[i] );
		else
			*target = LoadTextureFromImage( missing );

		userAssetTable[i].pos = (Vector2) {0, 0};
		userAssetTable[i].scale = 1;
	}
	UnloadImage(missing);
}

static void displayUserData( userAsset given )
{
	DrawTextureEx( given.imageDisplay, given.pos, 0, given.scale, WHITE );
}

void loadCutscene( unsigned index, Image *userDataArray, size_t entries )
{
	if( cutsceneIndex > 0 )
	{
		TraceLog(LOG_ERROR, TextFormat( 
					"CUTSCENE: Attempt to load cutscene %u while cutscene %u is running", 
					index, cutsceneIndex - 1 )
				);
		return;
	}

	TraceLog(LOG_INFO, TextFormat( "CUTSCENE: Loading cutscene assets for id %u", index));

	const loadInfo *currentlyLoading;
	loadedCount = 0;

	for( loadedCount = 0; loadedCount < MAXCUTSCENEASSET; loadedCount++ )
	{
		currentlyLoading = cutsceneInfo[index] + loadedCount;
		if( !currentlyLoading->path ) break;

		loadedAssets[loadedCount] = loadFromInfo(*currentlyLoading);
	}

	loadUserData( userDataArray, entries );

	cutsceneIndex = index + 1;
	cutsceneFrames = proccessedFrames = 0;
}

void drawCutscene(void)
{
	if( !cutsceneIndex ) return;

	unsigned long frameProgress = ( cutsceneFrames * 100.0 ) / FPS;
	if( frameProgress % 100 == 0 && !wasPaused )
		TraceLog(LOG_INFO, TextFormat( "CUTSCENE: Proccessing second %lu", frameProgress / 100 ));

	TraceLog( LOG_DEBUG, TextFormat( "CUTSCENE: Proccessing cutscene frames %lu - %lu", proccessedFrames, frameProgress ) );

	while( frameProgress > proccessedFrames )
	{
		interrupt customInt = cutsceneAct( cutsceneIndex - 1, proccessedFrames++, loadedAssets, userAssetTable );
		switch( customInt )
		{
			case STANDARD: wasPaused = false; break;

			case ERROR:
			TraceLog(LOG_ERROR, "CUTSCENE: Cutscene data flagged as error, force unloading..");
			unloadCutscene();
			return;
			
			case PAUSE:
			if( !wasPaused ) TraceLog(LOG_INFO, "CUTSCENE: Pausing cutscene in place"); 
			cutsceneFrames--;
			proccessedFrames--;
			frameProgress = 0;  // cant double break in c :(
			wasPaused = true;
			break;
		}

	}
	

	
	unsigned short i;
	for( i = 0; i < loadedCount; i++ )
		if( (1 << i) & visibleFlags )
			drawSpritesheet( loadedAssets[i] );

	for( i = 0; i < MAXCUTSCENEASSET; i++ )
		if( (1 << i) & userVisibleFlags )
			displayUserData( userAssetTable[i] );

	cutsceneFrames++;
}

void unloadCutscene(void)
{
	TraceLog(LOG_INFO, "CUTSCENE: Freeing cutscene assets");
	unsigned short i;
	for( i = 0; i < loadedCount; i++ )
	{
		freeTweenByPointer( &(loadedAssets[i].pos) );
		freeSpritesheetData( loadedAssets + i );
	}
	for( i = 0; i < MAXCUTSCENEASSET; i++ )
	{
		freeTweenByPointer( &(userAssetTable[i].pos) );
		UnloadTexture( userAssetTable[i].imageDisplay );
	}

	loadedCount = 0;
	cutsceneIndex = 0;
}

