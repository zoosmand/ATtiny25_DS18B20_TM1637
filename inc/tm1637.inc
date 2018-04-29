 /*
 * tm1637.inc
 *
 *  Created: 05.04.2018 8:56:06
 *   Author: zoosman
 */ 

TM1637_START:
;  sbi DSPLPORT, DSPLCLK
;  sbi DSPLPORT, DSPLDIO
  nop
  nop
  cbi DSPLPORT, DSPLDIO
  ret


TM1637_STOP:
  cbi DSPLPORT, DSPLCLK
  nop
  nop
  cbi DSPLPORT, DSPLDIO
  nop
  nop
  sbi DSPLPORT, DSPLCLK
  nop
  nop
  sbi DSPLPORT, DSPLDIO
  ret


TM1637_WRITE_BYTE:
  push R0
  ldi tmp, 0x08
  mov R0, tmp

  _WRITE_BYTE_BIT:
    cbi DSPLPORT, DSPLCLK
    asr dsplByte
    brcs _WRITE_BYTE_BIT_1

    _WRITE_BYTE_BIT_0:
      cbi DSPLPORT, DSPLDIO
      rjmp _WRITE_BYTE_NEXT_BIT

    _WRITE_BYTE_BIT_1:
      sbi DSPLPORT, DSPLDIO

    _WRITE_BYTE_NEXT_BIT:
      nop
      nop
      nop
      sbi DSPLPORT, DSPLCLK
      nop
      nop
      nop
      dec R0
      brne _WRITE_BYTE_BIT

  _WRITE_BYTE_WAIT_ACK:
    cbi DSPLPORT, DSPLCLK
    nop
    nop
    nop
    nop
    nop
    cbi DSPLDDR, DSPLDIO
    sbic DSPLPIN, DSPLDIO
    rjmp _WRITE_BYTE_WAIT_ACK_END

    sbi DSPLPORT, DSPLCLK
    nop
    nop
    cbi DSPLPORT, DSPLCLK

  _WRITE_BYTE_WAIT_ACK_END:
    sbi DSPLDDR, DSPLDIO

  pop R0
  ret




TM1637_INIT:
  rcall TM1637_START
  ldi tmp, 0x40
  mov dsplByte, tmp
  rcall TM1637_WRITE_BYTE
  rcall TM1637_STOP

  rcall TM1637_START

  ldi tmp, 0xc0
  mov dsplByte, tmp
  rcall TM1637_WRITE_BYTE

  clr dsplByte
  ldi tmp, 0x04
  mov R0, tmp
  _TM1637_INIT_WRITE_BYTE:
    rcall TM1637_WRITE_BYTE
    dec R0
    brne _TM1637_INIT_WRITE_BYTE

  rcall TM1637_STOP

  rcall TM1637_START
  ldi tmp, 0x8a
  mov dsplByte, tmp
  rcall TM1637_WRITE_BYTE
  rcall TM1637_STOP

  ret





TM1637_RUN:
  push ZH
  push ZL
  push YH
  push YL
  push R0

  ldi YL, low(displaySymbols + 4)
  ldi YH, high(displaySymbols + 4)

  rcall TM1637_START
  ldi tmp, 0x41
  mov dsplByte, tmp
  rcall TM1637_WRITE_BYTE
  rcall TM1637_STOP

  rcall TM1637_START

  ldi tmp, 0xc0
  mov dsplByte, tmp
  rcall TM1637_WRITE_BYTE

  ldi tmp, 0x04
  mov R0, tmp

  _TM1637_RUN_SET_ELEMENT:
    ldi ZH, high(2*symbols)
    ldi ZL, low(2*symbols)

    ld tmp, -Y
    add ZL, tmp
    clr tmp
    adc ZH, tmp
    lpm dsplByte, Z

    ldi tmp, 0x02
    cp tmp, R0
    brne PC+3
    ldi tmp, 0x80
    or dsplByte, tmp

    rcall TM1637_WRITE_BYTE

    dec R0
    brne _TM1637_RUN_SET_ELEMENT

  rcall TM1637_STOP

  pop R0
  pop YL
  pop YH
  pop ZL
  pop ZH
  ret

