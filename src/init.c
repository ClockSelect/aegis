#include <NUC1261.h>


/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock  = __HSI;              /*!< System Clock Frequency (Core Clock) */
uint32_t CyclesPerUs      = (__HSI / 1000000);  /*!< Cycles per micro second             */
uint32_t PllClock         = __HSI;              /*!< PLL Output Clock Frequency          */
const uint32_t gau32ClkSrcTbl[] = {__HXT, __LXT, __HSI, __LIRC, __HIRC48, 0UL, 0UL, __HIRC};


/**
 * @brief    Update the Variable SystemCoreClock
 *
 * @param    None
 *
 * @return   None
 *
 * @details  This function is used to update the variable SystemCoreClock
 *           and must be called whenever the core clock is changed.
 */
void SystemCoreClockUpdate(void)
{
    uint32_t u32Freq, u32ClkSrc;
    uint32_t u32HclkDiv;

    u32ClkSrc = CLK->CLKSEL0 & CLK_CLKSEL0_HCLKSEL_Msk;

    /* Update PLL Clock */
    PllClock = CLK_GetPLLClockFreq();

    if(u32ClkSrc != CLK_CLKSEL0_HCLKSEL_PLL)
    {
        /* Use the clock sources directly */
        u32Freq = gau32ClkSrcTbl[u32ClkSrc];
    }
    else
    {
        /* Use PLL clock */
        u32Freq = PllClock;
    }

    u32HclkDiv = (CLK->CLKDIV0 & CLK_CLKDIV0_HCLKDIV_Msk) + 1;

    /* Update System Core Clock */
    SystemCoreClock = u32Freq / u32HclkDiv;

    CyclesPerUs = (SystemCoreClock + 500000) / 1000000;
}


/**
 * @brief    System Initialization
 *
 * @param    None
 *
 * @return   None
 *
 * @details  The necessary initialization of system. Global variables are forbidden here.
 */
void SystemInit( void )
{
	SYS_UnlockReg();
	SYS_DISABLE_POR();

	CLK_EnableModuleClock( HDIV_MODULE );

	CLK_EnableXtalRC( CLK_PWRCTL_HIRCEN_Msk );
	CLK_WaitClockReady( CLK_STATUS_HIRCSTB_Msk );
	CLK_SetHCLK( CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1) );
	CLK_DisableXtalRC( CLK_PWRCTL_LXTEN_Msk );
	CLK_SetCoreClock( 72000000 );
	CLK_WaitClockReady( CLK_STATUS_PLLSTB_Msk );

	CLK_EnableSysTick( CLK_CLKSEL0_STCLKSEL_HCLK, 72000 );

	SYS_EnableBOD( SYS_BODCTL_BOD_INTERRUPT_EN, SYS_BODCTL_BODVL_2_7V );
	NVIC_EnableIRQ( BOD_IRQn );

	SYS_LockReg();
}


