//==============================================================================
// aegis/src/screens.c
//------------------------------------------------------------------------------
// Screens management
//==============================================================================
#include "aegis.h"

#include "screens.h"


//------------------------------------------------------------------------------
// Screen object definition
//------------------------------------------------------------------------------
typedef struct screen_desc
{
	SCREENID	id;
	uint8_t		dialog;
	uint32_t	refresh_rate;
	uint32_t	duration;
	void		(*pf_init)( void );
	int			(*pf_draw)( void );
	int			(*pf_refresh)( void );
	void		(*pf_close)( void );
	int			(*pf_event)( Event_t *ev );
}
screen_desc_t;


static int	screen_cls( void ) { display->CLS(); return 1; }
static void	screen_null( void ) {}
static int	screen_null_draw( void ) { return 1; }
static int	screen_noev( Event_t *ev ) { return 0; }


static const screen_desc_t screens[] =
{
	{
		SCREEN_OFF,
		0,
		0,
		0,
		screen_null,
		screen_cls,
		screen_null_draw,
		screen_null,
		screen_noev
	},
	{
		SCREEN_MAIN,
		0,
		50,
		0,
		ScrMainInit,
		ScrMainDraw,
		ScrMainRefresh,
		ScrMainClose,
		ScrMainEvent
	}
};

#define NUM_SCREENS (sizeof(screens)/sizeof(screen_desc_t))


typedef struct screen
{
	const screen_desc_t*	s;
	uint32_t	opening_tick;
	uint32_t	last_refresh;
	SCREENID	last_screen_id;
}
screen_t;


static screen_t screen;


//------------------------------------------------------------------------------
// Initialization. Call at startup.
//------------------------------------------------------------------------------
void SMStartup( void )
{
	SMScreen( SCREEN_MAIN );
}


//------------------------------------------------------------------------------
// Screen invocation (gently asked).
//------------------------------------------------------------------------------
void SMScreen( SCREENID sid )
{
	EMSendEvent1P( EVENT_DISPLAY, EV_D_SCREEN, sid );
}


//------------------------------------------------------------------------------
// Screen invocation (now do it).
//------------------------------------------------------------------------------
void SMShowScreen( SCREENID sid )
{
	const screen_desc_t *s = 0;

	for ( int i = 0 ; i < NUM_SCREENS ; ++i )
	{
		if ( screens[i].id == sid )
		{
			s = &screens[i];
			break;
		}
	}

	if ( !s ) return;

	if ( s->dialog )
	{
		if ( screen.s )
		{
			screen.last_screen_id = screen.s->id;
			screen.s->pf_close();
		}
		screen.s = s;
		screen.s->pf_init();
		screen.last_refresh = 0;
	}
	else
	{
		if ( screen.s )
		{
			screen.s->pf_close();
		}
		screen.s = s;
		screen.s->pf_init();
		screen.last_refresh = 0;
	}
}


//------------------------------------------------------------------------------
// Screen refresh.
//------------------------------------------------------------------------------
void SMRefresh( void )
{
	if ( GetDisplayStatus() != DISPLAY_ON ) return;

	uint32_t t = GetSysTick();

	if ( ( screen.s->duration ) && ( t - screen.opening_tick > screen.s->duration ) )
	{
		SMScreen( screen.last_screen_id );
		return;
	}

	if ( !screen.last_refresh )
	{
		if ( screen.s->pf_draw() )
		{
			screen.last_refresh = t;
		}
	}
	else if ( ( screen.s->refresh_rate ) && ( t - screen.last_refresh > screen.s->refresh_rate ) )
	{
		if ( screen.s->pf_refresh() )
		{
			screen.last_refresh = t;
		}
	}
}


//------------------------------------------------------------------------------
// Key Event.
//------------------------------------------------------------------------------
int SMInputEvent( Event_t *ev )
{
	return 0;
}


//------------------------------------------------------------------------------
// Display/Screen Event.
//------------------------------------------------------------------------------
int SMDisplayEvent( Event_t *ev )
{
	return 0;
}

