//==============================================================================
// aegis/src/box.c
//------------------------------------------------------------------------------
// Box management
//==============================================================================
#include "aegis.h"


BOX_STATE	bx_box_state;
uint32_t	bx_last_activity;
uint8_t		bx_sysrst;

#define BOX_ACTIVITY	( bx_box_state & BOX_ACT_Msk )
#define BOX_LOCKS		( bx_box_state & BOX_LCK_Msk )

//------------------------------------------------------------------------------
// Initialization. Put the box in a known state.
//------------------------------------------------------------------------------
void BXStartup( void )
{
	bx_sysrst = SYS_IS_SYSTEM_RST();
	bx_box_state = BOX_ACTIVE | BOX_STARTUP;
	BXUserActivity();
}


//------------------------------------------------------------------------------
// State change
//------------------------------------------------------------------------------
static void BXStateChange( BOX_STATE bs )
{
	bx_box_state = bs;
}


//------------------------------------------------------------------------------
// Activity monitoring (go to idle or sleep id needed)
//------------------------------------------------------------------------------
void BXCheckActivity( void )
{
	static uint32_t last = 0;

	uint32_t now = GetSysTick();
	uint32_t idt = now - BXLastActivity();

	if ( now - last < TI_BOX_ACTCHECK )
		return;

	switch ( BOX_ACTIVITY )
	{
		case BOX_STARTUP:
		{
			SMScreen( SCREEN_MAIN );
			EMSendEvent1P( EVENT_BOX, EV_B_STATE_CHANGE, BOX_ACTIVE | BOX_ON );
			break;
		}

		case BOX_ON:
		{
			if ( BOX_LOCKS == BOX_IDLE )
			{
				if ( idt > TI_SLEEP )
				{
					EMSendEventNP( EVENT_BOX, EV_B_SLEEP );
				}
				if ( idt < TI_IDLE )
				{
					BXStateChange( BOX_ACTIVE | BOX_ON );
					SMScreen( SCREEN_MAIN );
					PD2 = 1;
				}
				break;
			}
			if ( idt > TI_IDLE )
			{
				BXStateChange( BOX_IDLE | BOX_ON );
				SMScreen( SCREEN_IDLE );
				PD2 = 0;
			}
			break;
		}

		case BOX_OFF:
			break;

		case BOX_LOWBAT:
			break;
	}

	last = GetSysTick();
}


//------------------------------------------------------------------------------
// Sleep mode
//------------------------------------------------------------------------------
static void GoToSleep( void )
{
	// Quick and dirty. :)

	DMShutdown();

	DisableSPI0();
	DisablePWM0();
	DisablePWM1();
	DisableADC();
	DisableHDIV();

	PB1 = 1;
	PF4 = 0;
	PD2 = 0;
	PA0 = 0;
	PB2 = 0;

	GPIO_EnableInt( PE,  0, GPIO_INT_RISING  );
	GPIO_EnableInt( PD,  0, GPIO_INT_FALLING );
	GPIO_EnableInt( PE, 10, GPIO_INT_FALLING );
	GPIO_EnableInt( PE, 13, GPIO_INT_FALLING );

	NVIC_EnableIRQ( GPCDEF_IRQn );

	GPIO_SET_DEBOUNCE_TIME( GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_2 );
	GPIO_ENABLE_DEBOUNCE( PD, GPIO_PIN_PIN0_Msk );
	GPIO_ENABLE_DEBOUNCE( PE, GPIO_PIN_PIN0_Msk|GPIO_PIN_PIN10_Msk|GPIO_PIN_PIN13_Msk );

	WaitTicks( 10 );

	SYS_UnlockReg();
	SYS_DISABLE_BOD();
	CLK->PWRCTL |= CLK_PWRCTL_PDWKIEN_Msk;
	CLK_PowerDown();
	SYS_LockReg();

	ResetChip( 0 );
}


//------------------------------------------------------------------------------
// Box events handling
//------------------------------------------------------------------------------
void BXEvent( Event_t *ev )
{
	switch ( ev->b )
	{
		case EV_B_STATE_CHANGE:
			BXStateChange( ev->p1 );
			break;

		case EV_B_SLEEP:
			GoToSleep();
			break;

		case EV_B_NULL:
		default:
			break;
	}
}
