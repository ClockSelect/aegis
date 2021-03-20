#ifndef __DEVICES_H__
#define __DEVICES_H__


#define ADC_VREF			3300

#define ADC_CH_BATT_TOTAL	0
#define ADC_CH_BOARD_TEMP	3
#define ADC_CH_BATT_CELL1	4
#define ADC_CH_BATT_CURR	13
#define ADC_CH_COIL_CURR	14
#define ADC_CH_COIL_VOLT	15


#define PWM_CH_BCH				5
#define PWM_CH_BCH_MASK			(1<<PWM_CH_BCH)
#define PWM_BCH_CNTTYPE_Pos		(PWM_CH_BCH<<1)
#define PWM_BCH_CNTTYPE_Msk		(3<<PWM_BCH_CNTTYPE_Pos)


#define PWM_CH_LCD				2
#define PWM_CH_LCD_MASK			(1<<PWM_CH_LCD)
#define PWM_LCD_CNTTYPE_Pos		(PWM_CH_LCD<<1)
#define PWM_LCD_CNTTYPE_Msk		(3<<PWM_LCD_CNTTYPE_Pos)

#define PWM_CH_BOOST			3
#define PWM_CH_BOOST_MASK		(1<<PWM_CH_BOOST)
#define PWM_BOOST_CNTTYPE_Pos	(PWM_CH_BOOST<<1)
#define PWM_BOOST_CNTTYPE_Msk	(3<<PWM_BOOST_CNTTYPE_Pos)

#define PWM_CH_BUCK				4
#define PWM_CH_BUCK_MASK		(1<<PWM_CH_BUCK)
#define PWM_BUCK_CNTTYPE_Pos	(PWM_CH_BUCK<<1)
#define PWM_BUCK_CNTTYPE_Msk	(3<<PWM_BUCK_CNTTYPE_Pos)


#define DISP_CSX	PC4
#define DISP_DCX	PC1


extern void ConfigurePins( void );
extern void ConfigureGPIO( void );
extern void ConfigureSPI0( void );
extern void ConfigurePWM0( void );
extern void ConfigurePWM1( void );
extern void ConfigureADC( void );
extern void ConfigureHDIV( void );

extern void DisableSPI0( void );
extern void DisablePWM0( void );
extern void DisablePWM1( void );
extern void DisableADC( void );
extern void DisableHDIV( void );

extern uint32_t ADCSample( uint32_t ch, int count );


#endif /* __DEVICES_H__ */
