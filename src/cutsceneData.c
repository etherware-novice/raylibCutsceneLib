#include "cutscene.h"
#include "cutsceneData.h"
#include <raymath.h>


const loadInfo cutsceneInfo[][MAXCUTSCENEASSET] = {
	{{ "image/scientist.png", 32, 32 }, { "image/icecream.png", 16, 16}},
	{{ "image/scientist.png", 32, 32 }}
};

interrupt cutsceneAct(unsigned id, unsigned frame, spritesheet *staticAssets, userAsset *userAssetTable)
{
	interrupt returnCode = STANDARD;
	switch( id )
	{
		case 0:  // cutscene 0
		switch( frame )
		{
			case 0:
			staticAssets[0].pos = (Vector2) { 32, 32 };
			staticAssets[0].scale = 2;
			staticAssets[0].flipped = 1;
			loadSpritesheetOffset( staticAssets, 2 );

		        staticAssets[1].pos = (Vector2) { 100, 32 };
			staticAssets[1].scale = 4;
			loadSpritesheetOffset( staticAssets + 1, 16 );
			break;

			case 500:

			case 700:
			staticAssets[1].flipped = 2;
			loadSpritesheetOffset( staticAssets + 1, 2 );
			break;

			case 800:
			spritesheetFlipInPlace( staticAssets + 1, 3 );
			break;
			
			case 1000:
			spritesheetFlipInPlace( staticAssets + 1, 0 );
			break;

			case 1200:
			startVectorTween(
					&(staticAssets[0].pos),
					(Vector2) {-100, -100}, 4 );
			break;

			case 1401:
			startVectorTween( 
					&(staticAssets[1].pos), 
					(Vector2) {800, 400}, 1 );
			hideAsset(1);
			break;

			case 1420:
			showAsset(1);
			break;

			case 1600:
			unloadCutscene();
			break;
		};
		
		// each second, repeating
		switch( frame / 100 )
		{
			case 5:
			if( frame % 30 > 15 )
				loadSpritesheetOffset( staticAssets, 4 );
			else
				loadSpritesheetOffset( staticAssets, 1 );
			break;

			case 12:
			case 13:
			if( frame % 30 > 15 )
			{
				loadSpritesheetOffset( staticAssets, 1 );
				loadSpritesheetOffset( staticAssets + 1, 2 );
			}
			else
			{
				loadSpritesheetOffset( staticAssets, 3 );
				loadSpritesheetOffset( staticAssets + 1, 3 );
			}

			break;

			/*
			case 15:
			double abcde = ( proccessedFrames % 100 ) / (float) FPS;
			loadedAssets[1].pos = Vector2Lerp( 
				(Vector2) { 100, 32 }, (Vector2) { 700, 300 }, abcde );
			break;
			*/

		}
		break;

		case 1:
		switch( frame )
		{
			case 0:
			pauseAllTweens( false );

			showUserAsset(0);
			userAssetTable[0].pos = (Vector2) { 100, 3 };
			startVectorTween(
				&(userAssetTable[0].pos),
				(Vector2) {0, 250}, 10
				);

			showUserAsset(1);
			userAssetTable[1].pos = (Vector2) { 80, 64 };
			startVectorTween(
				&(userAssetTable[1].pos),
				(Vector2) {0, 250}, 10
				);

			showUserAsset(2);
			startVectorTween(
				&(userAssetTable[2].pos),
				(Vector2) {0, 250}, 10
				);

			showUserAsset(3);

			staticAssets[0].pos = (Vector2) {32, 32};
			loadSpritesheetOffset( staticAssets, 3 );
			startVectorTween(
				&(staticAssets[0].pos),
				(Vector2) {0, 250}, 10
				);

			break;

			case 400:
			pauseTweenByPointer( &(userAssetTable[0].pos ), true );
			break;

			case 497:
			pauseTweenByPointer( &(userAssetTable[1].pos ), true );
			break;

			case 498:
			displayTextbox( "TESTT TEST TEST WOW AMAZING", 5, BLACK, NULL );
			if(!IsKeyDown( KEY_E ))
				returnCode = PAUSE;
			break;

			case 499:
			resetVisibleTextboxText();
			pauseTweenByPointer( &(userAssetTable[1].pos ), false );
			break;

			case 550:
			pauseTweenByPointer( &(userAssetTable[0].pos ), false );
			break;

			case 600:
			pauseAllTweens( true );
			break;

			case 700:
			pauseAllTweens( false );
			break;

			case 900:
			hideUserAsset(0);
			hideUserAsset(1);
			hideUserAsset(2);
			hideUserAsset(3);
			loadSpritesheetOffset( staticAssets, 4 );
			break;

			case 1200:
			unloadCutscene();
			break;
		}
		break;

	}

	return returnCode;
}
