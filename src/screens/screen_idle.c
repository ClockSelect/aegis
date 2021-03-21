//==============================================================================
// aegis/src/screens/screen_main.c
//------------------------------------------------------------------------------
// Main screen
//==============================================================================
#include "aegis.h"



extern void	ScrIdleInit( void )
{
	DMSetBrightness( 0 );
}

extern int	ScrIdleDraw( void )
{
	display->CLS();
	return 1;
}

extern void	ScrIdleClose( void )
{
	uint8_t b;

	VOIGet( VOI_BRIGHTNESS, &b );
	DMSetBrightness( b );
}

extern int	ScrIdleEvent( Event_t *ev )
{
	return 0;
}

