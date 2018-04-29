/*
 * ow.inc
 *
 *  Created: 31.03.2018 6:51:35
 *   Author: zoosman
 */ 
/************************************* OneWire Subrutines ****************************************/

/*************************************** OneWire Reset *******************************************/
OW_RESET:
  SET_FLAG _OWREG_, (1<<_SRF_)
  CLEAR_FLAG _OWREG_, (1<<_SOF_)

  cli
  in tmp, TCCR0B
  sbr tmp, (1<<CS01) // 8
  out TCCR0B, tmp
  in tmp, TIMSK
  sbr tmp, (1<<OCIE0B)
  out TIMSK, tmp
  clr tmp
  out TCNT0, tmp
  sei

  _OWMA0_L

  ldi tmp, 0x3d ; 480us
  out OCR0B, tmp
  _OW_RESET_DELAY_480:
    in tmp, TIFR
    sbrs tmp, OCF0B
    rjmp _OW_RESET_DELAY_480

  _OWMA0_H

  ldi tmp, 0x45 ; 550us
  out OCR0B, tmp
  _OW_RESET_DELAY_550:
    in tmp, TIFR
    sbrs tmp, OCF0B
    rjmp _OW_RESET_DELAY_550
  
  sbis OWMAPIN, OWMA0PIN
  SET_FLAG _OWREG_, (1<<_SOF_)

  ldi tmp, 0x78 ; 960us
  out OCR0B, tmp
  _OW_RESET_DELAY_960:
    in tmp, TIFR
    sbrs tmp, OCF0B
    rjmp _OW_RESET_DELAY_960

  rcall CLEAR_TIMER
  CLEAR_FLAG _OWREG_, (1<<_SRF_)
  ret


/*************************************** OneWire Send Byte *******************************************/
OW_SEND_BYTE:
  ; Set Send Byte Flag
  SET_FLAG _OWREG_, (1<<_SBF_)

  ldi tmp, 0x08
  mov R0, tmp
  _OW_SEND_BIT:
    _OWMA0_L

	lsr txByte
	brcc _OW_SEND_ZERO
	
	_OW_SEND_ONE:
	  clr tmp ; 6us
	  nop
	  nop
	  nop
	  _OWMA0_H
	  rjmp _OW_SEND_ZERO_DELAY_70

  	_OW_SEND_ZERO:
	  clr tmp
	  _OW_SEND_ZERO_DELAY_60:
	    inc tmp
		cpi tmp, 0x0b ; 60us
	    brne _OW_SEND_ZERO_DELAY_60
	  _OWMA0_H

    _OW_SEND_ZERO_DELAY_70:
	  inc tmp
	  cpi tmp, 0x0d // 10us = 70 - 60, 64us = 70 - 6
	  brne _OW_SEND_ZERO_DELAY_70
    dec R0
	brne _OW_SEND_BIT

  CLEAR_FLAG _OWREG_, (1<<_SBF_)
  ret


/*************************************** OneWire Read Byte *******************************************/
OW_READ_BYTE:
  ; Set Read Byte Flag
  SET_FLAG _OWREG_, (1<<_RBF_)

  ldi tmp, 0x08
  mov R0, tmp
  _OW_READ_BIT:
    _OWMA0_L
    clr tmp

    _OW_READ_BIT_DELAY_6:
	  inc tmp ; 6us
	  nop
	  nop
	  nop
	_OWMA0_H

    _OW_READ_BIT_DELAY_15:
	  inc tmp
	  cpi tmp, 0x2 ; 15us
	  brne _OW_READ_BIT_DELAY_15

	lsr rxByte
	sbic OWMAPIN, OWMA0PIN
	ori rxByte, 0x80

  _OW_READ_BIT_DELAY_70:
	  inc tmp
	  cpi tmp, 0x0d ; 70us
	  brne _OW_READ_BIT_DELAY_70

  _OW_READ_BIT_COUNTDOWN:
    dec R0
	brne _OW_READ_BIT

  CLEAR_FLAG _OWREG_, (1<<_RBF_)
  ret


/************************* Read Scratchpad *************************/
READ_SCRATCHPAD:
  ldi ZH, high(scratchpad)
  ldi ZL, low(scratchpad)
  clr CRC
  ldi tmp, 0x09
  mov R1, tmp

  _OW_READ_SCRATCHPAD:
    clr rxByte
    rcall OW_READ_BYTE
	st Z+, rxByte
    rcall CHECK_CRC

  _OW_READ_BYTE_COUNTDOWN:
    dec R1
	brne _OW_READ_SCRATCHPAD
  ret


/******************* Read ROM Address from EEPROM ******************/
/*READ_ADDRESS:
  ldi ZH, high(address)
  ldi ZL, low(address)

  ldi tmp, 0x08
  mov R1, tmp
  clr R2
  _EEPROM_READ:
    sbic EECR,EEPE
    rjmp _EEPROM_READ
	out EEARL, R2
	sbi EECR, EERE
	in tmp, EEDR
	st Z+, tmp
    inc R2
  dec R1
  brne _EEPROM_READ
  ret*/


/********************** Send 64-bit ROM Address *********************/
/*SEND_ROM:
  ldi ZH, high(address)
  ldi ZL, low(address)

  ldi tmp, 0x08
  mov R1, tmp
  rcall OW_CLEAR_TIMER

  _OW_SEND_ROM:
	ld txByte, Z+
	rcall OW_SEND_BYTE

  _OW_SEND_ROM_COUNTDOWN:
    dec R1
	brne _OW_SEND_ROM
  ret
*/

/**************************** Check CRC ***************************/
CHECK_CRC:
  push R0
  ldi tmp, 0x08
  mov R0, tmp

  _CRC8_LOOP:
    mov	tmpCRC, CRC
    eor	tmpCRC, rxByte
    ror	tmpCRC
    brcc _CRC_ZERO

    ldi	tmp, 0x18
    eor CRC, tmp

  _CRC_ZERO:
    ror CRC
    ror rxByte 
    dec R0
    brne _CRC8_LOOP
	pop R0
  ret


/************************* Proceed Measure ************************/
MEASUREMENT:
  ; Check Devices On-Line
  rcall OW_RESET
  SKIP_FLAG_SET _OWREG_, _SOF_
  ret

  ; Send Convert Temperature command 0x44
  ldi txByte, 0xcc
  rcall OW_SEND_BYTE
  ldi txByte, 0x44
  rcall OW_SEND_BYTE
  ; Wait for convertion with strong pull-up
  _OWSTENA
  rcall DELAY_750ms
  _OWSTDIS

  rcall OW_RESET
  SKIP_FLAG_SET _OWREG_, _SOF_
  ret

  ; Send Read Scratchpad Command 0xbe
  ldi txByte, 0xcc
  rcall OW_SEND_BYTE
  ldi txByte, 0xbe
  rcall OW_SEND_BYTE
  ; Wait before read
  rcall DELAY_2ms
  rcall READ_SCRATCHPAD

  ; Check CRC 9-th byte of scratchpad
  tst CRC
  brne _SET_CRCEF_FLAG

  ; Exit
  ret
  ; Exit if CRC Error
  _SET_CRCEF_FLAG:
    SET_FLAG _OWREG_, (1<<_CRCEF_)
  ret