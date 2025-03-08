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
#include "ps2x2pico.h"

ps2out kb_out;
ps2in kb_in;

#define KBHOSTCMD_RESET_FF 0xff
#define KBHOSTCMD_RESEND_FE 0xfe
#define KBHOSTCMD_SCS3_SET_KEY_MAKE_FD 0xfd
#define KBHOSTCMD_SCS3_SET_KEY_MAKE_BREAK_FC 0xfc
#define KBHOSTCMD_SCS3_SET_KEY_MAKE_TYPEMATIC_FB 0xfb
#define KBHOSTCMD_SCS3_SET_ALL_MAKE_BREAK_TYPEMATIC_FA 0xfa
#define KBHOSTCMD_SCS3_SET_ALL_MAKE_F9 0xf9
#define KBHOSTCMD_SCS3_SET_ALL_MAKE_BREAK_F8 0xf8
#define KBHOSTCMD_SCS3_SET_ALL_MAKE_TYPEMATIC_F7 0xf7
#define KBHOSTCMD_SET_DEFAULT_F6 0xf6
#define KBHOSTCMD_DISABLE_F5 0xf5
#define KBHOSTCMD_ENABLE_F4 0xf4
#define KBHOSTCMD_SET_TYPEMATIC_PARAMS_F3 0xf3
#define KBHOSTCMD_READ_ID_F2 0xf2
#define KBHOSTCMD_SET_SCAN_CODE_SET_F0 0xf0
#define KBHOSTCMD_ECHO_EE 0xee
#define KBHOSTCMD_SET_LEDS_ED 0xed

#define KB_MSG_SELFTEST_PASSED_AA 0xaa
#define KB_MSG_ID1_AB 0xab
#define KB_MSG_ID2_83 0x83
#define KB_MSG_ECHO_EE 0xee
#define KB_MSG_ACK_FA 0xfa
#define KB_MSG_RESEND_FE 0xfe

typedef enum {
  KBH_STATE_IDLE,
  KBH_STATE_SET_LEDS_ED,
  KBH_STATE_SET_SCAN_CODE_SET_F0,
  KBH_STATE_SET_TYPEMATIC_PARAMS_F3,
  KBH_STATE_SET_KEY_MAKE_FD,
  KBH_STATE_SET_KEY_MAKE_BREAK_FC,
  KBH_STATE_SET_KEY_MAKE_TYPEMATIC_FB
} kbhost_state_enum_t;

kbhost_state_enum_t kbhost_state = KBH_STATE_IDLE;

typedef enum {
  SCS3_MODE_MAKE,
  SCS3_MODE_MAKE_BREAK,
  SCS3_MODE_MAKE_TYPEMATIC,
  SCS3_MODE_MAKE_BREAK_TYPEMATIC,
} scs3_mode_enum_t;

scs3_mode_enum_t scs3_mode = SCS3_MODE_MAKE_BREAK_TYPEMATIC;

#define SCAN_CODE_SET_1 1
#define SCAN_CODE_SET_2 2
#define SCAN_CODE_SET_3 3

u8 scancodeset = SCAN_CODE_SET_2;

#define HOST_CMD_MIN 0xe0
#define KEYMODEMASK_BREAK 0b00000001
#define KEYMODEMASK_TYPEMATIC 0b00000010

u8 scs3keymodemap[HOST_CMD_MIN];

u8 const led2ps2[] = { 0, 4, 1, 5, 2, 6, 3, 7 };



u32 const repeats[] = {
  33333, 37453, 41667, 45872, 48309, 54054, 58480, 62500,
  66667, 75188, 83333, 91743, 100000, 108696, 116279, 125000,
  133333, 149254, 166667, 181818, 200000, 217391, 232558, 250000,
  270270, 303030, 333333, 370370, 400000, 434783, 476190, 500000
};
u16 const delays[] = { 250, 500, 750, 1000 };

bool kb_enabled = true;
bool blinking = false;
u8 key2repeat = 0;
u8 last_byte_sent = 0;
u32 repeat_us;
u16 delay_ms;
alarm_id_t repeater;

void kb_send(u8 byte) {
  if(byte != KB_MSG_RESEND_FE) last_byte_sent = byte;
  #if DEBUG==1
  printf("kb > host %02x\n", byte);
  #endif
  queue_try_add(&kb_out.qbytes, &byte);
  ws2812_set_rgb(0, LEDBR, 0);
}

void kb_resend_last() {
  #if DEBUG==1 
  printf("r: k>h %x\n", last_byte_sent);
  #endif
  queue_try_add(&kb_out.qbytes, &last_byte_sent);
}

void kb_maybe_send_prefix(u8 key) {
  u8 const *l = IS_MOD_KEY(key) ? ext_code_modifier_keys_1_2 : ext_code_keys_1_2;
  for(int i = 0; l[i]; i++) {
    if(key == l[i]) {
      kb_send(KB_EXT_PFX_E0);
      break;
    }
  }
}

// sends out scan codes from a null byte terminated list
void kb_send_sc_list(const u8 *list) {
  key2repeat = 0;
  for(int i = 0; list[i]; i++) {
    kb_send(list[i]);
  }
}

void kb_set_leds(u8 byte) {
  
  if(byte > 7) byte = 0;
  tuh_kb_set_leds(led2ps2[byte]);
  ps2in_set(&kb_in, 0xed, byte);
  
}

s64 blink_callback() {
  if(blinking) {
    #if DEBUG==1
    printf("Blinking keyboard LEDs\n");
    #endif
    kb_set_leds(KEYBOARD_LED_NUMLOCK | KEYBOARD_LED_CAPSLOCK | KEYBOARD_LED_SCROLLLOCK);
    blinking = false;
    return 500000;
  }
  kb_set_leds(0);
  return 0;
}

void set_scancodeset(u8 scs) {
  scancodeset = scs;
  #if DEBUG==1
  printf("scancodeset set to %u\n", scancodeset);
  #endif
}

void kb_set_defaults() {
  #if DEBUG==1
  printf("Setting defaults for keyboard\n");
  #endif
  kbhost_state = KBH_STATE_IDLE;
  scs3_mode = SCS3_MODE_MAKE_BREAK_TYPEMATIC;
  set_scancodeset(2);
  kb_enabled = true;
  repeat_us = 91743;
  delay_ms = 500;
  blinking = true;
  add_alarm_in_ms(100, blink_callback, NULL, false);
  //#ifdef KBIN
  ps2in_reset(&kb_in);
  //#endif
}

s64 repeat_cb() {
  if(key2repeat) {
    switch(scancodeset) {
      case SCAN_CODE_SET_1:
        kb_maybe_send_prefix(key2repeat);
        IS_MOD_KEY(key2repeat) ? kb_send(mod2ps2_1[key2repeat - HID_KEY_CONTROL_LEFT]) : kb_send(hid2ps2_1[key2repeat]);
      break;
      case SCAN_CODE_SET_2:
        kb_maybe_send_prefix(key2repeat);
        IS_MOD_KEY(key2repeat) ? kb_send(mod2ps2_2[key2repeat - HID_KEY_CONTROL_LEFT]) : kb_send(hid2ps2_2[key2repeat]);
      break;
      case SCAN_CODE_SET_3:
        IS_MOD_KEY(key2repeat) ? kb_send(mod2ps2_3[key2repeat - HID_KEY_CONTROL_LEFT]) : kb_send(hid2ps2_3[key2repeat]);
      break;
      default:
        repeater = 0;
      return 0;
    }
    return repeat_us;
  }
  repeater = 0;
  return 0;
}

#define LOG_UNMAPPED_KEY printf("WARNING: Unmapped HID key 0x%x in set %d, ignoring it!\n",key,scancodeset);

void kb_send_key_scs1(u8 key, bool is_key_pressed, bool is_ctrl) {

  // PrintScreen and Pause have special sequences that must be sent.
  // Pause doesn't have a break code.
  if(key == HID_KEY_PAUSE || key == HID_KEY_PRINT_SCREEN) {
    if(is_key_pressed  && key == HID_KEY_PRINT_SCREEN)      kb_send_sc_list(prt_scn_make_1);
    if(!is_key_pressed && key == HID_KEY_PRINT_SCREEN)      kb_send_sc_list(prt_scn_break_1);
    if(is_key_pressed  && key == HID_KEY_PAUSE && is_ctrl)  kb_send_sc_list(break_make_1);
    if(is_key_pressed  && key == HID_KEY_PAUSE && !is_ctrl) kb_send_sc_list(pause_make_1);
    return;
  }

  u8 scan_code = IS_MOD_KEY(key) ? mod2ps2_1[key - HID_KEY_CONTROL_LEFT] : hid2ps2_1[key];

  if(!scan_code) {
    #if DEBUG==1
    LOG_UNMAPPED_KEY
    #endif
    return;
  }

  // Some keys require a prefix before the actual code
  kb_maybe_send_prefix(key);

  if(is_key_pressed) {
    // Take care of typematic repeat
    key2repeat = key;
    if(repeater) cancel_alarm(repeater);
    repeater = add_alarm_in_ms(delay_ms, repeat_cb, NULL, false);

    kb_send(scan_code);
  } else {
    // Cancel repeat
    if(key == key2repeat) key2repeat = 0;

    kb_send(scan_code | 0x80);
  }
}

void kb_send_key_scs2(u8 key, bool is_key_pressed, bool is_ctrl) {

  // PrintScreen and Pause have special sequences that must be sent.
  // Pause doesn't have a break code.
  if(key == HID_KEY_PAUSE || key == HID_KEY_PRINT_SCREEN) {
    if(is_key_pressed  && key == HID_KEY_PRINT_SCREEN)      kb_send_sc_list(prt_scn_make_2);
    if(!is_key_pressed && key == HID_KEY_PRINT_SCREEN)      kb_send_sc_list(prt_scn_break_2);
    if(is_key_pressed  && key == HID_KEY_PAUSE && is_ctrl)  kb_send_sc_list(break_make_2);
    if(is_key_pressed  && key == HID_KEY_PAUSE && !is_ctrl) kb_send_sc_list(pause_make_2);
    return;
  }

  u8 scan_code = IS_MOD_KEY(key) ? mod2ps2_2[key - HID_KEY_CONTROL_LEFT] : hid2ps2_2[key];

  if(!scan_code) {
    LOG_UNMAPPED_KEY
    return;
  }

  // Some keys require a prefix before the actual code
  kb_maybe_send_prefix(key);

  if(is_key_pressed) {
  // Take care of typematic repeat
    key2repeat = key;
    if(repeater) cancel_alarm(repeater);
    repeater = add_alarm_in_ms(delay_ms, repeat_cb, NULL, false);
  } else {
    if(key == key2repeat) key2repeat = 0;
    kb_send(KB_BREAK_2_3);
  }
  kb_send(scan_code);
}

void kb_send_key_scs3(u8 key, bool is_key_pressed) {

  u8 scan_code = IS_MOD_KEY(key) ? mod2ps2_3[key - HID_KEY_CONTROL_LEFT] : hid2ps2_3[key];

  if(!scan_code) {
    LOG_UNMAPPED_KEY
    return;
  }

  if(is_key_pressed) {
    // Take care of typematic repeat
    if(
      (scs3_mode == SCS3_MODE_MAKE_BREAK_TYPEMATIC || scs3_mode == SCS3_MODE_MAKE_TYPEMATIC)
      && !(scs3keymodemap[scan_code] & KEYMODEMASK_TYPEMATIC)
    ) {
      key2repeat = key;
      if(repeater) cancel_alarm(repeater);
      repeater = add_alarm_in_ms(delay_ms, repeat_cb, NULL, false);
    }

    kb_send(scan_code);
  } else {
    if(key == key2repeat) key2repeat = 0;

    if(
      (scs3_mode == SCS3_MODE_MAKE_BREAK || scs3_mode == SCS3_MODE_MAKE_BREAK_TYPEMATIC)
      && !(scs3keymodemap[scan_code] & KEYMODEMASK_BREAK)
    ) {
      kb_send(KB_BREAK_2_3);
      kb_send(scan_code);
    }
  }
}

// Sends a key state change to the host
// u8 keycode          - from hid.h HID_KEY_ definition
// bool is_key_pressed - state of key: true=pressed, false=released
void kb_send_key(u8 key, bool is_key_pressed, u8 modifiers) {
  if(!kb_enabled) {
    #if DEBUG==1
    printf("WARNING: Keyboard disabled, ignoring key press %u\n", key);
    #endif
    return;
  }

  if(!IS_VALID_KEY(key)) {
    #if DEBUG==1
    printf("INFO: Ignoring hid key 0x%x by design.\n", key);
    #endif
    return;
  }
  
  bool is_ctrl = modifiers & KEYBOARD_MODIFIER_LEFTCTRL || modifiers & KEYBOARD_MODIFIER_RIGHTCTRL;

  switch(scancodeset) {
    case SCAN_CODE_SET_1:
      kb_send_key_scs1(key, is_key_pressed, is_ctrl);
      break;
    case SCAN_CODE_SET_2:
      kb_send_key_scs2(key, is_key_pressed, is_ctrl);
      break;
    case SCAN_CODE_SET_3:
      kb_send_key_scs3(key, is_key_pressed);
      break;
    default:
      #if DEBUG==1
      printf("INTERNAL ERROR! SCAN CODE SET = %u\n", scancodeset);
      #endif
      break;
  }
  
}

bool kb_task() {
  keyboard_task(&kb_out);
  ps2in_task(&kb_in, &kb_out);
  return kb_enabled;// TODO: return value can probably be void
}

void kb_init(u8 gpio_in) {

  /*инициализация пинов пико для управления 8816 по порядку
  AX0 первый 11 штук */
  for (size_t i = 0; i < 11; i++)
  {
    gpio_init(i+AX0);
    gpio_set_dir(i+AX0, GPIO_OUT);
    gpio_put(i+AX0, 1);
  }
  
  //Инициализация MT8816
  SetAddr(0x00);
  gpio_put(RSTMT,0);
  gpio_put(CSMT,0);
  busy_wait_us(1);
  gpio_put(DATMT,0);
  busy_wait_us(1);
  gpio_put(STBMT,0); //инициализация
  busy_wait_us(1);
  gpio_put(CSMT,1); //выбор чипа
  busy_wait_us(1);
  gpio_put(RSTMT,1);
  busy_wait_us(2);
  gpio_put(RSTMT,0);  //сброс
  busy_wait_us(1);
  //gpio_put(CSMT,0);
  //gpio_put(CSMT, 1); //выбор чипа

  queue_init(&kb_out.qbytes, sizeof(u8), 9);


  ps2in_init(&kb_in, pio1, gpio_in);

  kb_set_defaults();
  //kb_send(KB_MSG_SELFTEST_PASSED_AA);

}
