/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 OneginForte
 *           (c) 2024 No0ne (https://github.com/No0ne)
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
#include "bsp/board_api.h"
#include "hardware/gpio.h"
#include "pico/multicore.h"
#include "hardware/watchdog.h"
#include "pio_usb.h"
#include "hardware/clocks.h"
#include "pico/stdlib.h"
#include "usbfs.h"

// core1: handle host events
void core1_main() {
  sleep_ms(10);
  //tuh_hid_set_default_protocol(HID_PROTOCOL_REPORT);
  //tuh_init(0);
  tud_init(0);
  //pio_usb_host_add_port(21, PIO_USB_PINOUT_DPDM);

  // Use tuh_configure() to pass pio configuration to the host stack
  // Note: tuh_configure() must be called before
  pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
  pio_cfg.pin_dp = 21;
  tuh_configure(1, TUH_CFGID_RPI_PIO_USB_CONFIGURATION, &pio_cfg);

  // To run USB SOF interrupt in core1, init host stack for pio_usb (roothub
  // port1) on core1
  tuh_init(1);
  sleep_ms(10);
 
  usbfs_init();
  
  while (true) {
    tuh_task(); // tinyusb host task
    usbfs_update();
  }
}

int main() {
  set_sys_clock_khz(240000, true);
  board_init();
  stdio_init_all();
  
  printf("\n%s-%s\n", PICO_PROGRAM_NAME, PICO_PROGRAM_VERSION_STRING);
  
   // all USB task run in core1
  multicore_reset_core1(); 
  multicore_launch_core1(core1_main);
 
  gpio_init(LEDPIN);
  gpio_set_dir(LEDPIN, GPIO_OUT);
  
  //nespad_begin(clock_get_hz(clk_sys) / 1000, NES_GPIO_CLK, NES_GPIO_DATA, NES_GPIO_LAT);
  
  //init_74hc595();

  kb_init(KBIN);

  while(1) {
    
    kb_task();
    
  }
}

void reset() {
  printf("\n\n *** PANIC via tinyusb: watchdog reset!\n\n");
  watchdog_enable(100, false);
}
