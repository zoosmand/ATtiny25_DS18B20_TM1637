;/*
; * utils.inc
; *
; *  Created: 30.03.2018 22:30:49
; *   Author: zoosman
; */ 


 
;/***************************** LED0 BLINK ***************************/
LED_BLINK:
  sbis LEDPIN, LED0PIN
  rjmp _LED_BLINK_UP
  cbi LEDPORT, LED0PIN
  ret
  _LED_BLINK_UP:
    sbi LEDPORT, LED0PIN
    ret

;/**************************** Clear Timer ***************************/
CLEAR_TIMER:
  cli
  in tmp, TCCR0B
  cbr tmp, (1<<CS00)|(1<<CS01)|(1<<CS02)
  out TCCR0B, tmp
  in tmp, TIMSK
  cbr tmp, (1<<OCIE0B)|(1<<TOIE0)
  out TIMSK, tmp
  reti

;/**************************** DELAY 2ms *****************************/
DELAY_2ms:
  cli
  in tmp, TCCR0B
  sbr tmp, (1<<CS01) ; 8
  out TCCR0B, tmp
  in tmp, TIMSK
  sbr tmp, (1<<OCIE0B)
  out TIMSK, tmp
  sei

  clr tmp
  out TCNT0, tmp

  ldi tmp, 0x02
  out OCR0B, tmp
  _OW_DELAY_2ms:
    in tmp, TIFR
    sbrs tmp, OCF0B
    rjmp _OW_DELAY_2ms

  rcall CLEAR_TIMER
  ret

;/*****************************  750ms Delay ************************/
DELAY_750ms:
  push R0
  cli
  in tmp, TCCR0B
  sbr tmp, (1<<CS02)|(1<<CS02) ; 1024
  out TCCR0B, tmp
  in tmp, TIMSK
  sbr tmp, (1<<OCIE0B)
  out TIMSK, tmp
  sei

  ldi tmp, 0x03
  mov R0, tmp

  ldi tmp, 0xfa
  out OCR0B, tmp
  _OW_DELAY_750ms:

    clr tmp
    out TCNT0, tmp
    _OW_DELAY_250ms:
      in tmp, TIFR
      sbrs tmp, OCF0B
      rjmp _OW_DELAY_250ms

    dec R0
    brne _OW_DELAY_750ms
  
  rcall CLEAR_TIMER
  pop R0
  ret
