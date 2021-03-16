#include "aegis.h"


void _putchar( char c )
{
}


void StartupDevices( void )
{
	ConfigurePins();
	ConfigureGPIO();
	ConfigureADC();
	ConfigurePWM1();
	ConfigureSPI0();
	ConfigureHDIV();
}


void ResetChip( int to )
{
	SYS_UnlockReg();

	FMC_SELECT_NEXT_BOOT( to & 1 );
	SYS_ResetChip();

	SYS_LockReg();
	while ( 1 )
		;
}


void BOD_IRQHandler( void )
{
	SYS_CLEAR_BOD_INT_FLAG();
}


volatile uint32_t TickCounter;

void SysTick_Handler( void )
{
	TickCounter++;
}


void WaitTicks( uint32_t ticks )
{
	uint32_t end = TickCounter + ticks;
	while ( TickCounter <= end )
		;
}


/**
 * @brief    Firmware entry point
 *
 * @param    None
 *
 * @return   None
 *
 * @details  None
 */

int main( void )
{
	StartupDevices();

	RBStartup();
	TMStartup();
	EMStartup();
	BMStartup();
	SMStartup();

	SetDisplay( &ST7735S );
	display->Startup();

	while ( 1 )
	{
		SMRefresh();
		ReadUserInputs();
		BMUpdateBattery();
		EMHandleEvents();
		TMExecTasks();
	}
}
