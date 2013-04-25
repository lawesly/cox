//*****************************************************************************
//
//! \file Example.c
//! \brief the AT45DBxxxD Full Test Example.
//! \version 1.0
//! \date 12/14/2012
//! \author CooCox
//! \copy
//!
//! Copyright (c)  2012, CooCox
//! All rights reserved.
//!
//! Redistribution and use in source and binary forms, with or without
//! modification, are permitted provided that the following conditions
//! are met:
//!
//!     * Redistributions of source code must retain the above copyright
//! notice, this list of conditions and the following disclaimer.
//!     * Redistributions in binary form must reproduce the above copyright
//! notice, this list of conditions and the following disclaimer in the
//! documentation and/or other materials provided with the distribution.
//!     * Neither the name of the <ORGANIZATION> nor the names of its
//! contributors may be used to endorse or promote products derived
//! from this software without specific prior written permission.
//!
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
//! THE POSSIBILITY OF SUCH DAMAGE.
//*****************************************************************************
#include "xhw_types.h"
#include "xgpio.h"
#include "xhw_memmap.h"
#include "xhw_sysctl.h"
#include "xhw_uart.h"
#include "xsysctl.h"
#include "xuart.h"
#include "AT45DBxxxD.h"

#define MAXLEN      4096

//*****************************************************************************
//
//! \brief UART1 initialize.
//!
//! \param sulBaudRate is the BaudRate of UART1
//!
//! \details UART1 initialize.
//!
//! \return None.
//
//*****************************************************************************
void MyUartInit(unsigned long ulBaudRate)
{
    xSysCtlPeripheralEnable(xSYSCTL_PERIPH_GPIOA);

    xSysCtlPeripheralReset(xSYSCTL_PERIPH_UART1);
    xSysCtlPeripheralEnable(xSYSCTL_PERIPH_UART1);
    xUARTConfigSet(xUART1_BASE, ulBaudRate, (xUART_CONFIG_WLEN_8 |
                   xUART_CONFIG_STOP_1 |
                   xUART_CONFIG_PAR_NONE));

    xUARTEnable(xUART1_BASE, (UART_BLOCK_UART | UART_BLOCK_TX | UART_BLOCK_RX));
    xSysCtlPeripheralEnable(SYSCTL_PERIPH_AFIO);
    xSPinTypeUART(UART1TX, PA9);
}

//*****************************************************************************
//
//! \brief Prints string through UART1.
//!
//! \param str is the string to be printed
//!
//! \details Prints string through UART1.
//!
//! \return None.
//
//*****************************************************************************
static void xUARTPutString(char *str)
{
    char *s = str;
    while(*s != 0)
    {
        xUARTCharPut(xUART1_BASE, *s++);
    }
}

//*****************************************************************************
//
//! \brief Prints a decimal unsigned number.
//!
//! \param n is the number to be printed
//!
//! \details Prints a decimal unsigned number.
//!
//! \return None.
//
//*****************************************************************************
static
void UartPrintfNumber(unsigned long n)
{
    char buf[16], *p;

    if (n == 0)
    {
    	xUARTCharPut(xUART1_BASE, '0');
    }
    else
    {
        p = buf;
        while (n != 0)
        {
            *p++ = (n % 10) + '0';
            n /= 10;
        }

        while (p > buf)
        	xUARTCharPut(xUART1_BASE, *--p);
    }
}

static long Powerof10(unsigned char pow)
{
	unsigned long result = 1;
	while(pow--)
	{
		result *= 10;
	}
	return result;
}
static long UartGetNumber(void)
{
	unsigned long num = 0;
	unsigned char numbuf[11];
	unsigned char i,j;
	for(i=0;i<11;i++)
	{
		numbuf[i] = xUARTCharGet(xUART1_BASE);
		xUARTCharPut(xUART1_BASE, numbuf[i]);
		if(numbuf[i]==13) break;
	}
	xUARTCharPut(xUART1_BASE, '\n');
	xUARTCharPut(xUART1_BASE, '\r');
	for(j=0;j<i;j++)
	{
		num += (unsigned long)(numbuf[j]-'0') * Powerof10(i-j-1);
	}
	return num;
}

void PrintResult(res)
{
	switch(res)
	{
	case AT45DBxxxD_OP_OK:
		xUARTPutString("\n\rOperation success!\n\r");
		break;
	case AT45DBxxxD_OP_BUSY:
		xUARTPutString("\n\rChip no response!\n\r");
		break;
	case AT45DBxxxD_OP_INVALID:
		xUARTPutString("\n\rInvalid address or operation!\n\r");
		break;
	case AT45DBxxxD_OP_WRITEPROTECT:
		xUARTPutString("\n\rSector write protect enable, unable to write or erase!\n\r");
		break;
	default :
		break;
	}
}

extern AT45DBxxxDInfoStruct AT45DBxxxDInfo;

void PrintDeviceInfo(void)
{
    xUARTPutString("\r\n\nAT45DBxxxD Device ID:>> ");
    UartPrintfNumber(AT45DBxxxDInfo.usChipID);
    xUARTPutString("\r\nAT45DBxxxD Page Size:>> ");
    UartPrintfNumber(AT45DBxxxDInfo.usPageSize);
    xUARTPutString(" Bytes\r\nAT45DBxxxD Block Size:>> ");
    UartPrintfNumber(AT45DBxxxDInfo.ulBlockSize);
    xUARTPutString(" Bytes\r\nAT45DBxxxD Sector Size:>> ");
    UartPrintfNumber(AT45DBxxxDInfo.ulSectorSize);
    xUARTPutString(" Bytes\r\nAT45DBxxxD Chip Size:>> ");
    UartPrintfNumber(AT45DBxxxDInfo.ulCapacity);
    xUARTPutString(" Bytes\r\n");
}

unsigned char Buff[MAXLEN] = {0};

void SPIFlashTest(void)
{
//    char input[8] = {0};
    unsigned long cnt;
    unsigned long tmp1;
    unsigned char res;

    xSysCtlClockSet(72000000, xSYSCTL_OSC_MAIN | xSYSCTL_XTAL_8MHZ);
    xSysCtlDelay(10000);

    MyUartInit(115200);		//UART1

    res = AT45DBxxxDInit(1000000);
    PrintResult(res);

    PrintDeviceInfo();

loop:
    xUARTPutString("\n\n\r**************** AT45DBxxxD flash test program ***************\n\r");
    xUARTPutString("\r\n\n01.Erase page\n\r02.Erase block\n\r03.Erase sector\n\r"
                   "04.Erase chip\n\n\r05.Print Device information\n\n\r06.Read sector protection register\n\r"
                   "07.Program sector protection register\n\r08.Enable Software Sector Protection\n\r"
                   "09.Disable Software Sector Protection\n\n\r10.Test read internal buffer\n\r"
                   "11.Test Write internal buffer\n\n\r12.Read a specified address and amount of bytes\n\r"
                   "13.Write a specified page of data\n\r14.Random address and amount write\n\n\rPress 0 to view data page.\n\r");
    xUARTPutString("\r\n\nPlease input command with an enter>>");

    tmp1 = UartGetNumber();

    switch(tmp1)
    {
    case 1:		//Erase page
        xUARTPutString("\r\nPlease input the page you want to erase(0~");
        UartPrintfNumber(AT45DBxxxDInfo.ulCapacity/AT45DBxxxDInfo.usPageSize - 1);
        xUARTPutString(")\n\r>>");

        tmp1 = UartGetNumber();
        res = AT45DBxxxDPageErase(tmp1);
        PrintResult(res);

        goto loop;
        break;

    case 2:		//Erase block
        xUARTPutString("\r\nPlease input the block you want to erase(0~");
        UartPrintfNumber(AT45DBxxxDInfo.ulCapacity/AT45DBxxxDInfo.ulBlockSize - 1);
        xUARTPutString(")\n\r>>");
        tmp1 = UartGetNumber();
        res = AT45DBxxxDBlockErase(tmp1);
        PrintResult(res);

        goto loop;
        break;

    case 3:		//Erase sector
        xUARTPutString("\r\nPlease input the sector you want to erase(0~");
        UartPrintfNumber(AT45DBxxxDInfo.ulCapacity/AT45DBxxxDInfo.ulSectorSize - 1);
        xUARTPutString(")\n\r>>");
        tmp1 = UartGetNumber();
        res = AT45DBxxxDSectorErase(tmp1);
        PrintResult(res);

        goto loop;
        break;

    case 4:		//Erase chip
        xUARTPutString("\r\nThe whole chip will be erased\n\r");
        res = AT45DBxxxDChipErase();
        PrintResult(res);
        goto loop;
        break;

    case 5:		//Restore all data
    	PrintDeviceInfo();
        goto loop;
        break;

    case 6:		//Read sector protection register
        xUARTPutString("\r\nThe sector protection register value are as follow:\n\r");
        res = AT45DBxxxDSecProtRegRead(Buff);
        PrintResult(res);
        if(!res)
        {
        	if((AT45DBxxxDInfo.usChipID & 0xFF) > 4)
        	{
                for(cnt = 0; cnt < 16;)
                {
        	    UartPrintfNumber(Buff[cnt]);
                    xUARTPutString("\t");
                    if(++cnt % 8 == 0)
                    xUARTPutString("\n\r");
                 }
        	}
        	else
        	{
        		for(cnt = 0; cnt < 8; cnt++)
        		{
        		    UartPrintfNumber(Buff[cnt]);
        		    xUARTPutString("\t");
        		}
        	}
        }
        xUARTPutString("\n\r");
        goto loop;
        break;

    case 7:		//Program sector protection register
        for(cnt = 0; cnt < MAXLEN; cnt++)
            Buff[cnt] = 0;
        for(cnt = 0; cnt < 4; cnt++)
        	Buff[cnt] = 0xFF;
        res = AT45DBxxxDSecProtRegWrite(Buff);
        PrintResult(res);
        goto loop;
        break;

    case 8:		//Enable Software Sector Protection
        res = AT45DBxxxDSectorProtectionEnable();
        PrintResult(res);
        goto loop;
        break;

    case 9:		//Disable Software Sector Protection
        res = AT45DBxxxDSectorProtectionDisable();
        PrintResult(res);
        goto loop;
        break;

    case 10:	//Test read internal buffer
        xUARTPutString("\r\nThe contents of internal buffer1 are:\n\r");
        AT45DBxxxDBufferRead(AT45DBxxxD_BUF1, Buff, 0, AT45DBxxxDInfo.usPageSize);
        for(cnt = 0; cnt < AT45DBxxxDInfo.usPageSize;)
        {
            UartPrintfNumber(Buff[cnt]);
            xUARTPutString("\t");
            if(++cnt % 10 == 0)
                xUARTPutString("\n\r");
            xSysCtlDelay(1000);
        }
        goto loop;
        break;

    case 11:	//Test Write internal buffer
        for(cnt = 0; cnt < AT45DBxxxDInfo.usPageSize; cnt++)
        {
            Buff[cnt] = 12;
        }
        AT45DBxxxDBufferWrite(AT45DBxxxD_BUF1, Buff, 0, AT45DBxxxDInfo.usPageSize);
        xUARTPutString("\r\nInternal buffer1 write over!\n\r");
        goto loop;
        break;
    case 12:
    	xUARTPutString("\r\nInput the byte address:>>");
    	tmp1 = UartGetNumber();
    	xUARTPutString("\r\nInput the length of data:>>");
    	cnt = UartGetNumber();
    	AT45DBxxxDRead(Buff, tmp1, cnt);
    	for(tmp1 = 0; tmp1 < cnt;)
    	{
    	    UartPrintfNumber(Buff[tmp1]);
    	    xUARTPutString("\t");
    	    if(++tmp1 % 10 == 0)
    	         xUARTPutString("\n\r");
    	     xSysCtlDelay(1000);
    	}
    	goto loop;
    	break;
    case 13:
    	xUARTPutString("\r\nInput the page address(0~");
    	UartPrintfNumber(AT45DBxxxDInfo.ulCapacity/AT45DBxxxDInfo.usPageSize - 1);
    	xUARTPutString(")>>");
    	tmp1 = UartGetNumber();
    	for(cnt = 0; cnt < MAXLEN; cnt+=2)
    	{
    	    Buff[cnt] = cnt/100;
    	    Buff[cnt+1] = (cnt+1)%100;
    	}
    	res = AT45DBxxxDPageWrite(tmp1, Buff);
    	PrintResult(res);
    	goto loop;
    	break;
    case 14:
    	for(cnt = 0; cnt < MAXLEN; cnt+=2)
    	{
    	    Buff[cnt] = cnt/100;
    	    Buff[cnt+1] = (cnt+1)%100;
    	}
    	xUARTPutString("\r\nInput the byte address(0~");
    	UartPrintfNumber(AT45DBxxxDInfo.ulCapacity - 1);
    	xUARTPutString(")>>");
    	tmp1 = UartGetNumber();
    	xUARTPutString("\r\nInput the number bytes:>>");
    	cnt = UartGetNumber();
    	res = AT45DBxxxDWrite(Buff, tmp1, cnt);
    	PrintResult(res);
    	goto loop;
    	break;
    default :
        //xUARTPutString("\r\n invalid command!\n\r");
        break;
    }

    while(1)
    {
        xUARTPutString("\r\n\nInput a page address to show(0~");
        UartPrintfNumber(AT45DBxxxDInfo.ulCapacity/AT45DBxxxDInfo.usPageSize - 1);
        xUARTPutString(")\n\r>>");

        tmp1 = UartGetNumber();
        if(tmp1 < AT45DBxxxDInfo.ulCapacity/AT45DBxxxDInfo.usPageSize)
        {
            xUARTPutString("\r\n\nThe page data are >>\n\n\r");
            //AT45DBxxxDRead(Buff, AT45DBxxxDInfo.usPageSize * tmp1, AT45DBxxxDInfo.usPageSize);
            AT45DBxxxDPageRead(tmp1, Buff, AT45DBxxxDInfo.usPageSize);
            for(cnt = 0; cnt < AT45DBxxxDInfo.usPageSize;)
            {
            	UartPrintfNumber(Buff[cnt]);
                xUARTPutString("\t");
                if(++cnt % 10 == 0)
                    xUARTPutString("\n\r");
                xSysCtlDelay(1000);
            }
        }
        else
        {
        	xUARTPutString("\r\nInvalid address!\n\r");
        }
        for(tmp1=0; tmp1<MAXLEN; tmp1++)
        {
        	Buff[tmp1] = 0;
        }
        xUARTPutString("\r\nGoto main menu? y/n >>");
        tmp1 = UartGetNumber();
        if(tmp1 == ('y'-'0')) goto loop;
    }
}
