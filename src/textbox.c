#include "cutscene.h"
#include <string.h>

#define TEXTFONT GetFontDefault()
#define TEXTPADDING 20

Rectangle textBoxLocation = { 100, 100, 500, 200 };

static Texture2D textBoxBack, cachedBoxBack;
static const char *defaultTexturePath, *cachedTexturePath = NULL;
static float textProgress = 0;
void unloadTextboxTextures(void);


static Texture2D updateCache( const char *cachedPath )
{
	if(!IsTextureReady( textBoxBack ))
	{
		TraceLog( LOG_INFO, "TEXTBOX: Re-loading default textbox texture" );
		UnloadTexture(textBoxBack);
		if( !defaultTexturePath )
		{
			TraceLog(LOG_ERROR, "TEXTBOX: No default textbox texure specified, generating..");
			Image checkerboard = GenImageChecked( 128, 128, 16, 16, PURPLE, BLACK );
			textBoxBack = LoadTextureFromImage( checkerboard );
			UnloadImage( checkerboard );
		}
		else
		{
			textBoxBack = LoadTexture(defaultTexturePath);
		}
	}

	if(!cachedPath) return textBoxBack;
	if(!strcmp( cachedPath, defaultTexturePath )) return textBoxBack;  // why are you double loading??

	if(!strcmp( cachedPath, cachedTexturePath ))
		if( IsTextureReady(cachedBoxBack) ) return cachedBoxBack;

	TraceLog( LOG_INFO, TextFormat("TEXTBOX: Loading texture %s into cache", cachedPath) );
	UnloadTexture( cachedBoxBack );
	cachedBoxBack = LoadTexture( cachedPath );
	cachedTexturePath = cachedPath;

	return cachedBoxBack;
}

void setDefaultTextbox( const char *path )
{
	textBoxBack = LoadTexture( path );
	defaultTexturePath = path;
	TraceLog( LOG_INFO, TextFormat("TEXTBOX: Loaded %s as default text box", path) );
}

void displayTextbox( const char *text, float speed, Color textcolor, const char *customBox )
{
	if(!text) return;
	Texture2D box = updateCache( customBox );
	Rectangle textureSize = { 0, 0, box.width, box.height };
	Rectangle textPadding = { textBoxLocation.x + TEXTPADDING, textBoxLocation.y + TEXTPADDING,
					textBoxLocation.width - TEXTPADDING, textBoxLocation.height - TEXTPADDING };
	const char *textVisible = NULL;

	DrawTexturePro( box, textureSize, textBoxLocation, VECTOR2ZERO, 0, WHITE );
	
	if( TextLength(text) < textProgress )
	{
		textProgress = TextLength(text) + 1;
		textVisible = text;
	}
	else
	{
		textProgress += (speed * GetFrameTime());
		textVisible = TextSubtext( text, 0, (int) textProgress );
	}

	TraceLog(LOG_DEBUG, TextFormat( "TEXTBOX: Displaying text %s (offset %.3f)", textVisible, textProgress ));
	DrawTextBoxed( TEXTFONT, textVisible, textPadding, 20.0f, 2.0f, true, textcolor );
}

void resetVisibleTextboxText(void)
{
	textProgress = 0;
}

void unloadTextboxTextures(void)
{
	UnloadTexture(textBoxBack);
	UnloadTexture(cachedBoxBack);
	TraceLog( LOG_INFO, "TEXTBOX: Unloaded all textures" );
}
