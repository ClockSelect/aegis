//==============================================================================
// aegis/src/display.c
//------------------------------------------------------------------------------
// Display management
//==============================================================================
#include "aegis.h"



const display_t	*display;

DISPLAY_STATUS display_status = DISPLAY_OFF;

uint16_t	fgcolor;
uint16_t	bgcolor;


//------------------------------------------------------------------------------
// Display startup.
//------------------------------------------------------------------------------
// Initializes the display global variable and calls the startup process.
//------------------------------------------------------------------------------
void DMStartup()
{
	display = &ST7735S;
	display->Startup();
}


//------------------------------------------------------------------------------
// Display status change
//------------------------------------------------------------------------------
void DMSetStatus( DISPLAY_STATUS status )
{
	display_status = status;
	EMSendEvent1P( EVENT_DISPLAY, EV_D_DISPLAY_STATUS, status );
}


DISPLAY_STATUS DMGetStatus( void )
{
	return display_status;
}


//------------------------------------------------------------------------------
// Number formatting
//------------------------------------------------------------------------------
//	- str:		Output buffer. Nust be large enough (nbdig+2).
//	- num:		The number.
//	- nbdig:	Maximum number of output digits.
//	- nbdec:	Maximum number of decimal places.
//	- exp:		Number divider in powers of 10.
//------------------------------------------------------------------------------
void FormatNumber( char *str, uint32_t num, int nbdig, int nbdec, int exp )
{
	char buf[10];

	int idx = 0;

	if ( !nbdig )
	{
		*str = 0;
		return;
	}

	while ( num )
	{
		num /= 10;
		uint16_t rem = HDIV->DIVREM;
		buf[idx++] = '0' + rem;
	}

	if ( idx <= exp )
	{
		*str++ = '0';
		if ( !nbdec || !--nbdig )
		{
			*str = 0;
			return;
		}
		if ( idx < exp )
		{
			*str++ = '.';

			int nbz = 0;
			while ( nbz++ < ( exp - idx ) )
			{
				*str++ = '0';
				if ( !--nbdec || !--nbdig )
				{
					*str = 0;
					return;
				}
			}
		}
	}

	while ( nbdig-- )
	{
		if ( idx == exp && nbdec ) *str++ = '.';
		if ( idx <= exp && !nbdec-- )
		{
			*str = 0;
			return;
		}
		*str++ = idx ? buf[--idx] : '0';
	}

	*str = 0;
}

