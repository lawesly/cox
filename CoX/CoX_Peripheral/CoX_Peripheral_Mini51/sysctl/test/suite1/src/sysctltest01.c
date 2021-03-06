//*****************************************************************************
//
//! @page xsysctl_testcase xcore register test
//!
//! File: @ref xsysctltest.c
//!
//! <h2>Description</h2>
//! This module implements the test sequence for the xsysctl sub component.<br><br>
//! - \p Board: M051 <br><br>
//! - \p Last-Time(about): 0.5s <br><br>
//! - \p Phenomenon: Success or failure information will be printed on the UART. <br><br>
//! .
//!
//! <h2>Preconditions</h2>
//! The module requires the following options:<br><br>
//! - \p Option-define:
//! <br>(1)None.<br><br>
//! - \p Option-hardware:
//! <br>(1)Connect an USB cable to the development board.<br><br>
//! - \p Option-OtherModule:
//! <br>Connect an COM cable to the development board.<br>
//! .
//! In case some of the required options are not enabled then some or all tests
//! may be skipped or result FAILED.<br>
//!
//! <h2>Test Cases</h2>
//! The module contain those sub tests:<br><br>
//! - \subpage test_xsysctl_register
//! .
//! \file xsysctltest.c
//! \brief xsysctl test source file
//! \brief xsysctl test header file <br>
//
//*****************************************************************************

#include "test.h"
#include "xhw_memmap.h"
//*****************************************************************************
//
//!\page test_xsysctl_register test_xsysctl_register
//!
//!<h2>Description</h2>
//!Test xsysctl register. <br>
//!
//
//*****************************************************************************

unsigned long ulxWDT0ClkSource[] = {xSYSCTL_WDT0_EXTSL,
                                    xSYSCTL_WDT0_HCLK_2048,
                                    xSYSCTL_WDT0_INTSL};

unsigned long ulxPWMAClkSource[] = {xSYSCTL_PWMA_HCLK};

unsigned long ulxUART0ClkSource[] = {xSYSCTL_UART0_EXTSL,
                                     xSYSCTL_UART0_INT};

unsigned long ulxTIMER0ClkSource[] = {xSYSCTL_TIMER0_MAIN,
                                      xSYSCTL_TIMER0_HCLK,
                                      xSYSCTL_TIMER0_INT};


unsigned long ulxTIMER1ClkSource[] = {xSYSCTL_TIMER1_MAIN,
                                      xSYSCTL_TIMER1_HCLK, 
                                      xSYSCTL_TIMER1_INT};

unsigned long ulxFRQDIVClkSource[] = {xSYSCTL_FRQDIV_MAIN, 
                                      xSYSCTL_FRQDIV_HCLK, 
                                      xSYSCTL_FRQDIV_INT};

unsigned long ulxADC0ClkSource[] = {xSYSCTL_ADC0_MAIN, 
                                    xSYSCTL_ADC0_HCLK, 
                                    xSYSCTL_ADC0_INT};
//*****************************************************************************
//
//! \brief Get the Test description of xsysctl002 register test.
//!
//! \return the desccription of the xcore001 test.
//
//*****************************************************************************
static char* xSysctl002GetTest(void)
{
    return "xsysctl, 002, SysCtl Peripheral Clock Source Set test";
}


//*****************************************************************************
//
//! \brief something should do before the test execute of xsysctl002 test.
//!
//! \return None.
//
//*****************************************************************************
static void xSysctl002Setup(void)
{
    SysCtlKeyAddrUnlock();
    xHWREG(SYSCLK_PWRCON) |= SYSCLK_PWRCON_XTL12M_EN;
    xHWREG(SYSCLK_PWRCON) |= SYSCLK_PWRCON_OSC22M_EN;
    xHWREG(SYSCLK_PWRCON) |= SYSCLK_PWRCON_OSC10K_EN;
    
}


//*****************************************************************************
//
//! \brief something should do after the test execute of xsysctl002 test.
//!
//! \return None.
//
//*****************************************************************************
static void xSysctl002TearDown(void)
{   
    unsigned long ulTemp;
    ulTemp = SYSCTL_SYSDIV_1|SYSCTL_XTAL_12MHZ|SYSCTL_OSC_MAIN|SYSCTL_INT_OSC_DIS;
    SysCtlHClockSet(ulTemp);

    SysCtlPeripheralClockSourceSet(SYSCTL_PERIPH_UART_S_EXT12M);
    SysCtlIPClockDividerSet(SYSCTL_PERIPH_UART_D|1);
}


//*****************************************************************************
//
//! \brief xsysctl 004 test of Set a peripheral clock source and peripheral 
//�� divide test .
//!
//! \return None.
//
//*****************************************************************************
static void xsysctl_xSysCtlPeripheralClockSourceSet_test(void)
{
    unsigned long ulTemp,i,ulTempValue,j;

    for(i = 1; i < 3; i++)
    {
        xSysCtlPeripheralClockSourceSet(ulxWDT0ClkSource[i], 1);
        ulTemp = xHWREG(SYSCLK_CLKSEL1);
        TestAssert(((i+1) == (ulTemp & SYSCLK_CLKSEL1_WDG_M)), "xsysctl API error!");
    }
    
    xSysCtlPeripheralClockSourceSet(ulxWDT0ClkSource[0], 1);
    ulTemp = xHWREG(SYSCLK_CLKSEL1);
    TestAssert((0 == (ulTemp & SYSCLK_CLKSEL1_WDG_M)), "xsysctl API error!");
    

    xSysCtlPeripheralClockSourceSet(ulxPWMAClkSource[0], 1);
    ulTemp = xHWREG(SYSCLK_CLKSEL1);
    TestAssert((2 == (ulTemp & SYSCLK_CLKSEL1_PWM01_M)>>SYSCLK_CLKSEL1_PWM01_S),
                                                          "xsysctl API error!");
    
    xSysCtlPeripheralClockSourceSet(ulxTIMER0ClkSource[0], 1);
    ulTemp = xHWREG(SYSCLK_CLKSEL1);
    TestAssert((0 == (ulTemp & SYSCLK_CLKSEL1_TMR0_M)>>SYSCLK_CLKSEL1_TMR0_S),
                                                          "xsysctl API error!");
    
    xSysCtlPeripheralClockSourceSet(ulxTIMER0ClkSource[1], 1);
    ulTemp = xHWREG(SYSCLK_CLKSEL1);
    TestAssert((2 == (ulTemp & SYSCLK_CLKSEL1_TMR0_M)>>SYSCLK_CLKSEL1_TMR0_S),
                                                          "xsysctl API error!");
    xSysCtlPeripheralClockSourceSet(ulxTIMER0ClkSource[2], 1);
    ulTemp = xHWREG(SYSCLK_CLKSEL1);
    TestAssert((7 == (ulTemp & SYSCLK_CLKSEL1_TMR0_M)>>SYSCLK_CLKSEL1_TMR0_S),
                                                          "xsysctl API error!");
    
    xSysCtlPeripheralClockSourceSet(ulxTIMER1ClkSource[0], 1);
    ulTemp = xHWREG(SYSCLK_CLKSEL1);
    TestAssert((0 == (ulTemp & SYSCLK_CLKSEL1_TMR1_M)>>SYSCLK_CLKSEL1_TMR1_S),
                                                          "xsysctl API error!");
    
    xSysCtlPeripheralClockSourceSet(ulxTIMER1ClkSource[1], 1);
    ulTemp = xHWREG(SYSCLK_CLKSEL1);
    TestAssert((2 == (ulTemp & SYSCLK_CLKSEL1_TMR1_M)>>SYSCLK_CLKSEL1_TMR1_S),
                                                          "xsysctl API error!");
    
    xSysCtlPeripheralClockSourceSet(ulxTIMER1ClkSource[2], 1);
    ulTemp = xHWREG(SYSCLK_CLKSEL1);
    TestAssert((7 == (ulTemp & SYSCLK_CLKSEL1_TMR1_M)>>SYSCLK_CLKSEL1_TMR1_S),
                                                          "xsysctl API error!");
    
    //
    // UART0 Clock Source and Divide Test
    //
    
    for(j = 1; j <= 16; j++)
    {
        for(i = 0; i < 1; i++)
        {
            xSysCtlPeripheralClockSourceSet(ulxUART0ClkSource[i], j);
            ulTemp = xHWREG(SYSCLK_CLKSEL1);
            TestAssert((i == (ulTemp & SYSCLK_CLKSEL1_UART_M)>>SYSCLK_CLKSEL1_UART_S),
                                                          "xsysctl API error!");
            ulTempValue = xHWREG(SYSCLK_CLKDIV);
            TestAssert(((j-1) == (ulTempValue & SYSCLK_CLKDIV_UART_M)>>SYSCLK_CLKDIV_UART_S),
                                                          "xsysctl API error!");
        }
        xSysCtlPeripheralClockSourceSet(ulxUART0ClkSource[1], j);
        ulTemp = xHWREG(SYSCLK_CLKSEL1);
        TestAssert((3 == (ulTemp & SYSCLK_CLKSEL1_UART_M)>>SYSCLK_CLKSEL1_UART_S),
                                                          "xsysctl API error!");
    }   
    
    

    xSysCtlPeripheralClockSourceSet2(ADC0, INT, 159);
    ulTemp = xHWREG(SYSCLK_CLKSEL1);
    TestAssert((2 == (ulTemp &0x00000008)>>SYSCLK_CLKSEL1_ADC_S),
                                                         "xsysctl API  error!");

    ulTemp = xHWREG(SYSCLK_CLKDIV);
    TestAssert((158 == (ulTemp & SYSCLK_CLKDIV_ADC_M)>>SYSCLK_CLKDIV_ADC_S),
                                                         "xsysctl API error!"); 

}


//*****************************************************************************
//
//! \brief xsysctl 002 test execute main body.
//!
//! \return None.
//
//*****************************************************************************
static void xSysctl002Execute(void)
{
    xsysctl_xSysCtlPeripheralClockSourceSet_test();
}

//
// xsysctl register test case struct.
//
const tTestCase sTestXSysctl002Register = {
		xSysctl002GetTest,
		xSysctl002Setup,
		xSysctl002TearDown,
		xSysctl002Execute
};

//
// xsysctl test suits.
//
const tTestCase * const psPatternXsysctl01[] =
{
    &sTestXSysctl002Register,
    0
};