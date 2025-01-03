#include "ps2x2pico.h"

uint8_t mt_matrix[8] = {0x00};// синхронизация матрицы для МТ8816
uint8_t tab_key[128] = {0x00};// таблица нажатых клавиш
uint8_t tab_key_old[128] = {0x00};// таблица предыдушего нажатия клавиш
//static uint8_t ps2bufsize = 0;
uint8_t ps2buffer[KBD_BUFFER_SIZE];

static bool flag_f0, flag_e0;
static uint8_t flag_e1;

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
{NC, NC, NC, NC},  
{NC, NC, NC, NC},/*42 F9 */
{NC, NC, NC, NC},
{NC, NC, NC, NC},/*3E F5*/
{NC, NC, NC, NC},/*3C F3*/
{NC, NC, NC, NC},/*3A F1*/
{NC, NC, NC, NC},/*3B F2*/
{NC, NC, NC, NC},/*45 F12 */
{NC, NC, NC, NC},/*68 F13*/
{NC, NC, NC, NC},/*43 F10 */
{NC, NC, NC, NC},/*41 F8 */
{NC, NC, NC, NC},/*3F F6*/
{NC, NC, NC, NC},/*3D F4*/
{_1, CS, NC, NC},/*2B Tab / Edit CS+1*/
{NC, NC, NC, NC},/*35 Keyboard Grave Accent and Tilde*/  // ` GRAVE
{NC, NC, NC, NC},/*67 Keypad =*/
{NC, NC, NC, NC},/*69 F14*/
{NC, NC, NC, NC},/*Left ALt*/
{CS, NC, NC, NC},/*Left Shift*/
{NC, NC, NC, NC},
{SS, NC, NC, NC},
{_Q, NC, NC, NC},/*14 Q*/
{_1, NC, NC, NC},/*1E 1*/
{NC, NC, NC, NC},
{NC, NC, NC, NC},/*6A F15*/
{NC, NC, NC, NC},
{_Z, NC, NC, NC},/*1D Z*/
{_S, NC, NC, NC},/*16 S*/
{_A, NC, NC, NC},/*04 A*/  // A	
{_W, NC, NC, NC},/*1A W*/
{_2, NC, NC, NC},/*1F 2*/ 
{NC, NC, NC, NC},
{NC, NC, NC, NC},/*6B F16*/
{_C, NC, NC, NC},/*06 C*/
{_X, NC, NC, NC},/*1B X*/
{_D, NC, NC, NC},/*07 D*/
{_E, NC, NC, NC},/*08 E*/
{_4, NC, NC, NC},/*21 4*/
{_3, NC, NC, NC},/*20 3*/
{NC, NC, NC, NC},
{NC, NC, NC, NC},/*6C F17*/
{SP, NC, NC, NC},/*2C Space*/
{_V, NC, NC, NC},/*19 V*/
{_F, NC, NC, NC},/*09 F*/
{_T, NC, NC, NC},/*17 T*/
{_R, NC, NC, NC},/*15 R*/
{_5, NC, NC, NC},/*22 5*/
{NC, NC, NC, NC},/*65*/ //E0 2f APPS	
{NC, NC, NC, NC},/*6D F18*/
{_N, NC, NC, NC},/*11 N*/
{_B, NC, NC, NC},/*05 B*/ 
{_H, NC, NC, NC},/*0B H*/
{_G, NC, NC, NC},/*0A G*/
{_Y, NC, NC, NC},/*1C Y*/
{_6, NC, NC, NC},/*23 6*/
{NC, NC, NC, NC},/*66*/ //POWER	
{NC, NC, NC, NC},/*6E F19*/ 
{NC, NC, NC, NC},
{_M, NC, NC, NC},/*10 M*/
{_J, NC, NC, NC},/*0D J*/
{_U, NC, NC, NC},/*18 U*/
{_7, NC, NC, NC},/*24 7*/
{_8, NC, NC, NC},/*25 8*/
{NC, NC, NC, NC},
{NC, NC, NC, NC},/*6F F20*/ 
{_M, SS, NC, NC},/*36 , SS+M*/ // ,	
{_K, NC, NC, NC},/*0E K*/
{_I, NC, NC, NC},/*0C I*/
{_O, NC, NC, NC},/*12 O*/
{_0, NC, NC, NC},/*27 0*/
{_9, NC, NC, NC},/*26 9*/
{NC, NC, NC, NC},
{SS, CS, NC, NC},/*70 F21 ALT   Ext.Mode SS+CS  */
{_N, SS, NC, NC},/*37 . SS+N*/ // .
{_C, SS, _V, SS},/*38 ? SS+C*/ // /	/*54 Keypad / SS+V*/
{_L, NC, NC, NC},/*0F L*/
{_O, SS, NC, NC},/*33 ; SS+O*/ // ;
{_P, NC, NC, NC},/*13 P*/
{_J, SS, NC, NC},/*2D -SS+J*/
{NC, NC, NC, NC},
{CS, NC, NC, NC},/*71 F22 CTRL  / CS ZX  */ 
{NC, NC, NC, NC},
{_P, SS, NC, NC},/*34 SS+P  "*/ // ' APOSTROPHE
{NC, NC, NC, NC},
{NC, NC, NC, NC},/*2F [*/ 
{_K, SS, NC, NC},/*2E +      SS+K*/
{NC, NC, NC, NC},
{SS, NC, NC, NC},/*72 F23 SHIFT / SS ZX  */
{_2, CS, NC, NC},/*39 Caps Lock  CS+2*/ // CAPS
{CS, NC, NC, NC},/*Casp Shift*/
{EN, NC, EN, NC},/*28 Enter*/      //E0 5a,EN, NC,/*58 Keypad Enter */
{_L, SS, NC, NC},/*30 SS+L*/ // ]	
{NC, NC, NC, NC},
{NC, NC, NC, NC},/*31 \*/ // BACKSLASH	5d,NC, NC,/*32 `*/ // EUROPE_1
{NC, NC, NC, NC},
{NC, NC, NC, NC},/*73 F24 WIN */
{NC, NC, NC, NC},
{NC, NC, NC, NC},/*64*/
{NC, NC, NC, NC},
{NC, NC, NC, NC},
{NC, NC, NC, NC},
{NC, NC, NC, NC},
{_0, CS, NC, NC},/*2A Keyboard Delete (Backspace)  CS+0*/
{NC, NC, NC, NC},
{NC, NC, NC, NC},
{_1, NC, NC, NC},/*59 Keypad 1 */   //E0 69,NC, NC,/*4D End */
{NC, NC, NC, NC},
{_4, NC, _5, CS},/*5C Keypad 4 */   //E0 6b,_5, CS,/*50 LeftArrow CS+5*
{_7, NC, NC, NC},/*5F Keypad 7 */  //E0 6c,NC, NC,/*4A Home */
{NC, NC, NC, NC},
{NC, NC, NC, NC},
{NC, NC, NC, NC},
{_0, NC, NC, NC},/*62 Keypad 0 */  //E0 70,NC, NC,/*49 Insert */
{NC, NC, NC, NC},/*63 Keypad Del.*///E0 71,NC, NC,/*4C Del */
{_2, NC, _6, CS},/*5A Keypad 2 */          //E0 72,_6, CS,/*51 DownArrow  CS+6*/
{_5, NC, NC, NC},/*5D Keypad 5 */  
{_6, NC, _8, CS},/*5E Keypad 6 */  //E0 74,_8, CS,/*4F RightArrow CS+8*/
{_8, NC, _7, CS},/*60 Keypad 8 */  //E0 75,_7, CS,/*52 UpArrow CS+7*/
{SP, CS, NC, NC},/*29 Escape/ Break CS+Space*/
{NC, NC, NC, NC},/*53 Keypad Num Lock and Clear*/
{NC, NC, NC, NC},/*44 F11 */
{_K, SS, NC, NC},/*57 Keypad + */ 
{_3, NC, _4, CS},/*5B Keypad 3 */  //E0 7a,_4, CS,/*4E PageDown CS+4 Inv Video*/
{_I, SS, NC, NC},/*56 Keypad - */
{NC, NC ,_9, CS},/*55 Keypad * */   //E0 7c,_9, CS,/*46 PrintScreen Graph Mode CS+9*/
{_9, NC, _3, CS},/*61 Keypad 9 */   //E0 7D,_3, CS,/*4B PageUp CS+3 True Video*/    
{NC, NC, NC, NC},/*47 Scroll Lock*/        //E1 14 77 E1 F0 14 F0 77 7e,NC, NC,/*48 Pause */
{NC, NC, NC, NC},
{NC, NC, NC, NC},
{NC, NC, NC, NC},
{NC, NC, NC, NC},
{NC, NC, NC, NC},/*40 F7 */
{NC, NC, NC, NC}
};
// 0x10 - 0x1f			  
// 0x20 - 0x2f			
// 0x30 - 0x3f			
// 0x40 - 0x4f			
// 0x50 - 0x5f			  
// 0x60 - 0x6f			
// 0x70 - 0x73			/* клавиши ALT CTRL SHIFT  специально для ZX  */


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
bool led_cl=true;


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
                printf( "Key E0 ON: 0x%02X  0x%02X 0x%02X 0x%02X \r\n", code, table_key_zx_ps[code][2], table_key_zx_ps[code][3], tab_key[code]);
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
            printf( "Key ON: 0x%02X  0x%02X 0x%02X 0x%02X \r\n", code, table_key_zx_ps[code][0], table_key_zx_ps[code][1], tab_key[code]);
            }

              
            
              
                          
             
}

void key_off(uint8_t code)// клавиша отпущена
{
            //kb_set_leds(0);// 0. Num lock 1.  Caps lock 2.  Scroll
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
                printf( "Key E0 OFF: 0x%02X  0x%02X 0x%02X 0x%02X \r\n", code, table_key_zx_ps[code][2], table_key_zx_ps[code][3], tab_key[code]);
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
            printf( "Key OFF: 0x%02X  0x%02X 0x%02X 0x%02X \r\n", code, table_key_zx_ps[code][0], table_key_zx_ps[code][1], tab_key[code]);
            }

            
}

void keyboard(uint8_t const *report, uint16_t len)
{

    printf("0x%d 0x%d 0x%d 0x%d 0x%d 0x%d 0x%d\r\n", report[0], report[1], report[2], report[3], report[4], report[5], report[6]);
    //printf("Modifiers %d, len %d \n", modifiers, len  );
    //debug_print("0x%02X 0x%02X 0x%02X 0x%02X  0x%02X 0x%02X 0x%02X\r\n", report[0] , report[2], report[3], report[4], report[5], report[6], report[7]);

    scancode_s(report[0]); // запись в таблицу 0 кода alt ctrl shift win
    // tab_key[report[1]]=1;// запись в таблицу 1 кода reserved
    tab_key[report[2]] = 1; // запись в таблицу 2 кода
    tab_key[report[3]] = 1; // запись в таблицу 3 кода
    tab_key[report[4]] = 1; // запись в таблицу 4 кода
    tab_key[report[5]] = 1; // запись в таблицу 5 кода
    tab_key[report[6]] = 1; // запись в таблицу 6 кода
    tab_key[report[7]] = 1; // запись в таблицу 7 кода

    for (uint8_t i = 0; i < 127; i++)
    {
        uint8_t d = (tab_key[i] << 1) | (tab_key_old[i]); // 0b000000x0 |0b0000000y
        // if (d==0) // не нажато уже выключать не нужно
        if (d == 2)
            key_on(i); // нажато сейчас
        // if (d==3) // нажато уже включать не нужно
        if (d == 1)
            key_off(i);              // клавиша отпущена  сейчас
        tab_key_old[i] = tab_key[i]; // копирование таблицы
        tab_key[i] = 0;              // стирание таблицы
    }
}


//----------------------------------------------------------------------------
void keyboard_task(ps2out* this)
{
    uint8_t i = 0;
    uint8_t byte;
    
  
    if(!queue_is_empty(&this->qbytes)) {
        while(i < 9 && queue_try_remove(&this->qbytes, &byte)) {

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

		default:
			if (temp>0x84) break;
       
            //code = code & 0x7f;//???
            
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

    
    //scancode_s(pack[0]); // запись в таблицу 0 кода alt ctrl shift win
    // tab_key[report[1]]=1;// запись в таблицу 1 кода reserved
    //tab_key[pack[0]] = 1; // запись в таблицу 2 кода
/*
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
}

 //----------------------------------------------------------
