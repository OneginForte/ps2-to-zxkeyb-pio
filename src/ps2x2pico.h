//#pragma once
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


extern const enum zx_key{
    NC=0xFF,
    //      ряд X0
    SP=0x00, //SPACE
    SS=0x10, //SYMBOL SHIFT
    _M=0x20,
    _N=0x40,
    _B=0x30,
    //      ряд X1
    EN=0x01, // ENTER
    _L=0x11,
    _K=0x21,
    _J=0x41,
    _H=0x31,
    //      ряд X2
    _P=0x02,
    _O=0x12,
    _I=0x22,
    _U=0x42,
    _Y=0x32,
    //      ряд X3
    _0=0x03,
    _9=0x13,
    _8=0x23,
    _7=0x43,
    _6=0x33,
    //      ряд X4    
    CS=0x04, //CAPS SHIFT
    _Z=0x14,
    _X=0x24,
    _C=0x44,
    _V=0x34,
    //      ряд X5
    _A=0x05,
    _S=0x15,
    _D=0x25,
    _F=0x45,
    _G=0x35,   
    //      ряд X6
    _Q=0x06,
    _W=0x16,
    _E=0x26,
    _R=0x46,
    _T=0x36,
    //      ряд X7
    _1=0x07,
    _2=0x17,
    _3=0x27,
    _4=0x47,
    _5=0x37
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



extern u8 const ext_code_keys_1_2[]; // keys in this list need to have KB_EXT_PFX_E0 sent before their actual code
extern u8 const ext_code_modifier_keys_1_2[]; // keys in this list need to have KB_EXT_PFX_E0 sent before their actual code
extern u8 const mod2ps2_1[];
extern u8 const mod2ps2_2[];
extern u8 const mod2ps2_3[];
extern u8 const hid2ps2_1[];
extern u8 const hid2ps2_2[];
extern u8 const hid2ps2_3[];
extern u8 const prt_scn_make_1[];
extern u8 const prt_scn_break_1[];
extern u8 const break_make_1[];
extern u8 const pause_make_1[];
extern u8 const prt_scn_make_2[];
extern u8 const prt_scn_break_2[];
extern u8 const break_make_2[];
extern u8 const pause_make_2[];

#endif