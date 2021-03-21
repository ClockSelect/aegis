//==============================================================================
// aegis/src/screens/screen_main.c
//------------------------------------------------------------------------------
// Main screen
//==============================================================================
#include "aegis.h"
#include "screens.h"



//------------------------------------------------------------------------------
// Screen elements ids
//------------------------------------------------------------------------------
typedef enum eids_e
{
	VCELL1,
	VCELL2,
	BSTATUS,
	EMAX
}
EID;


//------------------------------------------------------------------------------
// Drawing functions predeclarations
//------------------------------------------------------------------------------
void draw_voltage( SCREEN_ELEM *e );
void draw_battstatus( SCREEN_ELEM *e );


//------------------------------------------------------------------------------
// Screen elements
//------------------------------------------------------------------------------
SCREEN_ELEM elements[] =
{
	[VCELL1] =
	{
		.fct	= draw_voltage,
		.voi	= VOI_VCELL1,
		.rect	= { 0, 10, 79, 19 }
	},
	[VCELL2] =
	{
		.fct	= draw_voltage,
		.voi	= VOI_VCELL1,
		.rect	= { 33, 10, 79, 19 }
	},
	[BSTATUS] =
	{
		.fct	= draw_battstatus,
		.voi	= VOI_BATT_STATUS,
		.rect	= { 66, 10, 79, 19 }
	}
};

#define NUM_ELEMS (sizeof(elements)/sizeof(SCREEN_ELEM))


//------------------------------------------------------------------------------
// Screen functions
//------------------------------------------------------------------------------

static uint8_t	draw_step;
static uint8_t	draw_elem;

extern void	ScrMainInit( void )
{
	display->SetFont( &simple_font );
	display->SetColor( 0x00FF00 );
	display->SetBgColor( 0x000000 );

	for ( int e = 0 ; e < NUM_ELEMS ; ++e )
		elements[e].draw = 1;

	draw_step = 0;
	draw_elem = 0;
}

extern int	ScrMainDraw( void )
{
	display->CLS();
	return 1;
}

extern int	ScrMainRefresh( void )
{
	SCREEN_ELEM *e = &elements[draw_elem];
	e->fct( e );

	if ( ++draw_elem >= NUM_ELEMS )
	{
		draw_elem = 0;
		draw_step = 1;
	}

	return draw_step;
}

extern void	ScrMainClose( void )
{
}

extern int	ScrMainEvent( Event_t *ev )
{
	return 0;
}


//------------------------------------------------------------------------------
// Drawing functions
//------------------------------------------------------------------------------

void draw_voltage( SCREEN_ELEM *e )
{
	char buf[6];
	static uint16_t volts;
	uint8_t changed;

	changed = VOIGetChanged( e->voi, &volts, VOI_USCREEN );
	if ( changed || e->draw )
	{
		FormatNumber( buf, volts, 4, 3, 3 );
		display->Print( buf, &e->rect, 0 );
	}
}

void draw_battstatus( SCREEN_ELEM *e )
{
	static uint8_t bs;
	uint8_t changed;

	changed = VOIGetChanged( e->voi, &bs , VOI_USCREEN );
	if ( changed || e->draw )
	{
		if ( bs != BATT_UNK )
			display->Print( bs == BATT_OK ? "OK" : "LO" , &e->rect, 0 );
	}
}

