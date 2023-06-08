#include "cutscene.h"
#include <stddef.h>
#include <raymath.h>

#define MAXTWEENS 20

typedef struct {
	Vector2 startPos;
	Vector2 endPos;
	Vector2 *target;
	float progress;
	float duration;
	bool paused;
} vectorTweenData;


static bool pauseVectorTweens = false;
static vectorTweenData *tweenTable[MAXTWEENS];


static vectorTweenData *safeTableIndex( unsigned id )
{
	if( id > MAXTWEENS )
	{
		TraceLog( LOG_WARNING, TextFormat("TWEEN: Attempted to index table index %u (max %u)", id, MAXTWEENS) );
		return NULL;
	}

	return tweenTable[id];
}

static void pauseTweenByID( unsigned id, bool newPause )
{
	vectorTweenData *index = safeTableIndex(id);
	if( index == NULL )
	{
		TraceLog( LOG_WARNING, TextFormat("TWEEN: Attempted to pause nonexistant tween %u", id) );
		return;
	}
	index->paused = newPause;
	TraceLog( LOG_INFO, TextFormat("TWEEN: %saused tween id %u", newPause ? "P" : "Unp", id ) );
}

static void freeTweenByID( unsigned id )
{
	vectorTweenData *index = safeTableIndex(id);
	if( index == NULL )
	{
		TraceLog( LOG_WARNING, TextFormat("TWEEN: Attempted double free on tween %u"), id );
		return;
	}
	
	*(index->target) = index->endPos;

	TraceLog( LOG_INFO, TextFormat("TWEEN: Attempting to free tween %u (%p)", id, index->target) );
	free( index );
	tweenTable[id] = NULL;
	TraceLog( LOG_INFO, TextFormat("TWEEN: Tween %u freed", id) );
}

static long findTweenByPointer( Vector2 *search )
{
	unsigned i;
	vectorTweenData *candidate;
	
	for( i = 0; i < MAXTWEENS; i++ )
	{
		if( (candidate = tweenTable[i]) == NULL ) continue;
		if( candidate->target == search ) return (long) i;
	}

	return -1;
}

void startVectorTween( Vector2 *target, Vector2 endPos, float duration )
{
	if( !target ) return;

	if( findTweenByPointer(target) > 0 )
	{
		TraceLog(LOG_ERROR,
			TextFormat("TWEEN: Attempt to apply multiple tweens to vec2 %p", target));
		return;
	}

	TraceLog(LOG_INFO, 
			TextFormat( "TWEEN: Starting tween of vec2 %p over %d seconds to ( %.1f, %.1f )", 
				target, duration, endPos.x, endPos.y )
		);
	vectorTweenData *newData = malloc(sizeof(vectorTweenData));
	if( !newData ) return;

	newData->startPos = *target;
	newData->endPos = endPos;
	newData->target = target;
	newData->progress = 0;
	newData->duration = duration;
	newData->paused = false;

	for( unsigned i = 0; i < MAXTWEENS; i++ )
	{
		if( tweenTable[i] ) continue;
		tweenTable[i] = newData;
		break;
	}
}

void proccessTweens( void )
{
	if( pauseVectorTweens ) return;

	vectorTweenData *curData = NULL;
	for( unsigned i = 0; i < MAXTWEENS; i++ )
	{
		if( (curData = tweenTable[i] ) == NULL ) continue;
		if( curData->paused ) continue;

		curData->progress += GetFrameTime();
		float percent = curData->progress / curData->duration;
		
		if( percent > 1 )
		{
			TraceLog( LOG_INFO, TextFormat("TWEEN: Tween %p completed", curData->target) );
			freeTweenByID( i );
			continue;
		}
		
		else
			*(curData->target) = Vector2Lerp( curData->startPos, curData->endPos, percent );
	}
}

void pauseTweenByPointer( Vector2 *search, bool newPause )
{
	long id = findTweenByPointer( search );
	if( id < 0 ) return;
	pauseTweenByID( (unsigned) id, newPause );
}

void pauseAllTweens( bool newPause )
{
	TraceLog(LOG_INFO, TextFormat( "TWEEN: %saused tween proccessing", newPause ? "P" : "Unp" ));
	pauseVectorTweens = newPause;
}

void freeTweenByPointer( Vector2 *search )
{
	long id = findTweenByPointer( search );
	if( id < 0 ) return;
	freeTweenByID( (unsigned) id );
}
