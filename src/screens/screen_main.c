//==============================================================================
// aegis/src/screens/screen_main.c
//------------------------------------------------------------------------------
// Main screen
//==============================================================================
#include "aegis.h"



extern void	ScrMainInit( void )
{
	display->SetFont( &simple_font );
	display->SetColor( 0x00FF00 );
	display->SetBgColor( 0x000000 );
}

extern int	ScrMainDraw( void )
{
	display->CLS();
	return 1;
}

extern int	ScrMainRefresh( void )
{
	static uint32_t last_r_batt = 0;

	uint32_t t = GetSysTick();

	if ( t - last_r_batt > 1000 )
	{
		last_r_batt = t;

		rect_t r = { 0, 10, 79, 19 };

		char buf[16];
		uint32_t vbat1, vbat2;

		BMGetCells( &vbat1, &vbat2 );

		FormatNumber( buf, vbat1, 4, 3, 3 );
		r.left = 9;
		display->Print( buf, &r );

		FormatNumber( buf, vbat2, 4, 3, 3 );
		r.left = 44;
		display->Print( buf, &r );
	}

	return 1;
}

extern void	ScrMainClose( void )
{
}

extern int	ScrMainEvent( Event_t *ev )
{
	return 0;
}
