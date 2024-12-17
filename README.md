## Конвертор клавиатуры из usb и ps/2
## Контроллер ZX Spectrum клавиатуры на ключах MT8816

На основе проектов 🔗: 
- [No0ne](https://github.com/No0ne/ps2x2pico)
- [billgilbert7000](https://github.com/billgilbert7000/usb_to_zx_spectrum)
- [MikhaelKaa](https://github.com/MikhaelKaa/usb2spectrum)

### Для корректной работы TinyUSB необходим [патч](https://github.com/OneginForte/ps2-to-zxkeyb-pio/blob/main/tinyusb-0.17.0-dualusb.patch) из корня. 

### Выводы PICO для работы с MT8816:

- `AX0 4`
- `AX1 5`
- `AX2 6`
- `AX3 7`
- `AY0 8`
- `AY1 9`
- `AY2 10`
- `STBMT 11`
- `CSMT  12`
- `DATMT 14`

Выводы порта: 
-  PS/2 DATA `GPIO3` 
-  CLOCK - `GPIO4`

Выводы порта PIO-USB: 
- D+ `GPIO21` 
- D- `GPIO22`

Переназначение выводов порта DEBUG: 
- UART_TX_PIN `GPIO16` 
- UART_RX_PIN `GPIO17`


