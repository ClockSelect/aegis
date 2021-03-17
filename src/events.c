//==============================================================================
// aegis/src/events.c
//------------------------------------------------------------------------------
// Events management
//==============================================================================
#include "aegis.h"


static uint8_t	emrbid;
static uint8_t	embuffer[ 20 * sizeof( Event_t ) ];


// For debugging pruposes
static void PrintEvent( const Event_t *ev )
{
	if ( GetDisplayStatus() != DISPLAY_ON ) return;
	rect_t r = { 11, 150, 79, 159 };
	char buf[12];
	snprintf( buf, sizeof( buf ), "%02X %02X %02X %02X", ev->type, ev->k, ev->p1, ev->p2 );
	display->Print( buf, &r );
}


void EMStartup()
{
	emrbid = RBAlloc( embuffer, sizeof( embuffer ), sizeof( Event_t ) );
}


int EMSendEvent( const Event_t *ev )
{
	return RBSendMessage( emrbid, ev );
}

int EMSendEventInt( const uint32_t ev )
{
	return RBSendMessage( emrbid, &ev );
}

int EMSendEventNP( Event_e type, uint8_t id )
{
	Event_t ev;
	
	ev.type = type;
	ev.id = id;
	ev.p1 = 0;
	ev.p2 = 0;

	return RBSendMessage( emrbid, &ev );
}

int EMSendEvent1P( Event_e type, uint8_t id, uint8_t p1 )
{
	Event_t ev;
	
	ev.type = type;
	ev.id = id;
	ev.p1 = p1;
	ev.p2 = 0;

	return RBSendMessage( emrbid, &ev );
}

int EMSendEvent2P( Event_e type, uint8_t id, uint8_t p1, uint8_t p2 )
{
	Event_t ev;
	
	ev.type = type;
	ev.id = id;
	ev.p1 = p1;
	ev.p2 = p2;

	return RBSendMessage( emrbid, &ev );
}

int EMEventsPending()
{
	return RBHasMessages( emrbid );
}


int EMGetNextEvent( Event_t *ev )
{
	return RBReadMessage( emrbid, ev );
}



//==============================================================================
// Event handling
//==============================================================================

static void EMKeyEvent( Event_t *ev );
static void EMDisplayEvent( Event_t *ev );
static void EMHardwareEvent( Event_t *ev );


void EMHandleEvents()
{
	int pending = EMEventsPending();
	if ( !pending ) return;

	uint32_t t = GetSysTick();

	while ( 1 )
	{
		Event_t ev;
		EMGetNextEvent( &ev );

		PrintEvent( &ev );

		switch ( ev.type )
		{
			case EVENT_KEY:
				EMKeyEvent( &ev );
				break;

			case EVENT_DISPLAY:
				EMDisplayEvent( &ev );
				break;

			case EVENT_HARDWARE:
				EMHardwareEvent( &ev );
				break;

			case EVENT_NULL:
			default:
				break;
		}

		if ( !--pending ) break;
		if ( GetSysTick() - t > TI_EVENT_HANDLING ) break;
	}
}


static void EMKeyEvent( Event_t *ev )
{
	switch ( ev->k )
	{
		case EV_K_FIRE:
			if ( ev->p1 == 1 )
			{
				ResetChip( 0 );
			}
			break;

		case EV_K_FP:
			if ( ev->p1 == 1 )
			{
				ResetChip( 1 );
			}
			break;

		case EV_K_NULL:
		default:
			break;
	}
}


static void EMDisplayEvent( Event_t *ev )
{
	switch ( ev->d )
	{
		case EV_D_DISPLAY_STATUS:
		{
			switch ( ev->p1 )
			{
				case DISPLAY_ON:
					DELAYED_EVENT( 1, EVENT_DISPLAY, EV_D_BRIGHTNESS, 25, 0 );
					break;
			}
			break;
		}

		case EV_D_SCREEN:
		{
			SMShowScreen( ev->p1 );
			break;
		}

		case EV_D_BRIGHTNESS:
		{
			display->SetBrightness( ev->p1 );
			break;
		}

		case EV_D_NULL:
		default:
			break;
	}
}


static void EMHardwareEvent( Event_t *ev )
{
	switch ( ev->h )
	{
		case EV_H_BATT_STATUS:
			break;

		case EV_H_NULL:
		default:
			break;
	}
}

