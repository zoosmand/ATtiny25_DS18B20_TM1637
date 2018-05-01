;/*
; * macroses.inc
; *
; *  Created: 31.03.2018 17:52:33
; *   Author: zoosman
; */ 

.MACRO SET_FLAG
  lds tmp, @0
  sbr tmp, @1
  sts @0, tmp
.ENDMACRO

.MACRO CLEAR_FLAG
  lds tmp, @0
  cbr tmp, @1
  sts @0, tmp
.ENDMACRO

.MACRO SKIP_FLAG_SET
  lds tmp, @0
  sbrs tmp, @1
.ENDMACRO

.MACRO SKIP_FLAG_CLEAR
  lds tmp, @0
  sbrc tmp, @1
.ENDMACRO

