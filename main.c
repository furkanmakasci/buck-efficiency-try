/*
 * File:   main.c
 * Author: assas
 *
 * Created on October 18, 2024, 11:08 PM
 */

//#define F_CPU   20000000UL
#include <xc.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//#include <avr/iotn402.h>


FUSES = {
.WDTCFG = 0x00, // WDTCFG {PERIOD=OFF, WINDOW=OFF}
.BODCFG = 0x00, // BODCFG {SLEEP=DIS, ACTIVE=DIS, SAMPFREQ=1KHz, LVL=BODLEVEL0}
.OSCCFG = 0x02, // OSCCFG {FREQSEL=20MHZ, OSCLOCK=CLEAR}
.TCD0CFG = 0x00, // TCD0CFG {CMPA=CLEAR, CMPB=CLEAR, CMPC=CLEAR, CMPD=CLEAR, CMPAEN=CLEAR, CMPBEN=CLEAR, CMPCEN=CLEAR, CMPDEN=CLEAR}
.SYSCFG0 = 0xF6, // SYSCFG0 {EESAVE=CLEAR, RSTPINCFG=UPDI, CRCSRC=NOCRC}
.SYSCFG1 = 0x07, // SYSCFG1 {SUT=64MS}
.APPEND = 0x00, // APPEND {APPEND=User range:  0x0 - 0xFF}
.BOOTEND = 0x00, // BOOTEND {BOOTEND=User range:  0x0 - 0xFF}
};
LOCKBITS = 0xC5; // {LB=NOLOCK}

unsigned int period=100;
unsigned int duty=96; //min1
unsigned int deadtime=1; //min1
//duty+deadtime < period-1

void main(void) {
   
    //????????????????
    CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLA = CLKCTRL_CLKSEL_OSC20M_gc;
    while(!(CLKCTRL.MCLKSTATUS & CLKCTRL_OSC20MS_bm));
    CCP = CCP_IOREG_gc;
    CLKCTRL.MCLKCTRLB = 0x00;
    CLKCTRL.OSC20MCTRLA = CLKCTRL_RUNSTDBY_bm;
    //????????????????
   
   
    CLKCTRL_MCLKCTRLA=0x01;
    CLKCTRL_MCLKCTRLB=0;
   
    PORTA_DIRSET = PIN5_bm;
    PORTB_DIRSET = PIN1_bm;
    PORTA_PIN5CTRL |= PORT_INVEN_bm;
   
    PORTA_DIRCLR = PIN2_bm;
    PORTA_DIRCLR = PIN3_bm;
    //PORTA_PIN2CTRL |= PORT_ISC_RISING_gc;
    //PORTA_PIN3CTRL |= PORT_ISC_RISING_gc;
    PORTA_PIN2CTRL |= PORT_PULLUPEN_bm;
    PORTA_PIN3CTRL |= PORT_PULLUPEN_bm;
    //sei();
   
    TCA0_SINGLE_CTRLA|= TCA_SINGLE_CLKSEL_DIV1_gc;
    TCA0_SINGLE_CTRLA|= TCA_SINGLE_ENABLE_bm;
    TCA0_SINGLE_CTRLB|=TCA_SINGLE_CMP1EN_bm;
    TCA0_SINGLE_CTRLB|=TCA_SINGLE_WGMODE_SINGLESLOPE_gc;
    TCA0_SINGLE_PER=period+deadtime-1;
    TCA0_SINGLE_CMP1=duty;
    TCA0_SINGLE_CNT=TCA0_SINGLE_CMP1;
   
    TCB0_CTRLA|=TCB_ENABLE_bm;
    //TCB0_CTRLA|=TCB_CLKSEL_CLKTCA_gc;
    TCB0_CTRLA|=TCB_SYNCUPD_bm;
    TCB0_CTRLB|=TCB_CNTMODE_PWM8_gc;
    TCB0_CTRLB|=TCB_CCMPEN_bm;
    TCB0_CCMPL=period;
    TCB0_CCMPH=duty+deadtime+1;
    TCB0_CNT=TCB0_CCMPH;
   
   
    while(1){
        if(!(PORTA_IN & (1 << 3))){
            duty++;
            sett();
            _delay_ms(250);
        }else if(!(PORTA_IN & (1 << 2))){
            duty--;
            sett();
            _delay_ms(250);
        }
       
    }
   
   
    return;
}

/*ISR(PORTA_PORT_vect){
    //
}*/

void sett(){
    if(duty+deadtime>period-1){
        duty=period-deadtime-1;
    }else if(duty<1){
        duty=1;
    }
   
    TCA0_SINGLE_CMP1=duty;
    TCA0_SINGLE_CNT=TCA0_SINGLE_CMP1;
   
    TCB0_CCMPH=duty+deadtime+1;
    TCB0_CNT=TCB0_CCMPH;
}
