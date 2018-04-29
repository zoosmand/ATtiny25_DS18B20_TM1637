/*
 * interrupts.inc
 *
 *  Created: 31.03.2018 18:05:50
 *   Author: zoosman
 */ 
 
/******************************************* INT0 ******************************************/
_INT0:
  push R1
  push R0
  in R0, SREG
  push R0

  SKIP_FLAG_CLEAR _EREG_, _MTLF_
  rjmp _INT0_EXIT

 _OUT_OF_SLEEP:
    in tmp, MCUCR
    cbr tmp, (1<<SE)
    out MCUCR, tmp

  clr R1
  sts _EREG_, R1
  sts _OWREG_, R1

  SET_FLAG _EREG_, (1<<_MTLF_)
  cbi LEDPORT, LED0PIN

  in tmp, TCCR1
  sbr tmp, (1<<CS10)|(1<<CS11)|(1<<CS12)|(1<<CS13) // 16384
  out TCCR1, tmp
  in tmp, TIMSK
  sbr tmp, (1<<TOIE1)
  out TIMSK, tmp
  clr tmp // 256
  out TCNT1, tmp

  _MAIN_WAIT_BUTTON_UP:
    sbis INTPIN, INT0PIN
    rjmp _MAIN_WAIT_BUTTON_UP

  _INT0_EXIT:
    pop R0
    out SREG, R0
    pop R0
    pop R1
    reti
/********************************************************************************************/

/******************************************* WATHDOG ****************************************/
_WDT:
  push R1
  push R0
  in R0, SREG
  push R0

  wdr
  in tmp, WDTCR
  sbr tmp, (1<<WDIE)
  out WDTCR, tmp

  pop R0
  out SREG, R0
  pop R0
  pop R1
  reti
/********************************************************************************************/

/******************************************* TIMER0 *****************************************/
_TIMER0_COMPB:
_TIMER0_OVF:
 /* push R1
  push R0
  in R0, SREG
  push R0

  pop R0
  out SREG, R0
  pop R0
  pop R1*/
  reti

/********************************************************************************************/

/******************************************* TIMER1 *****************************************/
_TIMER1_OVF:
  push R1
  push R0
  in R0, SREG
  push R0

  CLEAR_FLAG _EREG_, (1<<_MTLF_)

  sbi LEDPORT, LED0PIN

  in tmp, TCCR1
  cbr tmp, (1<<CS10)|(1<<CS11)|(1<<CS12)|(1<<CS13) // 16384
  out TCCR1, tmp
  in tmp, TIMSK
  cbr tmp, (1<<TOIE1)
  out TIMSK, tmp
  clr tmp // 256
  out TCNT1, tmp

  pop R0
  out SREG, R0
  pop R0
  pop R1
  reti
/********************************************************************************************/

