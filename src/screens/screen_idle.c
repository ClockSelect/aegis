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
	DMSetBrightness( 25 );
}

extern int	ScrIdleEvent( Event_t *ev )
{
	return 0;
}

