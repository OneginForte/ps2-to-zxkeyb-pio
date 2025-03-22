#pragma once
/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 No0ne (https://github.com/No0ne)
 *           (c) 2023 Dustin Hoffman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#ifndef _PS2X2PICO_H_
#define _PS2X2PICO_H_


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "tusb.h"
#include "hardware/pio.h"
#include "pico/util/queue.h"

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define KBD_BUFFER_SIZE 16

enum MT8816_Y
{
	D0 = 0 << 4,
	D1 = 1 << 4,
	D2 = 2 << 4,
	D3 = 3 << 4,
	D4 = 4 << 4
};

enum CMT8816_X
{
	KA8  = 7,
	KA9  = 6,
	KA10 = 5,
	KA11 = 4,
	KA12 = 3,
	KA13 = 2,
	KA14 = 1,
	KA15 = 0
};

 //----------------------------------------------------------
 // Keytable   AX0 AX1 AX2 AX3 AY0 AY1 AY2
 //         
 //   
 // pent   Y0  Y1  Y2  Y4  Y3  Y3  Y4  Y2  Y1  Y0
 //        D0  D1  D2  D3  D4  D4  D3  D2  D1  D0
 // X7 KA11 1   2   3   4   5  6   7   8   9   0   KA12 X3
 // X6 KA10 Q   W   E   R   T  Y   U   I   O   P   KA13 X2
 // X5 KA9  A   S   D   F   G  H   J   K   L   EN  KA14 X1
 // X4 KA8 CS   Z   X   C   V  B   N   M   SS  SP  KA15 X0

extern const enum zx_key{
    NC=0xFF,
    //      ряд X0
    SP= D0 | KA15, //SPACE
    SS= D1 | KA15, //SYMBOL SHIFT
    _M= D2 | KA15,
    _N= D4 | KA15,
    _B= D3 | KA15,
    //      ряд X1
    EN= D0 | KA14, // ENTER
    _L= D1 | KA14,
    _K= D2 | KA14,
    _J= D4 | KA14,
    _H= D3 | KA14,
    //      ряд X2
    _P= D0 | KA13,
    _O= D1 | KA13,
    _I= D2 | KA13,
    _U= D4 | KA13,
    _Y= D3 | KA13,
    //      ряд X3
    _0= D0 | KA12,
    _9= D1 | KA12,
    _8= D2 | KA12,
    _7= D4 | KA12,
    _6= D3 | KA12,
    //      ряд X4    
    CS= D0 | KA8, //CAPS SHIFT
    _Z= D1 | KA8,
    _X= D2 | KA8,
    _C= D4 | KA8,
    _V= D3 | KA8,
    //      ряд X5
    _A= D0 | KA9,
    _S= D1 | KA9,
    _D= D2 | KA9,
    _F= D4 | KA9,
    _G= D3 | KA9,   
    //      ряд X6
    _Q= D0 | KA10,
    _W= D1 | KA10,
    _E= D2 | KA10,
    _R= D4 | KA10,
    _T= D3 | KA10,
    //      ряд X7
    _1= D0 | KA11,
    _2= D1 | KA11,
    _3= D2 | KA11,
    _4= D4 | KA11,
    _5= D3 | KA11
} zx_key_pent;


void scancode_s(uint8_t code);
void key_on(uint8_t code); // клавиша нажата
void key_off(uint8_t code);// клавиша отпущена  
void SetAddr(uint8_t addr) ;
void kb_set_leds(uint8_t d);
void kb_res_leds(uint8_t d);


void kb_init(u8 gpio_in);
void kb_send_key(u8 key, bool is_key_pressed, u8 modifiers);
void tuh_kb_set_leds(u8 leds);
bool kb_task();
void kb_set_leds(u8 byte);
void kb_res_leds(u8 byte);

void ms_init(u8 gpio_in);
void ms_send_movement(u8 buttons, s8 x, s8 y, s8 z);
bool ms_task();


u32 ps2_frame(u8 byte);
typedef void (*rx_callback)(u8 byte, u8 prev_byte);

typedef struct {
  PIO pio;
  uint sm;
  queue_t qbytes;
  queue_t qpacks;
  rx_callback rx;
  u8 last_rx;
  u8 last_tx;
  u8 sent;
  u8 busy;
} ps2out;

void keyboard( uint8_t const *report, uint16_t len); //uint8_t modifiers,
void keyboard_task(ps2out* this);
void ps2out_init(ps2out* this, rx_callback rx);
void ps2out_task(ps2out* this);


typedef struct {
  PIO pio;
  uint sm;
  u8 state;
  u8 last_tx;
  u8 byte;
} ps2in;

void ps2in_init(ps2in* this, PIO pio, u8 data_pin);
void ps2in_task(ps2in* this, ps2out* out);
void ps2in_reset(ps2in* this);
void ps2in_set(ps2in* this, u8 command, u8 byte);

#define PS2_LED_SCROLL_LOCK 1
#define PS2_LED_NUM_LOCK    2
#define PS2_LED_CAPS_LOCK   4

#define KB_EXT_PFX_E0 0xe0 // This is the extended code prefix used in sets 1 and 2
#define KB_BREAK_2_3 0xf0 // The prefix 0xf0 is the break code prefex in sets 2 and 3 (is send when key is released)
#define HID2PS2_IDX_MAX 0x73
#define IS_VALID_KEY(key) (key <= HID2PS2_IDX_MAX || (key >= HID_KEY_CONTROL_LEFT && key <= HID_KEY_GUI_RIGHT))
#define IS_MOD_KEY(key) (key >= HID_KEY_CONTROL_LEFT && key <= HID_KEY_GUI_RIGHT)

// пины пико для управления 8816

#define AX0 4
#define AX1 5
#define AX2 6
#define AX3 7
#define AY0 8
#define AY1 9
#define AY2 10
#define STBMT 11
#define CSMT  12
#define RSTMT 13
#define DATMT 14


//#define LEDPIN 25 //  Black pico
#define LEDPIN 25 //  zero pico

#define KEYBOARD_REPORT_SIZE    20



enum
{
  ITF_NUM_KEYBOARD,
  ITF_NUM_HID,
  ITF_NUM_CDC,
  ITF_NUM_CDC_DATA, // CDC needs 2 interfaces
  ITF_NUM_TOTAL
};


// 20-byte NKRO: support keycodes upto LANG8(0x97)
#define KEYBOARD_REPORT_KEYS    (KEYBOARD_REPORT_SIZE - 2)
#define KEYBOARD_REPORT_BITS    (KEYBOARD_REPORT_SIZE - 1)

// макросы
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#define bit(b) (1UL << (b))
// макросы end


extern uint8_t const macro1[]; 
extern uint8_t const macro2[]; 
extern uint8_t const macro3[]; 
extern uint8_t const macro4[]; 
extern uint8_t const macro5[];
extern uint8_t const macro6[]; 
extern uint8_t const macro7[];
extern uint8_t const macro8[];
extern uint8_t const macro9[];
extern uint8_t const macro10[];
extern uint8_t const macro11[];
extern uint8_t const macro12[];


extern uint8_t const ext_code_keys_1_2[]; // keys in this list need to have KB_EXT_PFX_E0 sent before their actual code
extern uint8_t const ext_code_modifier_keys_1_2[]; // keys in this list need to have KB_EXT_PFX_E0 sent before their actual code
extern uint8_t const mod2ps2_1[];
extern uint8_t const mod2ps2_2[];
extern uint8_t const mod2ps2_3[];
extern uint8_t const hid2ps2_1[];
extern uint8_t const hid2ps2_2[];
extern uint8_t const hid2ps2_3[];
extern uint8_t const prt_scn_make_1[];
extern uint8_t const prt_scn_break_1[];
extern uint8_t const break_make_1[];
extern uint8_t const pause_make_1[];
extern uint8_t const prt_scn_make_2[];
extern uint8_t const prt_scn_break_2[];
extern uint8_t const break_make_2[];
extern uint8_t const pause_make_2[];
extern uint8_t const table_key_zx_hid[];
extern uint8_t const table_key_zx_ps[][4];

#endif