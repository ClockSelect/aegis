//==============================================================================
// aegis/src/battery.c
//------------------------------------------------------------------------------
// Battery management
//==============================================================================
#include "aegis.h"


typedef struct battery_s
{
	uint16_t	vcell1;
	uint16_t	vcell2;
	uint16_t	vtotal;
	BATT_STATUS	status;
	uint8_t		cell1_low	:1;
	uint8_t		cell2_low	:1;
}
battery_t;


static battery_t battery;


//------------------------------------------------------------------------------
// Initialization. Call at startup.
//------------------------------------------------------------------------------
void BMStartup( void )
{
	battery.status = BATT_UNK;
}


void BMShutdown( void )
{
	battery.status = BATT_UNK;
}


void BMUpdateBattery( void )
{
	static uint32_t last = 0;

	uint32_t t = GetSysTick();
	if ( t - last > TI_UPDATE_BATTERY )
	{
		BMReadBattery();
		BMUpdateStatus();
	}
}


void BMReadBattery( void )
{
	uint32_t sample;
	uint32_t vbat1, vbat2;

	sample = ADCSample( ADC_CH_BATT_CELL1, 1 );
	vbat1 = 3 * ADC_VREF * sample >> 13;

	sample = ADCSample( ADC_CH_BATT_TOTAL, 1 );
	vbat2 = 3 * ADC_VREF * sample >> 12;

	if ( battery.status == BATT_LOW ) vbat2 += BVO_PF4;

	if ( vbat2 > vbat1 ) vbat2 -= vbat1;
	if ( vbat1 > 0 ) vbat1 += BVO_CELL1;
	if ( vbat2 > 0 ) vbat2 += BVO_CELL2;

	battery.vcell1 = vbat1;
	battery.vcell2 = vbat2;
	battery.vtotal = vbat1 + vbat2;

	VOISet( VOI_VCELL1, &battery.vcell1 );
	VOISet( VOI_VCELL2, &battery.vcell2 );
}


void BMUpdateStatus( void )
{
	BATT_STATUS old = battery.status;

	if ( battery.vcell1 < 3000 ) battery.cell1_low = 1;
	else if ( battery.vcell1 >= 3100 ) battery.cell1_low = 0;
	if ( battery.vcell2 < 3000 ) battery.cell2_low = 1;
	else if ( battery.vcell2 >= 3100 ) battery.cell2_low = 0;

	if ( battery.cell1_low || battery.cell2_low )
	{
		battery.status = BATT_LOW;
		PF4 = 0;
	}
	else
	{
		battery.status = BATT_OK;
		PF4 = 1;
	}

	VOISet( VOI_BATT_STATUS, &battery.status );

	if ( battery.status != old )
	{
		EMSendEvent1P( EVENT_HARDWARE, EV_H_BATT_STATUS, battery.status );
	}
}


BATT_STATUS BMGetStatus( void )
{
	return battery.status;
}
