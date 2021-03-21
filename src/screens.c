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
	[SCREEN_OFF] =
	{
		0,
		0,
		0,
		screen_null,
		screen_cls,
		screen_null_draw,
		screen_null,
		screen_noev
	},
	[SCREEN_MAIN] =
	{
		0,
		50,
		0,
		ScrMainInit,
		ScrMainDraw,
		ScrMainRefresh,
		ScrMainClose,
		ScrMainEvent
	},
	[SCREEN_IDLE] =
	{
		0,
		0,
		0,
		ScrIdleInit,
		ScrIdleDraw,
		screen_null_draw,
		ScrIdleClose,
		ScrIdleEvent
	}
};

#define NUM_SCREENS (sizeof(screens)/sizeof(screen_desc_t))


typedef struct screen
{
	const screen_desc_t *s;
	uint32_t	opening_tick;
	uint32_t	last_refresh;
	SCREENID	sid;
	SCREENID	last_sid;
}
screen_t;


static screen_t screen =
{
	.sid = SCREEN_NONE,
	.last_sid = SCREEN_NONE
};


//------------------------------------------------------------------------------
// Initialization. Call at startup.
//------------------------------------------------------------------------------
void SMStartup( void )
{
	SMScreen( SCREEN_OFF );
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
	if ( sid >= NUM_SCREENS ) return;

	const screen_desc_t *s = &screens[sid];

	if ( screen.s )
	{
		if ( s->dialog )
		{
			if ( !screen.s->dialog )
				screen.last_sid = screen.sid;
		}

		screen.s->pf_close();
	}

	screen.s = s;
	screen.sid = sid;
	screen.s->pf_init();
	screen.last_refresh = 0;
}


//------------------------------------------------------------------------------
// Screen refresh.
//------------------------------------------------------------------------------
void SMRefresh( void )
{
	if ( DMGetStatus() != DISPLAY_ON ) return;

	uint32_t t = GetSysTick();

	if ( ( screen.s->duration ) && ( t - screen.opening_tick > screen.s->duration ) )
	{
		SMScreen( screen.last_sid );
		return;
	}

	if ( !screen.last_refresh )
	{
		if ( screen.s->pf_draw() )
		{
			screen.opening_tick = t;
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
	return screen.s->pf_event( ev );
}


//------------------------------------------------------------------------------
// Display/Screen Event.
//------------------------------------------------------------------------------
int SMDisplayEvent( Event_t *ev )
{
	return 0;
}

