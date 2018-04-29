;
; ATtiny85 AS (termometer).asm
;
; Created: 30.03.2018 20:42:19
; Author : zoosman
;

.include "tn25def.inc"
.equ F_CPU = 1000000

.def CRC		= R10
.def tmpCRC		= R11
.def dsplByte	= R12
.def tmp		= R16
.def tmpA		= R17
.def txByte		= R18
.def rxByte		= R19
.def tmpL		= R20
.def tmpH		= R21
.def tmpZ		= R22
.def tmpD		= R23


.equ INTDDR		= DDRB
.equ INTPORT	= PORTB
.equ INTPIN		= PINB
.equ INT0PIN	= PB2

.equ LEDDDR		= DDRB
.equ LEDPORT	= PORTB
.equ LEDPIN		= PINB
.equ LED0PIN	= PB1

.equ DSPLDDR	= DDRB
.equ DSPLPORT	= PORTB
.equ DSPLPIN	= PINB
.equ DSPLDIO	= PB3
.equ DSPLCLK	= PB4

.equ OWMADDR	= DDRB
.equ OWMAPORT	= PORTB
.equ OWMAPIN	= PINB
.equ OWMA0PIN	= PB0

/********************************* Event REGistry *******************************/
.equ _MTLF_		= 0 ; Measure Time Line Flag
.equ _MIOF_		= 1 ; Measure Is Over Flag
.equ _DIRF_		= 2 ; Display Is Run Flag
.equ _NSF_		= 3 ; Negative Sign Flag

/******************************** OneWire REGistry ******************************/
.equ _MIF_		= 0 ; Measure Interval Flag
.equ _SRF_		= 1 ; Send Reset Flag
.equ _SOF_		= 2 ; Slave On-line Flag
.equ _SBF_		= 3 ; Send Byte Flag
.equ _RBF_		= 4 ; Read Byte Flag
.equ _750ms_	= 5
.equ _2ms_		= 6
.equ _CRCEF_	= 7 ; CRC Error Flag



.cseg
.org 0

.include "./inc/vectors.inc"
.include "./inc/macroses.inc"
.include "./inc/init.inc"



  
/************************************ MAIN LOOP *********************************/
MAIN:
  SKIP_FLAG_SET _EREG_, _MTLF_
  rjmp _GO_TO_SLEEP

  rcall MAKE_MEASURE
  rcall RUN_DISPLAY
  rjmp MAIN

  _GO_TO_SLEEP:
    in tmp, MCUCR
    sbr tmp, (1<<SE)
    out MCUCR, tmp
    sleep

  rjmp MAIN
  rjmp THE_END


.include "./inc/ow.inc"
.include "./inc/display.inc"
.include "./inc/utils.inc"
.include "./inc/interrupts.inc"


/******************************* Make Measurement ******************************/
MAKE_MEASURE:
  SKIP_FLAG_CLEAR _EREG_, _MIOF_
  ret

  rcall MEASUREMENT
  
  SET_FLAG _EREG_, (1<<_MIOF_)
  ret

/****************************** Show Data on Display ***************************/
RUN_DISPLAY:
  SKIP_FLAG_CLEAR _EREG_, _DIRF_
  ret

  sbi DSPLPORT, DSPLCLK
  sbi DSPLPORT, DSPLDIO

  rcall DISPLAY_RUN

  cbi DSPLPORT, DSPLDIO
  cbi DSPLPORT, DSPLCLK

  SET_FLAG _EREG_, (1<<_DIRF_)
  ret

/************************************ THE END **********************************/
THE_END:
  cli
  rjmp 0


.include "./inc/var.inc"
