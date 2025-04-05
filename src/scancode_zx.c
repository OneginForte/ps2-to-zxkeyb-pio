#include "ps2x2pico.h"

uint8_t tab_key[128] = {0x00};// таблица нажатых клавиш

static bool flag_f0, flag_e0;
static uint8_t flag_e1;

bool led_cl=false;

//------------------------------------------------------------------
void SetAddr(uint8_t addr) {
  
    gpio_put(AX0,bitRead(addr,0));
    gpio_put(AX1,bitRead(addr,1));
    gpio_put(AX2,bitRead(addr,2));
    gpio_put(AX3,bitRead(addr,3));
    gpio_put(AY0,bitRead(addr,4));
    gpio_put(AY1,bitRead(addr,5));
    gpio_put(AY2,bitRead(addr,6));
    busy_wait_us(1);
    //      Serial.print("Table: ");
   // Serial.println( addr, BIN );
}

void SetKey(bool data){
   //gpio_put(CSMT, 1); //выбор чипа
   //busy_wait_us(1);
   gpio_put(STBMT, 1); //строб on
   busy_wait_us(2);
   gpio_put(DATMT, data); //данные
   busy_wait_us(2);
   gpio_put(STBMT, 0); //строб off    
   busy_wait_us(2);
   //gpio_put(CSMT, 0);   
}

void key_on(uint8_t code) // клавиша нажата
{
            gpio_put(LEDPIN, 1);//led
            //ws2812_set_rgb(0, 1, 0);
    
            if (flag_e0==true){
                if ((table_key_zx_ps[code][2]==NC)||((tab_key[code]&0xf0)==0xf0)) return;// если на код нет нажатия клавиши ZX или уже нажата
                if (table_key_zx_ps[code][3]!=NC) //если есть клавиша модификатор
                {
                    
                    SetAddr(table_key_zx_ps[code][3]);// адрес  клавиши модификатора zx CS или SS 
                    SetKey(1);//нажатие клавиши
                    
                }
                tab_key[code]=(tab_key[code]&0x0f)|0xf0;
                SetAddr(table_key_zx_ps[code][2]);// адрес клавиши zx 
                SetKey(1);//нажатие клавиши

                printf( "Key E0 ON: 0x%02X 0x%02X 0x%02X 0x%02X \r\n", code, table_key_zx_ps[code][2], table_key_zx_ps[code][3], tab_key[code]);
            }
            else { 
                if ((table_key_zx_ps[code][0]==NC)||((tab_key[code]&0x0f)==0x0f)) return;// если на код нет нажатия клавиши ZX или уже нажата
                if (table_key_zx_ps[code][1]!=NC) //если есть клавиша модификатор
                {
                
                    SetAddr(table_key_zx_ps[code][1]);// адрес  клавиши модификатора zx CS или SS 
                    SetKey(1);//нажатие клавиши
                
                }
                tab_key[code]=(tab_key[code]&0xf0)|0x0f;
                SetAddr(table_key_zx_ps[code][0]);// адрес клавиши zx 
                SetKey(1);//нажатие клавиши

                printf( "Key ON: 0x%02X 0x%02X 0x%02X 0x%02X \r\n", code, table_key_zx_ps[code][0], table_key_zx_ps[code][1], tab_key[code]);
            }
         
}

void key_off(uint8_t code)// клавиша отпущена
{
            gpio_put(LEDPIN, 0);//led
            //ws2812_set_rgb(0, 0, 1); 

            if (flag_e0==true){
                if (table_key_zx_ps[code][2]==NC) return;// если на код нет нажатия клавиши ZX
                if (table_key_zx_ps[code][3]!=NC) //если есть клавиша модификатор
                {
                    
                    SetAddr(table_key_zx_ps[code][3]);// адрес  клавиши модификатора zx CS или SS 
                    SetKey(0);//нажатие клавиши
                    
                }
                tab_key[code]=(tab_key[code]&0x0f);
                SetAddr(table_key_zx_ps[code][2]);// адрес клавиши zx 
                SetKey(0);//нажатие клавиши

                printf( "Key E0 OFF: 0x%02X 0x%02X 0x%02X 0x%02X \r\n", code, table_key_zx_ps[code][2], table_key_zx_ps[code][3], tab_key[code]);
            }
            else { 
                if (table_key_zx_ps[code][0]==NC)return;// если на код нет нажатия клавиши ZX
                if (table_key_zx_ps[code][1]!=NC) //если есть клавиша модификатор
                {
                
                    SetAddr(table_key_zx_ps[code][1]);// адрес  клавиши модификатора zx CS или SS 
                    SetKey(0);//нажатие клавиши
                
                }
                tab_key[code]=(tab_key[code]&0xf0);
                SetAddr(table_key_zx_ps[code][0]);// адрес клавиши zx 
                SetKey(0);//нажатие клавиши

                printf( "Key OFF: 0x%02X 0x%02X 0x%02X 0x%02X \r\n", code, table_key_zx_ps[code][0], table_key_zx_ps[code][1], tab_key[code]);
            }

            
}

// sends out scan codes from a null byte terminated list
void kb_send_macro_list(const u8 *list) {
    key2repeat = 0;
    printf("Send macro");
    for(int i = 0; list[i]; i++) {
      kb_send(list[i]);
    }
  }

//----------------------------------------------------------------------------
void keyboard_task(ps2out* this)
{
    uint8_t i = 0;
    uint8_t byte;
    
  
    if(!queue_is_empty(&this->qbytes)) {
        while(i < 9 && queue_try_remove(&this->qbytes, &byte)) {  //Left from a previous project so as not to forget to make macros

      	uint8_t temp;
	    
		temp = byte;
		if (temp == 0x00) continue;
		switch (temp)
		{
		case 0x00:
			break;

		case 0xF0:
			flag_f0 = true;
			break;

		case 0xE0:
			flag_e0 = true;
			break;

		case 0xE1:
			flag_e1 = 2;
			break;
            //Add send macro void kb_send_sc_list(const u8 *list)
        case 0x05:
            if (flag_f0 == false) kb_send_macro_list(macro1);
                else flag_f0 = false;
			break;
        case 0x06:
            if (flag_f0 == false) kb_send_macro_list(macro2);
                else flag_f0 = false;
			break;
        case 0x04:
            if (flag_f0 == false) kb_send_macro_list(macro3);
                else flag_f0 = false;
			break;
        case 0x0C:
            if (flag_f0 == false) kb_send_macro_list(macro4);
                else flag_f0 = false;
			break;
/*
E 	    24 	    F0,24 	    F1 	    05 	    F0,05
F 	    2B 	    F0,2B 	    F2 	    06 	    F0,06
G 	    34 	    F0,34 	    F3 	    04 	    F0,04
H 	    33 	    F0,33 	    F4 	    0C 	    F0,0C
I 	    43 	    F0,43 	    F5 	    03 	    F0,03
J 	    3B 	    F0,3B 	    F6 	    0B 	    F0,0B
K 	    42 	    F0,42 	    F7 	    83 	    F0,83
L 	    4B 	    F0,4B 	    F8 	    0A 	    F0,0A
M 	    3A 	    F0,3A 	    F9 	    01 	    F0,01
N 	    31 	    F0,31 	    F10 	09 	    F0,09
O 	    44 	    F0,44 	    F11 	78 	    F0,78
P 	    4D 	    F0,4D 	    F12 	07 	    F0,07
*/            
		default:
			if (temp>0x84) break;

            if ((temp==0x58)&&(flag_f0!=true)) /*58 Caps Lock  */
                {
                    if (led_cl) {
                        kb_set_leds(PS2_LED_SCROLL_LOCK);
                    }
                    else {
                        led_cl = !led_cl; // тригер  Caps lock
                        kb_set_leds(PS2_LED_SCROLL_LOCK);
                    }
                
                }
            
            if (flag_f0) {
				key_off(temp);
				flag_f0 = false;
				flag_e0 = false;
				if (flag_e1 != 0) flag_e1--;
				break;
			}
			key_on(temp);
			flag_e0 = false;
			if (flag_e1 != 0) flag_e1--;
			break;
		}
	}

    
    }

}

 //----------------------------------------------------------
