;/*
; * init.inc
; *
; *  Created: 30.03.2018 22:29:21
; *   Author: zoosman
; */ 

;/**************************** START AFTER POWER UP OR RESET FROM ******************************/
_RESET:
  clr R1
  out SREG, R1
  ldi tmp, low(RAMEND)
  out SPL, tmp

  sts _EREG_, R1
  sts _OWREG_, R1
  SET_FLAG _EREG_, (1<<_MIOF_)|(1<<_DIRF_)

cli
;/******************************************* Timers *********************************************/
;INIT_TIMER0:
;  in tmp, TCCR0B
;  sbr tmp, (1<<CS02)|(1<<CS00) ; 1024
;  out TCCR0B, tmp
;  in tmp, TIMSK
;  sbr tmp, (1<<TOIE0)
;  out TIMSK, tmp
;  clr tmp ; 256
;  out TCNT0, tmp


;INIT_TIMER1:
;  in tmp, TCCR1
;  sbr tmp, (1<<CS10)|(1<<CS11)|(1<<CS12)|(1<<CS13) ; 16384
;  out TCCR1, tmp
;  in tmp, TIMSK
;  sbr tmp, (1<<TOIE1)
;  out TIMSK, tmp
;  clr tmp ; 256
;  out TCNT1, tmp

;/***************************************** Reduce Power *****************************************/
INIT_POWERMODE:
  in tmp, ACSR
  sbr tmp, (1<<ACD)
  out ACSR, tmp

  in tmp, PRR
  sbr tmp, (1<<PRUSI)|(1<<PRADC)
  out PRR, tmp

;/************************************** External Interrupt **************************************/
INIT_EXTINT:
  in tmp, GIMSK
  sbr tmp, (1<<INT0)
  out GIMSK, tmp

;/************************************ Sleep Mode (Power down) ***********************************/
INIT_SLEEPMODE:
  in tmp, MCUCR
  sbr tmp, (1<<SM1)
  out MCUCR, tmp

;/**************************************** WatchDog Timer ****************************************/
INIT_WATCHDOG:
  in tmp, WDTCR
  sbr tmp, (1<<WDCE)|(1<<WDE)
  out WDTCR, tmp
  sbr tmp, (1<<WDP0)|(1<<WDP3)|(1<<WDIE) // 8s
  out WDTCR, tmp

sei

;/***************************************** INT Button *******************************************/
INIT_INT:
  in tmp, INTPORT
  sbr tmp, (1<<INT0PIN)
  out INTPORT, tmp

;/******************************************* LED ************************************************/
INIT_LED:
  sbi LEDDDR, LED0PIN
  sbi LEDPORT, LED0PIN

;/*************************************** OneWire Master *****************************************/
INIT_OWMA:
  cbi OWMAPORT, OWMA0PIN
  cbi OWMADDR, OWMA0PIN

.MACRO _OWSTENA
  sbi OWMAPORT, OWMA0PIN
  sbi OWMADDR, OWMA0PIN
.ENDMACRO

.MACRO _OWSTDIS
  cbi OWMADDR, OWMA0PIN
  cbi OWMAPORT, OWMA0PIN
.ENDMACRO

.MACRO _OWMA0_L
  sbi OWMADDR, OWMA0PIN
.ENDMACRO

.MACRO _OWMA0_H
  cbi OWMADDR, OWMA0PIN
.ENDMACRO
