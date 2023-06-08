#ifndef CUTSCENEDATA_H
#define CUTSCENEDATA_H

#define MAXCUTSCENEASSET 10

typedef struct {
	const char *path;
	unsigned tileWidth;
	unsigned tileHeight;
} loadInfo;

typedef struct {
	Texture2D imageDisplay;
	Vector2 pos;
	float scale;
} userAsset;

typedef enum {
	STANDARD,
	ERROR,
	PAUSE
} interrupt;

// cutscene.c
void showAsset( unsigned id );
void hideAsset( unsigned id );
void showUserAsset( unsigned id );
void hideUserAsset( unsigned id );

// user provided data
extern const loadInfo cutsceneInfo[][MAXCUTSCENEASSET];
interrupt cutsceneAct(unsigned id, unsigned frame, spritesheet *staticAssets, userAsset *assetTable);

#endif
