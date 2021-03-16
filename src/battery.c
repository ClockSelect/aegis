//==============================================================================
// aegis/src/battery.c
//------------------------------------------------------------------------------
// Battery management
//==============================================================================
#include "aegis.h"


typedef struct battery_s
{
	uint32_t	vcell1;
	uint32_t	vcell2;
	uint32_t	vtotal;
}
battery_t;


static battery_t battery;


//------------------------------------------------------------------------------
// Initialization. Call at startup.
//------------------------------------------------------------------------------
void BMStartup( void )
{
}


void BMUpdateBattery( void )
{
	static uint32_t last = 0;

	uint32_t t = GetSysTick();
	if ( t - last > TI_UPDATE_BATTERY )
	{
		BMReadBattery();
	}
}


void BMReadBattery( void )
{
	uint32_t sample;
	uint32_t vbat1, vbat2;

	sample = ADCSample( ADC_CH_BATT_CELL1, 16 );
	vbat1 = 3 * ADC_VREF * sample >> 13;

	sample = ADCSample( ADC_CH_BATT_TOTAL, 16 );
	vbat2 = 3 * ADC_VREF * sample >> 12;

	if ( vbat2 > vbat1 ) vbat2 -= vbat1;
	if ( vbat1 > 0 ) vbat1 += BVO_CELL1;
	if ( vbat2 > 0 ) vbat2 += BVO_CELL2;

	battery.vcell1 = vbat1;
	battery.vcell2 = vbat2;
	battery.vtotal = vbat1 + vbat2;
}


void BMGetCells( uint32_t *v1, uint32_t *v2 )
{
	*v1 = battery.vcell1;
	*v2 = battery.vcell2;
}
