#ifndef _CUTSCENE_H
#define _CUTSCENE_H

#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>

#define FPS 60
#define CACHESIZE 5

typedef struct {
	void *cachedData[CACHESIZE];    // the data thats cached, should be casted obviously
	unsigned usageData[CACHESIZE];  // should be used to control which entry is freed when needed
	int cacheAssocData[CACHESIZE];  // generic info field for associated cache data
} cacheInfo;  // i reccomend mallocing this as needed instead of using directly

typedef struct {
	Image fullSheet;
	Texture2D *currentDisplay;
	unsigned currentID;
	unsigned flipped;
	unsigned tileWidth;
	unsigned tileHeight;

	Vector2 pos;
	int scale;
	float rotation;

	cacheInfo *cache;
} spritesheet;


#define ARR_SIZE(arr) ( sizeof((arr)) / sizeof((arr[0])) )
#define VECTOR2ZERO (Vector2) {0, 0}

// sprite.c
spritesheet loadNewSpritesheet( const char *filePath, unsigned tWidth, unsigned tHeight );
void loadSpritesheetOffset( spritesheet *sheet, unsigned id );
void spritesheetFlipInPlace( spritesheet *sheet, unsigned flipped );
void drawSpritesheet( spritesheet sheet );
void freeSpritesheetData( spritesheet *usedSheet );

// cutscene.c
void loadCutscene( unsigned index, Image *userImageArray, size_t entries );
void drawCutscene(void);
void unloadCutscene(void);

// tween.c
void startVectorTween( Vector2 *target, Vector2 endPos, float duration );
void proccessTweens( void );
void pauseTweenByPointer( Vector2 *search, bool newPause );
void pauseAllTweens( bool newPause );
void freeTweenByPointer( Vector2 *search );

// textbox.c
extern Rectangle textBoxLocation;
void setDefaultTextbox( const char *path );
void displayTextbox( const char *text, float speed, Color textcolor, const char *customBox );
void resetVisibleTextboxText(void);
void unloadTextboxTextures(void);

// wordWrap.c
void DrawTextBoxed(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint);
void DrawTextBoxedSelectable(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint);


#endif
