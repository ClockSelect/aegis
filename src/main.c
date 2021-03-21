#include "aegis.h"


void _putchar( char c )
{
}


void ResetChip( int to )
{
	SYS_UnlockReg();

	FMC_SELECT_NEXT_BOOT( to & 1 );
	NVIC_SystemReset();

	SYS_LockReg();
	while ( 1 )
		;
}


void GPCDEF_IRQHandler( void )
{
	PD->INTSRC = PD->INTSRC;
	PE->INTSRC = PE->INTSRC;
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


void StartupDevices( void )
{
	ConfigurePins();
	ConfigureGPIO();
	ConfigureADC();
	ConfigurePWM0();
	ConfigurePWM1();
	ConfigureSPI0();
}


void StartupBox( void )
{
	BXStartup();	//	Box manager
	RBStartup();	//	Ringbuffers
	TMStartup();	//	Task manager
	EMStartup();	//	Event manager
	BMStartup();	//	Battery manager
	SMStartup();	//	Screens manager
	DMStartup();	//	Display manager
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
	StartupBox();

	uint8_t b = 25;
	VOISet( VOI_BRIGHTNESS, &b );

	while ( 1 )
	{
		SMRefresh();
		ReadUserInputs();
		BMUpdateBattery();
		TestUSBPlug();
		EMHandleEvents();
		TMExecTasks();
		BXCheckActivity();
	}
}
