#ifndef __OB38R16T1_H__
#define __OB38R16T1_H__

#ifndef __SDCC
#error "This header file must be used for SDCC compiler !"
#endif

/* redefine some keywords */

// primitive type
typedef __sfr sfr;
typedef __sfr16 sfr16;
typedef __sfr32 sfr32;
typedef __sbit sbit;
typedef __bit bit;

// keywords
#define interrupt __interrupt
#define using __using
#define _at_ __at
#define _priority_
#define _task_

// storage type
#define reentrant __reentrant
#define compact
#define small __near
#define large __far
#define data __data
#define bdata
#define idata __idata
#define pdata __pdata
#define xdata __xdata
#define code __code

/*  BYTE Registers  */
sfr __at(0x80) P0;
sfr __at(0x90) P1;
sfr __at(0xA0) P2;
sfr __at(0xB0) P3;
sfr __at(0xD0) PSW;
sfr __at(0xE0) ACC;
sfr __at(0xF0) B;
sfr __at(0x81) SP;
sfr __at(0x82) DPL;
sfr __at(0x83) DPH;
sfr __at(0x86) RCON;
sfr __at(0x87) PCON;
sfr __at(0x8E) CKCON;
sfr __at(0x8F) IFCON;
sfr __at(0x91) AUX;
sfr __at(0xA1) RSTS;
sfr __at(0xE6) LVC;
sfr __at(0xE7) SWRES;
sfr __at(0xF7) TAKEY;
sfr __at(0xA9) IP0;
sfr __at(0xB9) IP1;
sfr __at(0xA8) IE;
sfr __at(0xA9) IP;
sfr __at(0xA8) IEN0;
sfr __at(0xB8) IEN1;
sfr __at(0x9A) IEN2;
sfr __at(0xC0) IRCON;
sfr __at(0x97) IRCON2;

//---------GPIO Type ----------------
sfr __at(0xD2) P0M0;
sfr __at(0xD3) P0M1;
sfr __at(0xD4) P1M0;
sfr __at(0xD5) P1M1;
sfr __at(0xD6) P2M0;
sfr __at(0xD7) P2M1;
sfr __at(0xDA) P3M0;
sfr __at(0xDB) P3M1;

//---------Timer---------------------
sfr __at(0x88) TCON;
sfr __at(0xC8) T2CON;
sfr __at(0x89) TMOD;
sfr __at(0x8A) TL0;
sfr __at(0x8B) TL1;
sfr __at(0x8C) TH0;
sfr __at(0x8D) TH1;
sfr __at(0xCC) TL2;
sfr __at(0xCD) TH2;
sfr __at(0xD9) PFCON;
sfr __at(0xE5) ENHIT;
sfr __at(0xEE) INTDEG;
sfr16 __at(0xCC) T2;

//----------CCU----------------------
sfr __at(0xC1) CCEN;
sfr __at(0xD1) CCEN2;
sfr __at(0xC9) CCCON;
sfr __at(0xCA) CRCL;
sfr __at(0xCB) CRCH;
sfr __at(0xC2) CCL1;
sfr __at(0xC3) CCH1;
sfr __at(0xC4) CCL2;
sfr __at(0xC5) CCH2;
sfr __at(0xC6) CCL3;
sfr __at(0xC7) CCH3;
sfr16 __at(0xCA) CRC;
sfr16 __at(0xC2) CC1;
sfr16 __at(0xC4) CC2;
sfr16 __at(0xC6) CC3;

//----------PWM----------------------
sfr __at(0xB5) PWMC;
sfr __at(0xCE) PWMMDL;
sfr __at(0xCF) PWMMDH;
sfr __at(0xBC) PWMD0L;
sfr __at(0xBD) PWMD0H;
sfr __at(0xBE) PWMD1L;
sfr __at(0xBF) PWMD1H;
sfr __at(0xB1) PWMD2L;
sfr __at(0xB2) PWMD2H;
sfr __at(0xB3) PWMD3L;
sfr __at(0xB4) PWMD3H;
sfr __at(0xDD) PWMC2;
sfr __at(0xDE) PWMDT0;
sfr __at(0xDF) PWMDT1;
sfr16 __at(0xCE) PWMMD;
sfr16 __at(0xBC) PWMD0;
sfr16 __at(0xBE) PWMD1;
sfr16 __at(0xB1) PWMD2;
sfr16 __at(0xB3) PWMD3;

//----------WDT----------------------
sfr __at(0xB6) WDTRC;
sfr __at(0xFF) WDTIC;
sfr __at(0xB7) WDTK;

//----------ISP----------------------
sfr __at(0xE1) ISPFAH;
sfr __at(0xE2) ISPFAL;
sfr __at(0xE3) ISPFDL;
sfr __at(0xEB) ISPFDH;
sfr __at(0xE4) ISPFC;

//----------ADC---------------------
sfr __at(0xAB) ADCC1;
sfr __at(0xAC) ADCC2;
sfr __at(0xAD) ADCDH;
sfr __at(0xAE) ADCDL;
sfr __at(0xAF) ADCCS;
sfr __at(0xEF) ADCSH;

//------------UART-------------------------
sfr __at(0x98) SCON;
sfr __at(0x99) SBUF;
sfr __at(0x98) S0CON;
sfr __at(0x99) S0BUF;
sfr __at(0xAA) S0RELL;
sfr __at(0xBA) S0RELH;
sfr __at(0xAA) SRELL;
sfr __at(0xBA) SRELH;

//-------------IIC----------------------
sfr __at(0xF8) IICS;
sfr __at(0xF9) IICCTL;
sfr __at(0xFA) IICA1;
sfr __at(0xFB) IICA2;
sfr __at(0xFC) IICRWD;
sfr __at(0xFD) IICEBT;

//----------Touch-------------------
sfr __at(0x93) TKEN0;
sfr __at(0x94) TKEN1;
sfr __at(0x9B) TKCON;
sfr __at(0x9C) TKRUNTIME;
sfr __at(0x9D) TKCHN;
sfr __at(0x9E) TKCDL;
sfr __at(0x9F) TKCDH;
sfr16 __at(0x9E) TKCD_16;
sfr __at(0xBB) TKSTATUS0;
sfr __at(0x85) TKSTATUS1;
sfr __at(0xF2) TKPSSR;
sfr __at(0xF3) TKWKTRICNT;
sfr __at(0xFE) TKSW;

//---------Xdata -----------
#define pTKTRIG_16 ((unsigned int volatile xdata *)0x01D0)
#define TK0TRIG_16 (*((unsigned int volatile xdata *)0x01D0))
#define TK1TRIG_16 (*((unsigned int volatile xdata *)0x01D2))
#define TK2TRIG_16 (*((unsigned int volatile xdata *)0x01D4))
#define TK3TRIG_16 (*((unsigned int volatile xdata *)0x01D6))
#define TK4TRIG_16 (*((unsigned int volatile xdata *)0x01D8))
#define TK5TRIG_16 (*((unsigned int volatile xdata *)0x01DA))
#define TK6TRIG_16 (*((unsigned int volatile xdata *)0x01DC))
#define TK7TRIG_16 (*((unsigned int volatile xdata *)0x01DE))
#define TK8TRIG_16 (*((unsigned int volatile xdata *)0x01E0))
#define TK9TRIG_16 (*((unsigned int volatile xdata *)0x01E2))
#define TK10TRIG_16 (*((unsigned int volatile xdata *)0x01E4))
#define TK11TRIG_16 (*((unsigned int volatile xdata *)0x01E6))
#define TK12TRIG_16 (*((unsigned int volatile xdata *)0x01E8))
#define TK13TRIG_16 (*((unsigned int volatile xdata *)0x01EA))
#define TK14TRIG_16 (*((unsigned int volatile xdata *)0x01EC))
#define TK15TRIG_16 (*((unsigned int volatile xdata *)0x01EE))

#define pTKTRICNT ((unsigned char volatile xdata *)0x01F0)
#define TK0TRICNT (*((unsigned char volatile xdata *)0x01F0))
#define TK1TRICNT (*((unsigned char volatile xdata *)0x01F1))
#define TK2TRICNT (*((unsigned char volatile xdata *)0x01F2))
#define TK3TRICNT (*((unsigned char volatile xdata *)0x01F3))
#define TK4TRICNT (*((unsigned char volatile xdata *)0x01F4))
#define TK5TRICNT (*((unsigned char volatile xdata *)0x01F5))
#define TK6TRICNT (*((unsigned char volatile xdata *)0x01F6))
#define TK7TRICNT (*((unsigned char volatile xdata *)0x01F7))
#define TK8TRICNT (*((unsigned char volatile xdata *)0x01F8))
#define TK9TRICNT (*((unsigned char volatile xdata *)0x01F9))
#define TK10TRICNT (*((unsigned char volatile xdata *)0x01FA))
#define TK11TRICNT (*((unsigned char volatile xdata *)0x01FB))
#define TK12TRICNT (*((unsigned char volatile xdata *)0x01FC))
#define TK13TRICNT (*((unsigned char volatile xdata *)0x01FD))
#define TK14TRICNT (*((unsigned char volatile xdata *)0x01FE))
#define TK15TRICNT (*((unsigned char volatile xdata *)0x01FF))
//---------ADC---------------
#define ADCCAL (*((unsigned char volatile xdata *)0xFFFF))

//---------LED---------------
#define COMEN (*((unsigned char volatile xdata *)0xFF1C))
#define SEGEN0 (*((unsigned char volatile xdata *)0xFF1D))
#define SEGEN1 (*((unsigned char volatile xdata *)0xFF1E))
#define LEDCON (*((unsigned char volatile xdata *)0xFF1F))
#define LEDCLK (*((unsigned char volatile xdata *)0xFF20))

#define COM0_A_16 (*((unsigned int volatile xdata *)0xFF00))
#define COM1_A_16 (*((unsigned int volatile xdata *)0xFF02))
#define COM2_A_16 (*((unsigned int volatile xdata *)0xFF04))
#define COM3_A_16 (*((unsigned int volatile xdata *)0xFF06))
#define COM4_A_16 (*((unsigned int volatile xdata *)0xFF08))
#define COM5_A_16 (*((unsigned int volatile xdata *)0xFF0A))
#define COM6_A_16 (*((unsigned int volatile xdata *)0xFF0C))
#define COM0_C_16 (*((unsigned int volatile xdata *)0xFF0E))
#define COM1_C_16 (*((unsigned int volatile xdata *)0xFF10))
#define COM2_C_16 (*((unsigned int volatile xdata *)0xFF12))
#define COM3_C_16 (*((unsigned int volatile xdata *)0xFF14))
#define COM4_C_16 (*((unsigned int volatile xdata *)0xFF16))
#define COM5_C_16 (*((unsigned int volatile xdata *)0xFF18))
#define COM6_C_16 (*((unsigned int volatile xdata *)0xFF1A))
#define pCOM_A_16 ((unsigned int volatile xdata *)0xFF00)
#define pCOM_C_16 ((unsigned int volatile xdata *)0xFF0E)

//---------Xdata End-----------

sbit __at(0xE0) ACC0;
sbit __at(0xE1) ACC1;
sbit __at(0xE2) ACC2;
sbit __at(0xE3) ACC3;
sbit __at(0xE4) ACC4;
sbit __at(0xE5) ACC5;
sbit __at(0xE6) ACC6;
sbit __at(0xE7) ACC7;

sbit __at(0x80) P0_0;
sbit __at(0x81) P0_1;
sbit __at(0x82) P0_2;
sbit __at(0x83) P0_3;
sbit __at(0x84) P0_4;
sbit __at(0x85) P0_5;
sbit __at(0x86) P0_6;
sbit __at(0x87) P0_7;

sbit __at(0x90) P1_0;
sbit __at(0x91) P1_1;
sbit __at(0x92) P1_2;
sbit __at(0x93) P1_3;
sbit __at(0x94) P1_4;
sbit __at(0x95) P1_5;
sbit __at(0x96) P1_6;
sbit __at(0x97) P1_7;

sbit __at(0xA3) P2_3;
sbit __at(0xA4) P2_4;
sbit __at(0xA5) P2_5;
sbit __at(0xA6) P2_6;
sbit __at(0xB0) P3_0;
sbit __at(0xB1) P3_1;
sbit __at(0xB2) P3_2;
sbit __at(0xB3) P3_3;
sbit __at(0xB4) P3_4;
sbit __at(0xB5) P3_5;

sbit __at(0xAF) EA;
sbit __at(0xAD) ET2;
sbit __at(0xAC) ES;
sbit __at(0xAB) ET1;
sbit __at(0xAA) EX1;
sbit __at(0xA9) ET0;
sbit __at(0xA8) EX0;

sbit __at(0xBF) EXEN2;
sbit __at(0xBD) IEIIC;
sbit __at(0xBC) IELVI;
sbit __at(0xBA) IEADC;
sbit __at(0xB8) IEPWM;

sbit __at(0xC7) EXF2;
sbit __at(0xC6) TF2;
sbit __at(0xC5) IICIF;
sbit __at(0xC4) LVIIF;
sbit __at(0xC2) ADCIF;
sbit __at(0xC0) PWMIF;

sbit __at(0x9F) SM0;
sbit __at(0x9E) SM1;
sbit __at(0x9D) SM2;
sbit __at(0x9C) REN;
sbit __at(0x9B) TB8;
sbit __at(0x9A) RB8;
sbit __at(0x99) TI;
sbit __at(0x98) RI;

/* PSW */
sbit __at(0xD7) CY;
sbit __at(0xD6) AC;
sbit __at(0xD5) F0;
sbit __at(0xD4) RS1;
sbit __at(0xD3) RS0;
sbit __at(0xD2) OV;
sbit __at(0xD1) F1;
sbit __at(0xD0) P;

/*  TCON  */
sbit __at(0x8F) TF1;
sbit __at(0x8E) TR1;
sbit __at(0x8D) TF0;
sbit __at(0x8C) TR0;
sbit __at(0x8B) IE1;
sbit __at(0x8A) IT1;
sbit __at(0x89) IE0;
sbit __at(0x88) IT0;

//-------------------------------------------
sbit __at(0xFE) MPIF;
sbit __at(0xFD) LAIF;
sbit __at(0xFC) RXIF;
sbit __at(0xFB) TXIF;
sbit __at(0xFA) RXAK;
sbit __at(0xF9) TXAK;
sbit __at(0xF8) RW;
sbit __at(0xF8) BB;
//---------------------------------------------
#define dCOM0 P1_5
#define dCOM1 P1_4
#define dCOM2 P1_3
#define dCOM3 P1_2
#define dCOM4 P0_0
#define dCOM5 P0_1
#define dCOM6 P2_4

#define dSEG0_0 P0_0
#define dSEG1_0 P0_1
#define dSEG0_1 P3_0
#define dSEG1_1 P3_1
#define dSEG2 P0_2
#define dSEG3 P0_3
#define dSEG4 P0_4
#define dSEG5 P0_5
#define dSEG6 P0_6

#define dSEG7 P0_7
#define dSEG8 P1_0
#define dSEG9 P1_1
#define dSEG10 P2_3
#define dSEG11 P3_2
#define dSEG12 P3_3
#define dSEG13 P3_4
#define dSEG14 P1_6
#define dSEG15 P1_7

// Interrupt Define
#define d_INT0_Vector (0)  // 0003h
#define d_T0_Vector (1)    // 000Bh
#define d_INT1_Vector (2)  // 0013h
#define d_T1_Vector (3)    // 001Bh
#define d_UART_Vector (4)  // 0023h
#define d_UART0_Vector (4) // 0023h
#define d_T2_Vector (5)    // 002Bh
#define d_PWM_Vector (8)   // 0043h
#define d_ADC_Vector (10)  // 0053h
#define d_LVI_Vector (12)  // 0063h
#define d_IIC0_Vector (13) // 006Bh
#define d_IIC_Vector (13)  // 006Bh
#define d_WDT_Vector (17)  // 008Bh
#define d_TK_Vector (19)   // 009Bh

#endif
