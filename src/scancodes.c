/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 No0ne (https://github.com/No0ne)
 *           (c) 2023 Dustin Hoffman
 *           (c) 2024 Bernd Strobel
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
#include "ps2x2pico.h"
#include "tusb.h"



u8 const ext_code_keys_1_2[] = {
  HID_KEY_INSERT,
  HID_KEY_HOME,
  HID_KEY_PAGE_UP,
  HID_KEY_DELETE,
  HID_KEY_END,
  HID_KEY_PAGE_DOWN,
  HID_KEY_ARROW_RIGHT,
  HID_KEY_ARROW_LEFT,
  HID_KEY_ARROW_DOWN,
  HID_KEY_ARROW_UP,
  HID_KEY_KEYPAD_DIVIDE,
  HID_KEY_KEYPAD_ENTER,
  HID_KEY_APPLICATION,
  HID_KEY_POWER,
  0 // End marker
};

u8 const ext_code_modifier_keys_1_2[] = {
  HID_KEY_GUI_LEFT,
  HID_KEY_CONTROL_RIGHT,
  HID_KEY_ALT_RIGHT,
  HID_KEY_GUI_RIGHT,
  0
};

u8 const prt_scn_make_1[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 };
u8 const prt_scn_break_1[] = { 0xe0, 0xb7, 0xe0, 0xaa, 0 };
u8 const break_make_1[]    = { 0xe0, 0x46, 0xe0, 0xc6, 0 };
u8 const pause_make_1[]    = { 0xe1, 0x1d, 0x45, 0xe1, 0x9d, 0xc5, 0 };

u8 const prt_scn_make_2[]  = { 0xe0, 0x12, 0xe0, 0x7c, 0 };
u8 const prt_scn_break_2[] = { 0xe0, 0xf0, 0x7c, 0xe0, 0xf0, 0x12, 0 };
u8 const break_make_2[]    = { 0xe0, 0x7e, 0xe0, 0xf0, 0x7e, 0 };
u8 const pause_make_2[]    = { 0xe1, 0x14, 0x77, 0xe1, 0xf0, 0x14, 0xf0, 0x77, 0 };

// break codes in set 1 are created from the make code by adding 128 (0x80) or in other words set the msb.
// this is true even for the extended codes (but 0xe0 stays 0xe0) and the special multi byte codes for print screen and pause
u8 const mod2ps2_1[] = {
  0x1d, // L CTRL
  0x2a, // L SHFT
  0x38, // L ALT
  0x5b, // L GUI
  0x1d, // R CTRL
  0x36, // R SHFT
  0x38, // R ALT
  0x5c  // R GUI
};

u8 const mod2ps2_2[] = {
  0x14, // L CTRL
  0x12, // L SHFT
  0x11, // L ALT
  0x1f, // L WIN
  0x14, // R CTRL
  0x59, // R SHFT
  0x11, // R ALT
  0x27  // R WIN
};

u8 const mod2ps2_3[] = {
  0x11, // L CTRL
  0x12, // L SHFT
  0x19, // L ALT
  0x8b, // L WIN
  0x58, // R CTRL
  0x59, // R SHFT
  0x39, // R ALT
  0x8c  // R WIN
};

// break codes in set 1 are created from the make code by adding 128 (0x80) or in other words set the msb.
// this is true even for the extended codes (but 0xe0 stays 0xe0) and the special multi byte codes for print screen and pause
u8 const hid2ps2_1[] = {
  // 0x00 - 0x0f
  0, 0, 0, 0, // NONE
  0x1e, // A
  0x30, // B
  0x2e, // C
  0x20, // D
  0x12, // E
  0x21, // F
  0x22, // G
  0x23, // H
  0x17, // I
  0x24, // J
  0x25, // K
  0x26, // L
  // 0x10 - 0x1f
  0x32, // M
  0x31, // N
  0x18, // O
  0x19, // P
  0x10, // Q
  0x13, // R
  0x1f, // S
  0x14, // T
  0x16, // U
  0x2f, // V
  0x11, // W
  0x2d, // X
  0x15, // Y
  0x2c, // Z
  0x02, // 1
  0x03, // 2
  // 0x20 - 0x2f
  0x04, // 3
  0x05, // 4
  0x06, // 5
  0x07, // 6
  0x08, // 7
  0x09, // 8
  0x0a, // 9
  0x0b, // 0
  0x1c, // ENTER
  0x01, // ESC
  0x0e, // BKSP
  0x0f, // TAB
  0x39, // SPACE
  0x0c, // -
  0x0d, // =
  0x1a, // [
  // 0x30 - 0x3f
  0x1b, // ]
  0x2b, // BACKSLASH
  0x2b, // EUROPE_1
  0x27, // ;
  0x28, // ' APOSTROPHE
  0x29, // ` GRAVE
  0x33, // ,
  0x34, // .
  0x35, // /
  0x3a, // CAPS
  0x3b, // F1
  0x3c, // F2
  0x3d, // F3
  0x3e, // F4
  0x3f, // F5
  0x40, // F6
  // 0x40 - 0x4f
  0x41, // F7
  0x42, // F8
  0x43, // F9
  0x44, // F10
  0x57, // F11
  0x58, // F12
  0x00, // E0,2A,E0,37 // PRNT SCRN // break E0,B7,E0,AA
  0x46, // SCROLL
  0x00, // E1,1D,45,E1,9D,C5 // PAUSE, no break codes!
  0x52, // E0 INSERT
  0x47, // E0 HOME
  0x49, // E0 PG UP
  0x53, // E0 DELETE
  0x4f, // E0 END
  0x51, // E0 PG DN
  0x4d, // E0 R ARROW
  // 0x50 - 0x5f
  0x4b, // E0 L ARROW
  0x50, // E0 D ARROW
  0x48, // E0 U ARROW
  0x45, // NUM
  0x35, // E0 KP /
  0x37, // KP *
  0x4a, // KP -
  0x4e, // KP +
  0x1c, // E0 KP EN
  0x4f, // KP 1
  0x50, // KP 2
  0x51, // KP 3
  0x4b, // KP 4
  0x4c, // KP 5
  0x4d, // KP 6
  0x47, // KP 7
  // 0x60 - 0x6f
  0x48, // KP 8
  0x49, // KP 9
  0x52, // KP 0
  0x53, // KP .
  0x56, // EUROPE_2
  0x5d, // E0 APPS
  0x5e, // E0 POWER
  0x49, // KEYPAD_EQUAL
  0x64, // F13
  0x65, // F14
  0x66, // F15
  0x67, // F16
  0x68, // F17
  0x69, // F18
  0x6a, // F19
  0x6b, // F20
  // 0x70 - 0x73
  0x6c, // F21
  0x6d, // F22
  0x6e, // F23
  0x76  // F24
};

u8 const hid2ps2_2[] = {
  // 0x00 - 0x0f
  0, 0, 0, 0, // NONE
  0x1c, // A
  0x32, // B
  0x21, // C
  0x23, // D
  0x24, // E
  0x2b, // F
  0x34, // G
  0x33, // H
  0x43, // I
  0x3b, // J
  0x42, // K
  0x4b, // L
  // 0x10 - 0x1f
  0x3a, // M
  0x31, // N
  0x44, // O
  0x4d, // P
  0x15, // Q
  0x2d, // R
  0x1b, // S
  0x2c, // T
  0x3c, // U
  0x2a, // V
  0x1d, // W
  0x22, // X
  0x35, // Y
  0x1a, // Z
  0x16, // 1
  0x1e, // 2
  // 0x20 - 0x2f
  0x26, // 3
  0x25, // 4
  0x2e, // 5
  0x36, // 6
  0x3d, // 7
  0x3e, // 8
  0x46, // 9
  0x45, // 0
  0x5a, // ENTER
  0x76, // ESC
  0x66, // BACKSPACE
  0x0d, // TAB
  0x29, // SPACE
  0x4e, // -
  0x55, // =
  0x54, // [
  // 0x30 - 0x3f
  0x5b, // ]
  0x5d, // BACKSLASH
  0x5d, // EUROPE_1
  0x4c, // ;
  0x52, // ' APOSTROPHE
  0x0e, // ` GRAVE
  0x41, // ,
  0x49, // .
  0x4a, // /
  0x58, // CAPS
  0x05, // F1
  0x06, // F2
  0x04, // F3
  0x0c, // F4
  0x03, // F5
  0x0b, // F6
  // 0x40 - 0x4f
  0x83, // F7
  0x0a, // F8
  0x01, // F9
  0x09, // F10
  0x78, // F11
  0x07, // F12
  0x7c, // PRNT SCRN
  0x7e, // SCROLL
  0x7e, // PAUSE
  0x70, // INSERT
  0x6c, // HOME
  0x7d, // PG UP
  0x71, // DELETE
  0x69, // END
  0x7a, // PD DN
  0x74, // R ARROW
  // 0x50 - 0x5f
  0x6b, // L ARROW
  0x72, // D ARROW
  0x75, // U ARROW
  0x77, // NUM
  0x4a, // KP /
  0x7c, // KP *
  0x7b, // KP -
  0x79, // KP +
  0x5a, // KP EN
  0x69, // KP 1
  0x72, // KP 2
  0x7a, // KP 3
  0x6b, // KP 4
  0x73, // KP 5
  0x74, // KP 6
  0x6c, // KP 7
  // 0x60 - 0x6f
  0x75, // KP 8
  0x7d, // KP 9
  0x70, // KP 0
  0x71, // KP .
  0x61, // EUROPE_2
  0x2f, // APPS
  0x37, // POWER
  0x0f, // KEYPAD_EQUAL
  0x08, // F13
  0x10, // F14
  0x18, // F15
  0x20, // F16
  0x28, // F17
  0x30, // F18
  0x38, // F19
  0x40, // F20
  // 0x70 - 0x73
  0x48, // F21
  0x50, // F22
  0x57, // F23
  0x5f  // F24
};

u8 const hid2ps2_3[] = {
  // 0x00 - 0x0f
  0, 0, 0, 0, // NONE
  0x1C,       // A
  0x32,       // B
  0x21,       // C
  0x23,       // D
  0x24,       // E
  0x2B,       // F
  0x34,       // G
  0x33,       // H
  0x43,       // I
  0x3B,       // J
  0x42,       // K
  0x4B,       // L
  // 0x10 - 0x1f
  0x3A,       // M
  0x31,       // N
  0x44,       // O
  0x4D,       // P
  0x15,       // Q
  0x2D,       // R
  0x1B,       // S
  0x2C,       // T
  0x3C,       // U
  0x2A,       // V
  0x1D,       // W
  0x22,       // X
  0x35,       // Y
  0x1A,       // Z
  0x16,       // 1
  0x1E,       // 2
  // 0x20 - 0x2f
  0x26,       // 3
  0x25,       // 4
  0x2E,       // 5
  0x36,       // 6
  0x3D,       // 7 ?
  0x3E,       // 8
  0x46,       // 9
  0x45,       // 0
  0x5A,       // ENTER
  0x08,       // ESC
  0x66,       // BKSP
  0x0D,       // TAB
  0x29,       // SPACE
  0x4E,       // -
  0x55,       // =
  0x54,       // [
  // 0x30 - 0x3f
  0x5B,       // ]
  0x5C,       // BACKSLASH
  0x53,       // EUROPE_1
  0x4C,       // ;
  0x52,       // ' APOSTROPHE
  0x0E,       // ` GRAVE
  0x41,       // ,
  0x49,       // .
  0x4A,       // /
  0x14,       // CAPS
  0x07,       // F1
  0x0F,       // F2
  0x17,       // F3
  0x1F,       // F4
  0x27,       // F5
  0x2F,       // F6
  // 0x40 - 0x4f
  0x37,       // F7
  0x3F,       // F8
  0x47,       // F9
  0x4F,       // F10
  0x56,       // F11
  0x5E,       // F12
  0x57,       // PRNT SCRN
  0x5F,       // SCROLL
  0x62,       // PAUSE
  0x67,       // INSERT
  0x6E,       // HOME
  0x6F,       // PG UP
  0x64,       // DELETE
  0x65,       // END
  0x6D,       // PG DN
  0x6A,       // R ARROW
  // 0x50 - 0x5f
  0x61,       // L ARROW
  0x60,       // D ARROW
  0x63,       // U ARROW
  0x76,       // NUM
  0x77,       // KP /
  0x7E,       // KP *
  0x84,       // KP -
  0x7C,       // KP +
  0x79,       // KP EN
  0x69,       // KP 1
  0x72,       // KP 2
  0x7A,       // KP 3
  0x6B,       // KP 4
  0x73,       // KP 5
  0x74,       // KP 6
  0x6C,       // KP 7
  // 0x60 - 0x6f
  0x75,       // KP 8
  0x7D,       // KP 9
  0x70,       // KP 0
  0x71,       // KP .
  0x13,       // EUROPE_2
  0x00,       // APPS
  0x00,       // POWER
  0x00,       // KEYPAD_EQUAL
  0x00,       // F13
  0x00,       // F14
  0x00,       // F15
  0x00,       // F16
  0x00,       // F17
  0x00,       // F18
  0x00,       // F19
  0x00,       // F20
  // 0x70 - 0x73
  0x00,       // F21
  0x00,       // F22
  0x00,       // F23
  0x00        // F24
};


const uint8_t __in_flash() table_key_zx_hid[256] = 
 {
  NC, NC,/*00 Reserved*/
  NC, NC,/*01 Keyboard ErrorRollOver*/
  NC, NC,/*02 Keyboard POSTFail*/
  NC, NC,/*03 Keyboard ErrorUndefined[*/
  _A, NC,/*04 A*/
  _B, NC,/*05 B*/
  _C, NC,/*06 C*/
  _D, NC,/*07 D*/
  _E, NC,/*08 E*/
  _F, NC,/*09 F*/
  _G, NC,/*0A G*/
  _H, NC,/*0B H*/    
  _I, NC,/*0C I*/
  _J, NC,/*0D J*/
  _K, NC,/*0E K*/
  _L, NC,/*0F L*/
  
  _M, NC,/*10 M*/
  _N, NC,/*11 N*/
  _O, NC,/*12 O*/
  _P, NC,/*13 P*/
  _Q, NC,/*14 Q*/
  _R, NC,/*15 R*/
  _S, NC,/*16 S*/
  _T, NC,/*17 T*/  
  _U, NC,/*18 U*/
  _V, NC,/*19 V*/
  _W, NC,/*1A W*/
  _X, NC,/*1B X*/
  _Y, NC,/*1C Y*/
  _Z, NC,/*1D Z*/
  _1, NC,/*1E 1*/
  _2, NC,/*1F 2*/   

  _3, NC,/*20 3*/
  _4, NC,/*21 4*/
  _5, NC,/*22 5*/
  _6, NC,/*23 6*/
  _7, NC,/*24 7*/
  _8, NC,/*25 8*/
  _9, NC,/*26 9*/
  _0, NC,/*27 0*/
  EN, NC,/*28 Enter*/
  SP, CS,/*29 Escape/ Break CS+Space*/
  _0, CS,/*2A Keyboard Delete (Backspace)  CS+0*/
  _1, CS,/*2B Tab / Edit CS+1*/
  SP, NC,/*2C Space*/    
  _J, SS,/*2D -      SS+J*/
  _K, SS,/*2E +      SS+K*/
  NC, NC,/*2F [*/ 

  _L, SS,/*30 SS+L*/
  NC, NC,/*31 \*/
  NC, NC,/*32 `*/
  _O, SS,/*33 ; SS+O*/
  _P, SS,/*34 SS+P  "*/
  NC, NC,/*35 Keyboard Grave Accent and Tilde*/
  _M, SS,/*36 , SS+M*/
  _N, SS,/*37 . SS+N*/
  _C, SS,/*38 ? SS+C*/
  _2, CS,/*39 Caps Lock  CS+2*/
  NC, NC,/*3A F1*/
  NC, NC,/*3B F2*/    
  NC, NC,/*3C F3*/
  NC, NC,/*3D F4*/
  NC, NC,/*3E F5*/
  NC, NC,/*3F F6*/

  NC, NC,/*40 F7 */
  NC, NC,/*41 F8 */
  NC, NC,/*42 F9 */
  NC, NC,/*43 F10 */
  NC, NC,/*44 F11 */
  NC, NC,/*45 F12 */
  _9, CS,/*46 PrintScreen Graph Mode CS+9*/
  NC, NC,/*47 Scroll Lock*/
  NC, NC,/*48 Pause */
  NC, NC,/*49 Insert */
  NC, NC,/*4A Home */
  _3, CS,/*4B PageUp CS+3 True Video*/    
  NC, NC,/*4C Del */
  NC, NC,/*4D End */
  _4, CS,/*4E PageDown CS+4 Inv Video*/
  _8, CS,/*4F RightArrow CS+8*/
  
  _5, CS,/*50 LeftArrow CS+5*/
  _6, CS,/*51 DownArrow  CS+6*/
  _7, CS,/*52 UpArrow CS+7*/
  NC, NC,/*53 Keypad Num Lock and Clear*/
  _V, SS,/*54 Keypad / SS+V*/
  NC, NC,/*55 Keypad * */
  _I, SS,/*56 Keypad - */
  _K, SS,/*57 Keypad + */  
  EN, NC,/*58 Keypad Enter */
  _1, NC,/*59 Keypad 1 */
  _2, NC,/*5A Keypad 2 */
  _3, NC,/*5B Keypad 3 */
  _4, NC,/*5C Keypad 4 */
  _5, NC,/*5D Keypad 5 */
  _6, NC,/*5E Keypad 6 */
  _7, NC,/*5F Keypad 7 */ 

  _8, NC,/*60 Keypad 8 */
  _9, NC,/*61 Keypad 9 */
  _0, NC,/*62 Keypad 0 */
  NC, NC,/*63 Keypad Del . */
  NC, NC,/*64*/
  NC, NC,/*65*/
  NC, NC,/*66*/
  NC, NC,/*67 Keypad =*/
  NC, NC,/*68 F13*/
  NC, NC,/*69 F14*/
  NC, NC,/*6A F15*/
  NC, NC,/*6B F16*/    
  NC, NC,/*6C F17*/
  NC, NC,/*6D F18*/
  NC, NC,/*6E F19*/
  NC, NC,/*6F F20*/ 
/* клавиши ALT CTRL SHIFT  специально для ZX  */
  SS, CS,/*70 F21 ALT   Ext.Mode SS+CS  */
  CS, NC,/*71 F22 CTRL  / CS ZX  */ 
  SS, NC,/*72 F23 SHIFT / SS ZX  */
  NC, NC,/*73 F24 WIN */
  NC, NC,/*74*/
  NC, NC,/*75*/
  NC, NC,/*76*/
  NC, NC,/*77*/
  NC, NC,/*78*/
  NC, NC,/*79*/
  NC, NC,/*7A*/
  NC, NC,/*7B*/    
  NC, NC,/*7C*/
  NC, NC,/*7D*/
  NC, NC,/*7E*/
  NC, NC,/*7F*/   
 };

 const uint8_t __in_flash() macro1[]  = { 0x7C, 0xF0, 0x7C, 0x37, 0 }; //*"A : "
 const uint8_t __in_flash() macro2[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 }; //RANDOMIZE USR 15616
 const uint8_t __in_flash() macro3[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 }; //PageUP
 const uint8_t __in_flash() macro4[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 }; //PageDown
 const uint8_t __in_flash() macro5[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 }; //RUN "BOOT" 
 const uint8_t __in_flash() macro6[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 }; //RUN ""
 const uint8_t __in_flash() macro7[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 };
 const uint8_t __in_flash() macro8[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 };
 const uint8_t __in_flash() macro9[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 };
 const uint8_t __in_flash() macro10[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 };
 const uint8_t __in_flash() macro11[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 };
 const uint8_t __in_flash() macro12[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 };
 
 
 /*
 KEY 	MAKE 	    BREAK 	    KEY 	  MAKE 	BREAK
 A 	    1C 	    F0,1C 	    R ALT 	E0,11 	E0,F0,11
 B 	    32 	    F0,32 	    APPS 	  E0,2F 	E0,F0,2F
 C 	    21 	    F0,21 	    ENTER 	5A 	    F0,5A
 D 	    23 	    0,23 	      ESC 	  76 	    F0,76
 E 	    24 	    F0,24 	    F1 	    05 	    F0,05
 F 	    2B 	    F0,2B 	    F2 	    06 	    F0,06
 G 	    34 	    F0,34 	    F3 	    04 	    F0,04
 H 	    33 	    F0,33 	    F4 	    0C 	    F0,0C
 I 	    43 	    F0,43 	    F5 	    03 	    F0,03
 J 	    3B 	    F0,3B 	    F6 	    0B 	    F0,0B
 K 	    42 	    F0,42 	    F7 	    83 	    F0,83
 L 	    4B 	    F0,4B 	    F8 	    0A 	    F0,0A
 M 	    3A 	    F0,3A 	    F9 	    01 	    F0,01
 N 	    31 	    F0,31 	    F10 	  09 	    F0,09
 O 	    44 	    F0,44 	    F11 	  78 	    F0,78
 P 	    4D 	    F0,4D 	    F12 	  07 	    F0,07
 Q 	    15 	    F0,15 	    SCROLL	7E 	    F0,7E
 R 	    2D 	    F0,2D 	    [ 	    54 	    FO,54
 S 	    1B 	    F0,1B 	    INSERT	E0,70 	E0,F0,70
 T 	    2C 	    F0,2C 	    HOME 	  E0,6C 	E0,F0,6C
 U 	    3C 	    F0,3C 	    PG UP 	E0,7D 	E0,F0,7D
 V 	    2A 	    F0,2A 	    DELETE	E0,71 	E0,F0,71
 W 	    1D 	    F0,1D 	    END 	  E0,69 	E0,F0,69
 X 	    22 	    F0,22 	    PG DN 	E0,7A 	E0,F0,7A
 Y 	    35 	    F0,35 	    UP 	    E0,75 	E0,F0,75
 Z 	    1A 	    F0,1A 	    LEFT 	  E0,6B 	E0,F0,6B
 0 	    45 	    F0,45 	    DOWN 	  E0,72 	E0,F0,72
 1 	    16 	    F0,16 	    RIGHT 	E0,74 	E0,F0,74
 2 	    1E 	    F0,1E 	    NUM 	  77 	    F0,77
 3 	    26 	    F0,26 	    KP / 	  E0,4A 	E0,F0,4A
 4 	    25 	    F0,25 	    KP * 	  7C 	    F0,7C
 5 	    2E 	    F0,2E 	    KP - 	  7B 	    F0,7B
 6 	    36 	    F0,36 	    KP + 	  79 	    F0,79
 7 	    3D 	    F0,3D 	    KP EN 	E0,5A 	E0,F0,5A
 8 	    3E 	    F0,3E 	    KP .    71 	    F0,71
 9 	    46 	    F0,46 	    KP 0 	  70 	    F0,70
 ` 	    0E 	    F0,0E 	    KP 1 	  69 	    F0,69
 - 	    4E 	    F0,4E 	    KP 2 	  72 	    F0,72
 = 	    55 	    FO,55 	    KP 3 	  7A 	    F0,7A
 \ 	    5D 	    F0,5D 	    KP 4 	  6B 	    F0,6B
 BKSP 	66 	    F0,66 	    KP 5 	  73 	    F0,73
 SPACE 	29 	    F0,29 	    KP 6 	  74 	    F0,74
 TAB 	  0D 	    F0,0D 	    KP 7 	  6C  	  F0,6C
 CAPS 	58 	    F0,58 	    KP 8 	  75 	    F0,75
 L SHFT	12 	    FO,12 	    KP 9 	  7D 	    F0,7D
 L CTRL	14 	    FO,14 	    ] 	    5B 	    F0,5B
 L GUI 	E0,1F 	E0,F0,1F	  ; 	    4C 	    F0,4C
 L ALT 	11 	    F0,11 	    ' 	    52 	    F0,52
 R SHFT	59 	    F0,59 	    , 	    41 	    F0,41
 R CTRL	E0,14 	E0,F0,14	  . 	    49 	    F0,49
 R GUI 	E0,27 	E0,F0,27	  / 	    4A 	    F0,4A
 
 */
 
 /*
 Left/Right Shift 	      N/A 	                                                  CAPS SHIFT 	      Modifies the other keys you’ll press with it
 Left/Right CTRL 	        N/A 	                                                  SYMBOL SHIFT 	    Modifies the other keys you’ll press with it
 Left/Right ALT  	        Green cursor in NextBASIC/E cursor on 48K BASICs 	      EXTEND MODE 	    Presses SYMBOL SHIFT and CAPS SHIFT together
 Cursor Keys 	            Cursor will move 	                                      CAPS SHIFT + / 5,6,7,8 	Emulates the ZX Spectrum + cursor keys
 PgUp 	                  Reset Inverse Video for 48K / See Chp. 1 for NextBASIC 	CAPS SHIFT + 3 	  Emulates the ZX Spectrum + TRUE VIDEO key
 PgDn 	                  Sets Inverse Video for 48K / See Chp. 1 for NextBASIC 	CAPS SHIFT + 4 	  Emulates the ZX Spectrum + INVERSE VIDEO key
 ESC 	                    Either Space or BREAK INTO PROGRAM i           	        CAPS SHIFT + SPACE 	Breaks the execution of a program
 Left Windows Key / Home 	{ on NextBASIC / TO on 48K BASIC 	                      SYMBOL SHIFT + F 	Prints a symbol / token
 Right Windows Key / End 	} on NextBASIC / THEN on 48K BASIC 	                    SYMBOL SHIFT + G 	Prints a symbol / token
 Scroll Lock 	            Magenta Cursor on NextBASIC / G cursor on 48K Basic 	  CAPS SHIFT + 9 /  GRAPH key on +/128 	Sets / resets GRAPHICS mode
 PrtScr 	                (c) symbol 	                                            EXTEND MODE + P 	Prints a symbol
 ‘,” key 	                Double Quotes ‘”‘ 	                                    SYMBOL SHIFT + P 	Emulates the ZX Spectrum + ” key
 , key 	                  Comma “,” 	                                            SYMBOL SHIFT + N 	Emulates the ZX Spectrum + , key
 . key 	                  Period “.” 	                                            SYMBOL SHIFT + M 	Emulates the ZX Spectrum + . key
 ;,: key 	                Semicolon “;” 	                                        SYMBOL SHIFT + O 	Emulates the ZX Spectrum + ; key
 [,{ key 	                [ 	                                                    SYMBOL SHIFT + Y 	Prints a symbol
 ],} key 	                ] 	                                                    SYMBOL SHIFT + U 	Prints a symbol
 /,? key 	                Forward slash “/” 	                                    SYMBOL SHIFT + V 	Prints a symbol
 \,| key 	                Backward slash “\” on NextBASIC / STEP on 48K BASIC 	  SYMBOL SHIFT + D 	Prints a symbol / token
 =,+ key 	                Equals sign “=” 	                                      SYMBOL SHIFT + L 	Prints a symbol
 -,_ key 	                Minus sign “-“ 	                                        SYMBOL SHIFT + J 	Prints a symbol
 `,~ key 	                Single quote “‘” 	                                    S YMBOL SHIFT + 7 	Prints a symbol
 Ins 	                    Tilde “~” in NextBASIC / STOP in 48K BASIC 	            SYMBOL SHIFT + A 	Prints a symbol / token
 Del 	                    Question mark “?” 	                                    SYMBOL SHIFT + C 	Prints a symbol
 Numeric Keypad keys 	    Number on keyboard 	                                    0…9 	            Prints a number
 Numeric “/” 	            Forward slash “/” 	                                    SYMBOL SHIFT + V 	Prints a symbol
 Numeric “*” 	            Asterisk “*” 	                                          SYMBOL SHIFT + B 	Prints a symbol
 Numeric “-“ 	            Minus “-“ 	                                            SYMBOL SHIFT + J 	Prints a symbol
 Numeric “+” 	            Plus “+” 	                                              SYMBOL SHIFT + K 	Prints a symbol
 Numeric “.” 	            Period “.” 	                                            SYMBOL SHIFT + M 	Prints a symbol
 Numeric “Enter” 	        ENTER 	                                                ENTER 	          Presses Enter
 ENTER 	                  ENTER 	                                                ENTER 	          Presses Enter
 A…Z keys 	              A…Z 	A…Z 	Prints character
 
 */
 
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
 
 
 
 const uint8_t __in_flash() table_key_zx_ps[][4]  =
 {
 {NC, NC, NC, NC},/*00*/  
 {NC, NC, NC, NC},/*(01) F9 */
 {NC, NC, NC, NC},/*(02)*/
 {NC, NC, NC, NC},/*(03) F5 */
 {_3, CS, NC, NC},/*(04) F3 */
 {NC, NC, NC, NC},/*(05) F1 */
 {NC, NC, NC, NC},/*(06) F2 */
 {NC, NC, NC, NC},/*(07) F12 */
 {NC, NC, NC, NC},/*(08) F13 */
 {NC, NC, NC, NC},/*(09) F10 */
 {NC, NC, NC, NC},/*(0A) F8 */
 {NC, NC, NC, NC},/*(0B) F6 */
 {_4, CS, NC, NC},/*(0C) F4 */
 {SS, CS, NC, NC},/*(0D) Tab CS+SS */
 {_1, CS, NC, NC},/*(0E) Keyboard Grave Accent and Tilde  / Edit CS+1 */  // ` GRAVE
 {NC, NC, NC, NC},/*(0F) Keypad = */
 {NC, NC, NC, NC},/*(10) F14 */
 {NC, NC, NC, NC},/*(11) Left ALt */
 {CS, NC, NC, NC},/*(12) Left Shift */
 {NC, NC, NC, NC},/*(13)*/
 {SS, NC, NC, NC},/*(14)*/
 {_Q, NC, NC, NC},/*(15) Q */
 {_1, NC, NC, NC},/*(16) 1 */
 {NC, NC, NC, NC},/*(17)*/
 {NC, NC, NC, NC},/*(18) F15 */
 {NC, NC, NC, NC},/*(19)*/
 {_Z, NC, NC, NC},/*(1A) Z */
 {_S, NC, NC, NC},/*(1B) S */
 {_A, NC, NC, NC},/*(1C) A */ 	
 {_W, NC, NC, NC},/*(1D) W */
 {_2, NC, NC, NC},/*(1E) 2 */ 
 {NC, NC, NC, NC},/*(1F)*/
 {NC, NC, NC, NC},/*(20) F16 */
 {_C, NC, NC, NC},/*(21) C */
 {_X, NC, NC, NC},/*(22) X */
 {_D, NC, NC, NC},/*(23) D */
 {_E, NC, NC, NC},/*(24) E */
 {_4, NC, NC, NC},/*(25) 4 */
 {_3, NC, NC, NC},/*(26) 3 */
 {NC, NC, NC, NC},/*(27)*/
 {NC, NC, NC, NC},/*(28) F17 */
 {SP, NC, NC, NC},/*(29) Space */
 {_V, NC, NC, NC},/*(2A) V */
 {_F, NC, NC, NC},/*(2B) F */
 {_T, NC, NC, NC},/*(2C) T */
 {_R, NC, NC, NC},/*(2D) R */
 {_5, NC, NC, NC},/*(2E) 5 */
 {NC, NC, NC, NC},/*(2F)*/ //E0 2f APPS	
 {NC, NC, NC, NC},/*(30) F18 */
 {_N, NC, NC, NC},/*(31) N */
 {_B, NC, NC, NC},/*(32) B */ 
 {_H, NC, NC, NC},/*(33) H */
 {_G, NC, NC, NC},/*(34) G */
 {_Y, NC, NC, NC},/*(35) Y */
 {_6, NC, NC, NC},/*(36) 6 */
 {NC, NC, NC, NC},/*(37)*/ //POWER	
 {NC, NC, NC, NC},/*(38) F19 */ 
 {NC, NC, NC, NC},/*(39)*/
 {_M, NC, NC, NC},/*(3A) M */
 {_J, NC, NC, NC},/*(3B) J */
 {_U, NC, NC, NC},/*(3C) U */
 {_7, NC, NC, NC},/*(3D) 7 */
 {_8, NC, NC, NC},/*(3E) 8 */
 {NC, NC, NC, NC},/*(3F)*/
 {NC, NC, NC, NC},/*(40) F20 */ 
 {_N, SS, NC, NC},/*(41) . SS+N  , SS+M */ // .	
 {_K, NC, NC, NC},/*(42) K */
 {_I, NC, NC, NC},/*(43) I */
 {_O, NC, NC, NC},/*(44) O */
 {_0, NC, NC, NC},/*(45) 0 */
 {_9, NC, NC, NC},/*(46) 9 */
 {NC, NC, NC, NC},/*(47)*/
 {SS, CS, NC, NC},/*(48) F21 ALT   Ext.Mode SS+CS  */
 {_M, SS, NC, NC},/*(49) , SS+M  . SS+N */ // ,
 {_C, SS, _V, SS},/*(4A) ? SS+C */ /*E0 4A Keypad / SS+V */
 {_L, NC, NC, NC},/*(4B) L */
 {_Z, SS, NC, NC},/*(4С) : SS+Z ; SS+O */ // :
 {_P, NC, NC, NC},/*(4D) P*/
 {_J, SS, NC, NC},/*(4E) - SS+J*/
 {NC, NC, NC, NC},/*(4F)*/
 {CS, NC, NC, NC},/*(50) F22 CTRL  / CS ZX  */ 
 {NC, NC, NC, NC},/*(51)*/
 {_P, SS, NC, NC},/*(52) SS+P  "*/ // ' APOSTROPHE
 {NC, NC, NC, NC},/*(53)*/
 {NC, NC, NC, NC},/*(54) [*/
 {_K, SS, NC, NC},/*(55) +  SS+K*/
 {NC, NC, NC, NC},/*(56)*/
 {SS, NC, NC, NC},/*(57) F23 SHIFT / SS ZX  */
 {_2, CS, NC, NC},/*(58) Caps Lock  CS+2*/ // CAPS
 {CS, NC, NC, NC},/*(59) Casp Shift*/
 {EN, NC, EN, NC},/*(5A) Enter*/  /*E0 5A {EN, NC} Keypad Enter */
 {_L, SS, NC, NC},/*(5B) SS+L*/ // ]	
 {NC, NC, NC, NC},/*(5C)*/
 {NC, NC, NC, NC},/*(5D) \ */ /* BACKSLASH 5D {NC, NC} `*/ // EUROPE_1
 {NC, NC, NC, NC},/*(5E)*/
 {NC, NC, NC, NC},/*(5F) F24 WIN */
 {NC, NC, NC, NC},/*(60)*/
 {NC, NC, NC, NC},/*(61)*/
 {NC, NC, NC, NC},/*(62)*/
 {NC, NC, NC, NC},/*(63)*/
 {NC, NC, NC, NC},/*(64)*/
 {NC, NC, NC, NC},/*(65)*/
 {_0, CS, NC, NC},/*(66) Keyboard Delete (Backspace)  CS+0 ???????*/
 {NC, NC, NC, NC},/*(67)*/
 {NC, NC, NC, NC},/*(68)*/
 {_1, NC, NC, NC},/*(69) Keypad 1 */  /*E0 69 {NC, NC} End */
 {NC, NC, NC, NC},/*(6A)*/
 {_4, NC, _5, CS},/*(6B) Keypad 4 */  /*E0 6B {_5, CS} LeftArrow CS+5*/
 {_7, NC, NC, NC},/*(6C) Keypad 7 */  /*E0 6B {NC, NC}, Home */
 {NC, NC, NC, NC},/*(6D)*/
 {NC, NC, NC, NC},/*(6E)*/
 {NC, NC, NC, NC},/*(6F)*/
 {_0, NC, NC, NC},/*(70) Keypad 0 */   /*E0 70 {NC, NC} Insert */
 {NC, NC, NC, NC},/*(71) Keypad Del.*/ /*E0 71 {NC, NC} Del */
 {_2, NC, _6, CS},/*(72) Keypad 2 */   /*E0 72 {_6, CS} DownArrow CS+6 */
 {_5, NC, NC, NC},/*(73) Keypad 5 */  
 {_6, NC, _8, CS},/*(74) Keypad 6 */   /*E0 74 {_8, CS} RightArrow CS+8 */
 {_8, NC, _7, CS},/*(75) Keypad 8 */   /*E0 75 {_7, CS} UpArrow CS+7 */
 {SP, CS, NC, NC},/*(76) Escape/ Break CS+Space */
 {NC, NC, NC, NC},/*(77) Keypad Num Lock and Clear */
 {NC, NC, NC, NC},/*(78) F11 */
 {_K, SS, NC, NC},/*(79) Keypad + */ 
 {_3, NC, _4, CS},/*(7A) Keypad 3 */ /*E0 7A {_4, CS} PageDown CS+4 Inv Video */
 {_I, SS, NC, NC},/*(7B) Keypad - */
 {NC, NC ,_9, CS},/*(7C) Keypad * */ /*E0 7C {_9, CS} PrintScreen Graph Mode CS+9 */
 {_9, NC, _3, CS},/*(7D) Keypad 9 */ /*E0 7D {_3, CS} PageUp CS+3 True Video */
 {NC, NC, NC, NC},/*(7E) Scroll Lock */ /*E1 14 77 E1 F0 14 F0 77 7E {NC, NC} Pause */
 {NC, NC, NC, NC},/*(7F)*/
 {NC, NC, NC, NC},/*(80)*/
 {NC, NC, NC, NC},/*(81)*/
 {NC, NC, NC, NC},/*(82)*/
 {NC, NC, NC, NC},/*(83) F7 */
 {NC, NC, NC, NC}/*(84)*/
 };
 // 0x10 - 0x1f			  
 // 0x20 - 0x2f			
 // 0x30 - 0x3f			
 // 0x40 - 0x4f			
 // 0x50 - 0x5f			  
 // 0x60 - 0x6f			
 // 0x70 - 0x73			/* клавиши ALT CTRL SHIFT  специально для ZX  */



    //scancode_s(pack[0]); // запись в таблицу 0 кода alt ctrl shift win
    // tab_key[report[1]]=1;// запись в таблицу 1 кода reserved
    //tab_key[pack[0]] = 1; // запись в таблицу 2 кода
/*


//---------------------------------------------------
void scancode_s(uint8_t code)
{
   if (code & 0x02) tab_key[0x71]=1;    // left shift  
   if (code & 0x20) tab_key[0x71]=1;    // righr shift 

   if (code & 0x04) tab_key[0x70]=1;    // left alt    0000 0100
   if (code & 0x40) tab_key[0x70]=1;    // right alt   0100 0000
   
   if (code & 0x01) tab_key[0x72]=1;    // right ctrl   
   if (code & 0x10) tab_key[0x72]=1;    // left ctrl
   
   if (code & 0x08) tab_key[0x73]=1;    // win  l
   if (code & 0x80) tab_key[0x73]=1;    // win r
   }

//----------------------------------------------------------
            i++;
        pack[i] = byte;
        }
        pack[0] = i;
        //queue_try_add(&this->qpacks, &pack);
    
    if (pack[0]!=0){
        for (uint8_t i = 0; i < 127; i++)
        {
            uint8_t d = (tab_key[i] << 1) | (tab_key_old[i]); // 0b000000x0 |0b0000000y
            // if (d==0) // не нажато уже выключать не нужно
            if ((d==0) && (pack[1] != 0xFF)){
                key_on(pack[1]); // нажато сейчас
                tab_key[pack[1]] = 1;
            }   
            // if (d==3) // нажато уже включать не нужно
            else {
                key_off(pack[2]);              // клавиша отпущена  сейчас
                //tab_key[pack[1]] = 0;
            }
            tab_key_old[i] = tab_key[i]; // копирование таблицы
            tab_key[i] = 0;              // стирание таблицы
        }
        }
        */