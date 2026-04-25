#ifndef __OB38R16T1_H__
#define __OB38R16T1_H__

#ifndef __SDCC
#error "This header file must be used for SDCC compiler !"
#endif

#include <compiler.h>

#ifndef MICROCONTROLLER_DEFINED 
#define MICROCONTROLLER_DEFINED
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
SFR(P0, 0x80);
SFR(P1, 0x90);
SFR(P2, 0xA0);
SFR(P3, 0xB0);
SFR(PSW, 0xD0);
SFR(ACC, 0xE0);
SFR(B, 0xF0);
SFR(SP, 0x81);
SFR(DPL, 0x82);
SFR(DPH, 0x83);
SFR(RCON, 0x86);
SFR(PCON, 0x87);
SFR(CKCON, 0x8E);
SFR(IFCON, 0x8F);
SFR(AUX, 0x91);
SFR(RSTS, 0xA1);
SFR(LVC, 0xE6);
SFR(SWRES, 0xE7);
SFR(TAKEY, 0xF7);
SFR(IP0, 0xA9);
SFR(IP1, 0xB9);
SFR(IE, 0xA8);
SFR(IP, 0xA9);
SFR(IEN0, 0xA8);
SFR(IEN1, 0xB8);
SFR(IEN2, 0x9A);
SFR(IRCON, 0xC0);
SFR(IRCON2, 0x97);

//---------GPIO Type ----------------
SFR(P0M0, 0xD2);
SFR(P0M1, 0xD3);
SFR(P1M0, 0xD4);
SFR(P1M1, 0xD5);
SFR(P2M0, 0xD6);
SFR(P2M1, 0xD7);
SFR(P3M0, 0xDA);
SFR(P3M1, 0xDB);

//---------Timer---------------------
SFR(TCON, 0x88);
SFR(T2CON, 0xC8);
SFR(TMOD, 0x89);
SFR(TL0, 0x8A);
SFR(TL1, 0x8B);
SFR(TH0, 0x8C);
SFR(TH1, 0x8D);
SFR(TL2, 0xCC);
SFR(TH2, 0xCD);
SFR(PFCON, 0xD9);
SFR(ENHIT, 0xE5);
SFR(INTDEG, 0xEE);
SFR16(T2, 0xCC);

//----------CCU----------------------
SFR(CCEN, 0xC1);
SFR(CCEN2, 0xD1);
SFR(CCCON, 0xC9);
SFR(CRCL, 0xCA);
SFR(RCAP2L, 0xCA);
SFR(CRCH, 0xCB);
SFR(RCAP2H, 0xCB);
SFR(CCL1, 0xC2);
SFR(CCH1, 0xC3);
SFR(CCL2, 0xC4);
SFR(CCH2, 0xC5);
SFR(CCL3, 0xC6);
SFR(CCH3, 0xC7);
SFR16(CRC, 0xCA);
SFR16(CC1, 0xC2);
SFR16(CC2, 0xC4);
SFR16(CC3, 0xC6);
//----------PWM----------------------
SFR(PWMC, 0xB5);
SFR(PWMMDL, 0xCE);
SFR(PWMMDH, 0xCF);
SFR(PWMD0L, 0xBC);
SFR(PWMD0H, 0xBD);
SFR(PWMD1L, 0xBE);
SFR(PWMD1H, 0xBF);
SFR(PWMD2L, 0xB1);
SFR(PWMD2H, 0xB2);
SFR(PWMD3L, 0xB3);
SFR(PWMD3H, 0xB4);
SFR(PWMC2, 0xDD);
SFR(PWMDT0, 0xDE);
SFR(PWMDT1, 0xDF);
SFR16(PWMMD, 0xCE);
SFR16(PWMD0, 0xBC);
SFR16(PWMD1, 0xBE);
SFR16(PWMD2, 0xB1);
SFR16(PWMD3, 0xB3);

//----------WDT----------------------
SFR(WDTRC, 0xB6);
SFR(WDTIC, 0xFF);
SFR(WDTK, 0xB7);

//----------ISP----------------------
SFR(ISPFAH, 0xE1);
SFR(ISPFAL, 0xE2);
SFR(ISPFDL, 0xE3);
SFR(ISPFDH, 0xEB);
SFR(ISPFC, 0xE4);

//----------ADC---------------------
SFR(ADCC1, 0xAB);
SFR(ADCC2, 0xAC);
SFR(ADCDH, 0xAD);
SFR(ADCDL, 0xAE);
SFR(ADCCS, 0xAF);
SFR(ADCSH, 0xEF);

//------------UART-------------------------
SFR(SCON, 0x98);
SFR(SBUF, 0x99);
SFR(S0CON, 0x98);
SFR(S0BUF, 0x99);
SFR(S0RELL, 0xAA);
SFR(S0RELH, 0xBA);
SFR(SRELL, 0xAA);
SFR(SRELH, 0xBA);

//-------------IIC----------------------
SFR(IICS, 0xF8);
SFR(IICCTL, 0xF9);
SFR(IICA1, 0xFA);
SFR(IICA2, 0xFB);
SFR(IICRWD, 0xFC);
SFR(IICEBT, 0xFD);

//----------Touch-------------------
SFR(TKEN0, 0x93);
SFR(TKEN1, 0x94);
SFR(TKCON, 0x9B);
SFR(TKRUNTIME, 0x9C);
SFR(TKCHN, 0x9D);
SFR(TKCDL, 0x9E);
SFR(TKCDH, 0x9F);
SFR16(TKCD_16, 0x9E);
SFR(TKSTATUS0, 0xBB);
SFR(TKSTATUS1, 0x85);
SFR(TKPSSR, 0xF2);
SFR(TKWKTRICNT, 0xF3);
SFR(TKSW, 0xFE);

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

SBIT(ACC0, 0xE0 /* ACC */, 0);
SBIT(ACC1, 0xE0 /* ACC */, 1);
SBIT(ACC2, 0xE0 /* ACC */, 2);
SBIT(ACC3, 0xE0 /* ACC */, 3);
SBIT(ACC4, 0xE0 /* ACC */, 4);
SBIT(ACC5, 0xE0 /* ACC */, 5);
SBIT(ACC6, 0xE0 /* ACC */, 6);
SBIT(ACC7, 0xE0 /* ACC */, 7);

SBIT(P0_0, 0x80 /* P0 */, 0);
SBIT(P0_1, 0x80 /* P0 */, 1);
SBIT(P0_2, 0x80 /* P0 */, 2);
SBIT(P0_3, 0x80 /* P0 */, 3);
SBIT(P0_4, 0x80 /* P0 */, 4);
SBIT(P0_5, 0x80 /* P0 */, 5);
SBIT(P0_6, 0x80 /* P0 */, 6);
SBIT(P0_7, 0x80 /* P0 */, 7);

SBIT(P1_0, 0x90 /* P1 */, 0);
SBIT(P1_1, 0x90 /* P1 */, 1);
SBIT(P1_2, 0x90 /* P1 */, 2);
SBIT(P1_3, 0x90 /* P1 */, 3);
SBIT(P1_4, 0x90 /* P1 */, 4);
SBIT(P1_5, 0x90 /* P1 */, 5);
SBIT(P1_6, 0x90 /* P1 */, 6);
SBIT(P1_7, 0x90 /* P1 */, 7);

SBIT(P2_3, 0xA0 /* P2 */, 3);
SBIT(P2_4, 0xA0 /* P2 */, 4);
SBIT(P2_5, 0xA0 /* P2 */, 5);
SBIT(P2_6, 0xA0 /* P2 */, 6);
SBIT(P3_0, 0xB0 /* P3 */, 0);
SBIT(P3_1, 0xB0 /* P3 */, 1);
SBIT(P3_2, 0xB0 /* P3 */, 2);
SBIT(P3_3, 0xB0 /* P3 */, 3);
SBIT(P3_4, 0xB0 /* P3 */, 4);
SBIT(P3_5, 0xB0 /* P3 */, 5);

SBIT(EA, 0xA8 /* IEN0 */, 7);
SBIT(ET2, 0xA8 /* IEN0 */, 5);
SBIT(ES, 0xA8 /* IEN0 */, 4);
SBIT(ET1, 0xA8 /* IEN0 */, 3);
SBIT(EX1, 0xA8 /* IEN0 */, 2);
SBIT(ET0, 0xA8 /* IEN0 */, 1);
SBIT(EX0, 0xA8 /* IEN0 */, 0);

SBIT(EXEN2, 0xB8 /* IEN1 */, 7);
SBIT(IEIIC, 0xB8 /* IEN1 */, 5);
SBIT(IELVI, 0xB8 /* IEN1 */, 4);
SBIT(IEADC, 0xB8 /* IEN1 */, 2);
SBIT(IEPWM, 0xB8 /* IEN1 */, 0);

SBIT(EXF2, 0xC0 /* IRCON */, 7);
SBIT(TF2, 0xC0 /* IRCON */, 6);
SBIT(IICIF, 0xC0 /* IRCON */, 5);
SBIT(LVIIF, 0xC0 /* IRCON */, 4);
SBIT(ADCIF, 0xC0 /* IRCON */, 2);
SBIT(PWMIF, 0xC0 /* IRCON */, 0);

SBIT(SM0, 0x98 /* SCON */, 7);
SBIT(SM1, 0x98 /* SCON */, 6);
SBIT(SM2, 0x98 /* SCON */, 5);
SBIT(REN, 0x98 /* SCON */, 4);
SBIT(TB8, 0x98 /* SCON */, 3);
SBIT(RB8, 0x98 /* SCON */, 2);
SBIT(TI, 0x98 /* SCON */, 1);
SBIT(RI, 0x98 /* SCON */, 0);

/* PSW */
SBIT(CY, 0xD0 /* PSW */, 7);
SBIT(AC, 0xD0 /* PSW */, 6);
SBIT(F0, 0xD0 /* PSW */, 5);
SBIT(RS1, 0xD0 /* PSW */, 4);
SBIT(RS0, 0xD0 /* PSW */, 3);
SBIT(OV, 0xD0 /* PSW */, 2);
SBIT(F1, 0xD0 /* PSW */, 1);
SBIT(P, 0xD0 /* PSW */, 0);

/*  TCON  */
SBIT(TF1, 0x88 /* TCON */, 7);
SBIT(TR1, 0x88 /* TCON */, 6);
SBIT(TF0, 0x88 /* TCON */, 5);
SBIT(TR0, 0x88 /* TCON */, 4);
SBIT(IE1, 0x88 /* TCON */, 3);
SBIT(IT1, 0x88 /* TCON */, 2);
SBIT(IE0, 0x88 /* TCON */, 1);
SBIT(IT0, 0x88 /* TCON */, 0);

//-------------------------------------------
SBIT(MPIF, 0xF8 /* IICS */, 6);
SBIT(LAIF, 0xF8 /* IICS */, 5);
SBIT(RXIF, 0xF8 /* IICS */, 4);
SBIT(TXIF, 0xF8 /* IICS */, 3);
SBIT(RXAK, 0xF8 /* IICS */, 2);
SBIT(TXAK, 0xF8 /* IICS */, 1);
SBIT(RW, 0xF8 /* IICS */, 0);
SBIT(BB, 0xF8 /* IICS */, 0);

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
