## Конвертор клавиатуры из usb и ps/2
## Контроллер ZX Spectrum клавиатуры на ключах MT8816

На основе проектов 🔗: 
- [No0ne](https://github.com/No0ne/ps2x2pico)
- [billgilbert7000](https://github.com/billgilbert7000/usb_to_zx_spectrum)
- [MikhaelKaa](https://github.com/MikhaelKaa/usb2spectrum)

### Для корректной работы TinyUSB необходим [патч](https://github.com/OneginForte/ps2-to-zxkeyb-pio/blob/main/tinyusb-0.17.0-dualusb.patch) из корня. 

### Используемые выводы PICO 

Для работы с MT8816:
- AX0 `GPIO4`
- AX1 `GPIO5`
- AX2 `GPIO6`
- AX3 `GPIO7`
- AY0 `GPIO8`
- AY1 `GPIO9`
- AY2 `GPIO10`
- STBMT `GPIO11`
- CSMT  `GPIO12`
- RSTMT `GPIO13`
- DATMT `GPIO14`

Для интерфейса PS/2: 
-  PS/2 DATA `GPIO2` 
-  CLOCK  `GPIO3`

Для порта PIO-USB: 
- D+ `GPIO0`` 
- D- `GPIO1``

Переназначение выводов порта DEBUG: 
- UART_TX_PIN `GPIO16` 
- UART_RX_PIN `GPIO17`


Соединение выводов MT8816 для Пентагон 128

На порту Y выводы с Dx соединяются 1 к 1, как на изображении. 
Ремап сделан программно.

![Connection](https://github.com/OneginForte/ps2-to-zxkeyb-pio/blob/main/doc/conn.PNG)  
