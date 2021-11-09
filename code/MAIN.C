/*********************************************************************
    Project:MG82F6D17-DEMO
    Author:LZD
			MG82F6D17 SSOP20_V10 EV Board (TH194A)
			CpuCLK=12MHz, SysCLK=12MHz
		Description:

			T3 mode: 8bit PWM, output to P33 
		

*********************************************************************/
#define _MAIN_C

#include <Intrins.h>
#include <Absacc.h>

#include <Stdio.h>  // for printf

#include ".\include\REG_MG82F6D17.H"
#include ".\include\Type.h"
#include ".\include\API_Macro_MG82F6D17.H"
#include ".\include\API_Uart_BRGRL_MG82F6D17.H"


/*************************************************
Различные варианты частоты тактирования 

Set SysClk (MAX.50MHz) (MAX.50MHz)
Selection: 
	11059200,12000000,
	22118400,24000000,
	29491200,32000000,
	44236800,48000000
*************************************************/
#define MCU_SYSCLK		12000000 // использование внутреннего PC-генератора 12МГц
/*************************************************/
/*************************************************
Set  CpuClk (MAX.36MHz)
	1) CpuCLK=SysCLK
	2) CpuClk=SysClk/2
*************************************************/
#define MCU_CPUCLK		(MCU_SYSCLK)
//#define MCU_CPUCLK		(MCU_SYSCLK/2)

#define LED_R		P34
#define LED_G_1		P35

#define IO_T3_PWM_OUT	P33

#define T3_PWM_STEP_MAX	256		// 2~256 40 - 25.0kHz, 200 - 5.0kHz,

#define T3_PWM_DUTY_0	((256-T3_PWM_STEP_MAX)+0)	
#define T3_PWM_DUTY_10	((256-T3_PWM_STEP_MAX)+(10*T3_PWM_STEP_MAX)/100)	
#define T3_PWM_DUTY_30	((256-T3_PWM_STEP_MAX)+(30*T3_PWM_STEP_MAX)/100)	
#define T3_PWM_DUTY_60	((256-T3_PWM_STEP_MAX)+(60*T3_PWM_STEP_MAX)/100)	
#define T3_PWM_DUTY_80	((256-T3_PWM_STEP_MAX)+(80*T3_PWM_STEP_MAX)/100)	


/*************************************************
Функция: void DelayXus(u16 xUs)
Для: описания настроек задержки в зависимости от частоты тактирования, при использовании просто задать в скобках время в микросекундах.

Description:   	dealy��unit:us
Input:     		u8 Us -> *1us  (1~255)   
*************************************************/
void DelayXus(u8 xUs)
{
	while(xUs!=0)
	{
#if (MCU_CPUCLK>=11059200)
		_nop_();
#endif
#if (MCU_CPUCLK>=14745600)
		_nop_();
		_nop_();
		_nop_();
		_nop_();
#endif
#if (MCU_CPUCLK>=16000000)
		_nop_();
#endif

#if (MCU_CPUCLK>=22118400)
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
#endif
#if (MCU_CPUCLK>=24000000)
		_nop_();
		_nop_();
#endif		
#if (MCU_CPUCLK>=29491200)
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
#endif
#if (MCU_CPUCLK>=32000000)
		_nop_();
		_nop_();
#endif

		xUs--;
	}
}

/*************************************************
Функция: void DelayXms(u16 xMs)
Для: описания настроек задержки в миллисекундах

Description:    dealy��unit:ms
Input:     		u16 xMs -> *1ms  (1~65535)
   
*************************************************/
void DelayXms(u16 xMs)
{
	while(xMs!=0)
	{
		CLRWDT();
		DelayXus(200);
		DelayXus(200);
		DelayXus(200);
		DelayXus(200);
		DelayXus(200);
		xMs--;
		
	}
}

/***********************************************************************************
Функция: void InitTimer3_8bit_PWM(void)
Для: описания настроек таймера Т3
     
*************************************************************************************/
void InitTimer3_8bit_PWM(void)
{
	TM_SetT3Mode_8bitPWM();							// T3 mode: 8bit PWM - настраиваем, как 8-битный ШИМ
	TM_SetT3Clock_SYSCLKDiv12();				// T3 clock source: SYSCLK/12 - задаем тактирование 
	
	TM_SetT3LowByte(T3_PWM_DUTY_30);				// T3 low byte	(duty) -  			
  TM_SetT3RLLowByte(T3_PWM_DUTY_30);				// T3 low byte reload  (duty)

	TM_SetT3HighByte(256-T3_PWM_STEP_MAX);			// T3 high byte	(freq)  
  TM_SetT3RLHighByte(256-T3_PWM_STEP_MAX);		// T3 high byte	reload (freq)

	TM_EnableT3();									// Enable T3 - включение таймера Т3
	
	//TM_EnableT3CKO_Invert();
	TM_DisT3CKO();									// disable T3CKO output - выключение таймера Т3 
}

/***********************************************************************************
Функция: void InitPort()
Для: описания инициализации портов

на этапе разработки в качестве настоечных пинов буду использовать P30,P31
в конечном проекте - P44,P45 (они на одной строне) - сейчас эти пины задействованы в отладке.
 		
*************************************************************************************/
void InitPort(void)
{

	IO_T3_PWM_OUT=1; //При включении на выходном пине 1(соответствует 0 состоянию выходного ключа)
	
	PORT_SetP3QuasiBi(BIT0|BIT1);			// set P30,P31 as Quasi-Bidirectional - устанавливаем как квази-бинарные
	PORT_SetP3PushPull(BIT3);					// Set P33 as Push-Pull for T3 PWM ouptput - порт Р33 на выход ШИМ
}

/***********************************************************************************
Функция: void InitClock()
Для: описания инициализации тактирования, лишнее потом уберу  
  		
*************************************************************************************/
void InitClock(void)
{
#if (MCU_SYSCLK==11059200)
#if (MCU_CPUCLK==MCU_SYSCLK)
	// SysClk=11.0592MHz CpuClk=11.0592MHz
	CLK_SetCKCON0(IHRCO_110592MHz|CPUCLK_SYSCLK_DIV_1|SYSCLK_MCKDO_DIV_1);
	
#else
	// SysClk=11.0592MHz CpuClk=5.5296MHz
	CLK_SetCKCON0(IHRCO_110592MHz|CPUCLK_SYSCLK_DIV_2|SYSCLK_MCKDO_DIV_1);
#endif
#endif

#if (MCU_SYSCLK==12000000)
#if (MCU_CPUCLK==MCU_SYSCLK)
	// SysClk=12MHz CpuClk=12MHz
	CLK_SetCKCON0(IHRCO_12MHz|CPUCLK_SYSCLK_DIV_1|SYSCLK_MCKDO_DIV_1);
	
#else
	// SysClk=12MHz CpuClk=6MHz
	CLK_SetCKCON0(IHRCO_12MHz|CPUCLK_SYSCLK_DIV_2|SYSCLK_MCKDO_DIV_1);
#endif
#endif

#if (MCU_SYSCLK==22118400)
#if (MCU_CPUCLK==MCU_SYSCLK)
	// SysClk=22.1184MHz CpuClk=22.1184MHz
	CLK_SetCKCON0(IHRCO_110592MHz|CPUCLK_SYSCLK_DIV_1|SYSCLK_MCKDO_DIV_1|ENABLE_CKM|CKM_OSCIN_DIV_2);
	DelayXus(100);
	// IHRCO, MCK=CKMIx4, OSCin=IHRCO
	CLK_SetCKCON2(ENABLE_IHRCO|MCK_CKMI_X4|OSCIn_IHRCO);
#else
	// SysClk=22.1184MHz CpuClk=11.0592MHz
	CLK_SetCKCON0(IHRCO_110592MHz|CPUCLK_SYSCLK_DIV_2|SYSCLK_MCKDO_DIV_1|ENABLE_CKM|CKM_OSCIN_DIV_2);
	DelayXus(100);
	// IHRCO, MCK=CKMIx4, OSCin=IHRCO
	CLK_SetCKCON2(ENABLE_IHRCO|MCK_CKMI_X4|OSCIn_IHRCO);
#endif
#endif

#if (MCU_SYSCLK==24000000)
#if (MCU_CPUCLK==MCU_SYSCLK)
	// SysClk=24MHz CpuClk=24MHz
	CLK_SetCKCON0(IHRCO_12MHz|CPUCLK_SYSCLK_DIV_1|SYSCLK_MCKDO_DIV_1|ENABLE_CKM|CKM_OSCIN_DIV_2);
	DelayXus(100);
	// IHRCO, MCK=CKMIx4, OSCin=IHRCO
	CLK_SetCKCON2(ENABLE_IHRCO|MCK_CKMI_X4|OSCIn_IHRCO);
#else
	// SysClk=24MHz CpuClk=12MHz
	CLK_SetCKCON0(IHRCO_12MHz|CPUCLK_SYSCLK_DIV_2|SYSCLK_MCKDO_DIV_1|ENABLE_CKM|CKM_OSCIN_DIV_2);
	DelayXus(100);
	// IHRCO, MCK=CKMIx4, OSCin=IHRCO
	CLK_SetCKCON2(ENABLE_IHRCO|MCK_CKMI_X4|OSCIn_IHRCO);
#endif
#endif

#if (MCU_SYSCLK==29491200)
#if (MCU_CPUCLK==MCU_SYSCLK)
	// SysClk=29.491200MHz CpuClk=29.491200MHz
	CLK_SetCKCON0(IHRCO_110592MHz|CPUCLK_SYSCLK_DIV_1|SYSCLK_MCKDO_DIV_1|ENABLE_CKM|CKM_OSCIN_DIV_2);
	DelayXus(100);
	// IHRCO, MCK=CKMIx5.33, OSCin=IHRCO
	CLK_SetCKCON2(ENABLE_IHRCO|MCK_CKMI_X533|OSCIn_IHRCO);
#else
	// SysClk=29.491200MHz CpuClk=14.7456MHz
	CLK_SetCKCON0(IHRCO_110592MHz|CPUCLK_SYSCLK_DIV_2|SYSCLK_MCKDO_DIV_1|ENABLE_CKM|CKM_OSCIN_DIV_2);
	DelayXus(100);
	// IHRCO, MCK=CKMIx5.33, OSCin=IHRCO
	CLK_SetCKCON2(ENABLE_IHRCO|MCK_CKMI_X533|OSCIn_IHRCO);
#endif
#endif

#if (MCU_SYSCLK==32000000)
#if (MCU_CPUCLK==MCU_SYSCLK)
	// SysClk=32MHz CpuClk=32MHz
	CLK_SetCKCON0(IHRCO_12MHz|CPUCLK_SYSCLK_DIV_1|SYSCLK_MCKDO_DIV_1|ENABLE_CKM|CKM_OSCIN_DIV_2);
	DelayXus(100);
	// IHRCO, MCK=CKMIx5.33, OSCin=IHRCO
	CLK_SetCKCON2(ENABLE_IHRCO|MCK_CKMI_X533|OSCIn_IHRCO);
#else
	// SysClk=32MHz CpuClk=16MHz
	CLK_SetCKCON0(IHRCO_12MHz|CPUCLK_SYSCLK_DIV_2|SYSCLK_MCKDO_DIV_1|ENABLE_CKM|CKM_OSCIN_DIV_2);
	DelayXus(100);
	// IHRCO, MCK=CKMIx5.33, OSCin=IHRCO
	CLK_SetCKCON2(ENABLE_IHRCO|MCK_CKMI_X533|OSCIn_IHRCO);
#endif
#endif


#if (MCU_SYSCLK==44236800)
	// SysClk=44.2368MHz CpuClk=22.1184MHz
	CLK_SetCKCON0(IHRCO_110592MHz|CPUCLK_SYSCLK_DIV_1|SYSCLK_MCKDO_DIV_1|ENABLE_CKM|CKM_OSCIN_DIV_2);
	DelayXus(100);
	// IHRCO, MCK=CKMIx8, OSCin=IHRCO
	CLK_SetCKCON2(ENABLE_IHRCO|MCK_CKMI_X8|OSCIn_IHRCO);
#endif

#if (MCU_SYSCLK==48000000)
	// SysClk=48MHz CpuClk=24MHz
	CLK_SetCKCON0(IHRCO_12MHz|CPUCLK_SYSCLK_DIV_2|SYSCLK_MCKDO_DIV_1|ENABLE_CKM|CKM_OSCIN_DIV_2);
	DelayXus(100);
	// IHRCO, MCK=CKMIx8, OSCin=IHRCO
	CLK_SetCKCON2(ENABLE_IHRCO|MCK_CKMI_X8|OSCIn_IHRCO);
#endif
}

/***********************************************************************************
Функция: void InitSystem(void)
Для: инициализации всей системы - здесь вызываем те функции, которые описали выше - порты, часы, ШИМ 

*************************************************************************************/
void InitSystem(void)
{
	InitPort();
	InitClock();		
	InitTimer3_8bit_PWM();			
}


void main() // главная функция 
{
	u8 i; // вводим переменную i, u8 - тип данных.
	
  InitSystem();
	
 	LED_G_1=0;LED_R=0;
	DelayXms(1000);
 	LED_G_1=1;LED_R=1;

					
	while(1) // бесконечный цикл
    {   	
			for(i=0;i<120;i++) // цикл от 0 до 120 (невключительно) - изменяем параметр для плавного воздействия на выходной сигнал.
    	{
				
			TM_EnT3CKO(); // включаем выход таймера
    	DelayXms(2); // ставим задержку
				
			TM_SetT3HighByte(256-(T3_PWM_STEP_MAX-i));			// T2 high byte	(freq) - частота ШИМ сигнала плавно увеличивается   
  		TM_SetT3RLHighByte(256-(T3_PWM_STEP_MAX-i));		// T2 high byte	reload (freq)
			DelayXms(2); // ставим задержку, чтоб лучше было видно на осцилографе 
				
			TM_SetT3LowByte(((256-(T3_PWM_STEP_MAX-i))+(30*(T3_PWM_STEP_MAX-i))/100));	 // T2 low byte	(duty) - корректировка скважности в процессе изменения частоты   			
      TM_SetT3RLLowByte(((256-(T3_PWM_STEP_MAX-i))+(30*(T3_PWM_STEP_MAX-i))/100)); // T2 low byte reload  (duty)
				
			}	    	
    }
}
