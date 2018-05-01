;/*
; * display.inc
; *
; *  Created: 04.04.2018 17:41:29
; *   Author: setup
; */

.include "./inc/tm1637.inc"


;/* //////////////////////////////////////// Display ///////////////////////////////////////////// */
DISPLAY_RUN:
  rcall DELAY_2ms
  sbi DSPLDDR, DSPLDIO
  sbi DSPLDDR, DSPLCLK
  rcall DELAY_2ms
  rcall TM1637_INIT

  rcall SET_DISPLAY_SYMBOLS

  rcall DELAY_2ms
  rcall TM1637_RUN
  ret


;/* ///////////////////////////////// Prepare Data For Out ////////////////////////////////////// */
SET_DISPLAY_SYMBOLS:
  push ZH
  push ZL
  push R0

  SKIP_FLAG_SET _OWREG_, _SOF_
  rjmp _DISPLAY_ERROR

  ldi ZH, high(scratchpad)
  ldi ZL, low(scratchpad)
  ldd tmpL, Z+0 
  ldd tmpH, Z+1

  _CHECK_NEGATIVE:
    sbrs tmpH, 7
    rjmp _GET_DECIMAL_DATA
    inc tmpH
    neg tmpH
    inc tmpL
    neg tmpL
    SET_FLAG _EREG_, (1<<_NSF_)

  ;*************** Get Decimal ***************
  _GET_DECIMAL_DATA:
    mov tmp, tmpL
    andi tmp, (0x0f<<0)
    mov R0, tmp

    rcall MUL_8BIT_BY_100

    ldi tmp, 0x04
    mov R2, tmp
    _DIV_DECIMAL_BY_16:
      lsr R1
      ror R0
      dec R2
      brne _DIV_DECIMAL_BY_16

    push tmpL
    mov tmpL, R0
    ldi YL, low(decnum)
    ldi YH, high(decnum)

    rcall BIN8_DEC3
    pop tmpL
    ld tmp, -Y
    ld tmpD, -Y

  ;*************** Get Integral ***************
  ldi tmp, 0x04
  mov R0, tmp
  _DIV_INTEGRAL_BY_16:
    lsr tmpH
    ror tmpL
    dec R0
    brne _DIV_INTEGRAL_BY_16

  ;********** Order Symbols Onto RAM **********
  ldi YL, low(displaySymbols)
  ldi YH, high(displaySymbols)
  st Y+, tmpD
  rcall BIN8_DEC3

  ldi tmpA, 0x0a
  SKIP_FLAG_CLEAR _EREG_, _NSF_
  ldi tmpA, 0x0b
  st -Y, tmpA

  ld tmp, -Y
  tst tmp
  brne _BIN8_DEC3_EXIT

  ldi tmp, 0x0a
  std Y+1, tmp

  SKIP_FLAG_CLEAR _EREG_, _NSF_
  ldi tmp, 0x0b
  st Y, tmp

  _BIN8_DEC3_EXIT:
    CLEAR_FLAG _EREG_, (1<<_NSF_)
    rjmp _SET_DISPLAY_SYMBOLS_EXIT

  _DISPLAY_ERROR:
    ldi YH, high(displaySymbols)
    ldi YL, low(displaySymbols)
    ldi tmp, 0x01
    std Y+0, tmp
    ldi tmp, 0x0e
    std Y+1, tmp
    std Y+2, tmp
    ldi tmp, 0x0d
    std Y+3, tmp

  _SET_DISPLAY_SYMBOLS_EXIT:
    pop R0
    pop ZL
    pop ZH
  ret


;/* /////////////////////////////////// Binary to Decimal /////////////////////////////////////// */
BIN8_DEC3:
  push tmp
  push R0
  push R1

  ldi tmp, 0x02
  mov R0, tmp
  _BD1:
    clr tmpZ
    ldi tmp, 0x08
    mov R1, tmp
  _BD2:
    lsl tmpL
    rol tmpZ
    andi tmpL, 0xfe
    cpi tmpZ, 0x0a
    brcs _BD3
    subi tmpZ, 0x0a
    ori tmpL, 0x01
  _BD3:
    dec R1      
    brne _BD2
    st Y+, tmpZ
    dec R0
    brne _BD1
    st Y+, tmpL

  pop R1
  pop R0
  pop tmp
  ret

;/* ///////////////////////////////// Multiplify 8-bit by 100 //////////////////////////////////// */
MUL_8BIT_BY_100:
  clr r1
  add r0,r0
  adc r1,r1
  add r0,r0
  adc r1,r1
  movw r5:r4,r1:r0
  add r0,r0
  adc r1,r1
  add r0,r0
  adc r1,r1
  add r0,r0
  adc r1,r1
  add r4,r0
  adc r5,r1
  add r0,r0
  adc r1,r1
  add r0,r4
  adc r1,r5
  ret
