#include "ps2x2pico.h"

uint8_t mt_matrix[8] = {0x00};// синхронизация матрицы для МТ8816
uint8_t tab_key[128] = {0x00};// таблица нажатых клавиш
uint8_t tab_key_old[128] = {0x00};// таблица предыдушего нажатия клавиш
//static uint8_t ps2bufsize = 0;
uint8_t ps2buffer[KBD_BUFFER_SIZE];

static bool flag_f0, flag_e0;
static uint8_t flag_e1;

uint8_t const macro1[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 }; //*"A : "
uint8_t const macro2[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 }; //RANDOMIZE USR 15616
uint8_t const macro3[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 }; //PageUP
uint8_t const macro4[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 }; //PageDown
uint8_t const macro5[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 }; //RUN "BOOT" 
uint8_t const macro6[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 }; //RUN ""
uint8_t const macro7[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 };
uint8_t const macro8[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 };
uint8_t const macro9[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 };
uint8_t const macro10[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 };
uint8_t const macro11[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 };
uint8_t const macro12[]  = { 0xe0, 0x2a, 0xe0, 0x37, 0 };


/*
KEY 	MAKE 	BREAK 	    KEY 	MAKE 	BREAK
A 	    1C 	    F0,1C 	    R ALT 	E0,11 	E0,F0,11
B 	    32 	    F0,32 	    APPS 	E0,2F 	E0,F0,2F
C 	    21 	    F0,21 	    ENTER 	5A 	    F0,5A
D 	    23 	    0,23 	    ESC 	76 	    F0,76
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
Q 	    15 	    F0,15 	    SCROLL	7E 	    F0,7E
R 	    2D 	    F0,2D 	    [ 	    54 	    FO,54
S 	    1B 	    F0,1B 	    INSERT	E0,70 	E0,F0,70
T 	    2C 	    F0,2C 	    HOME 	E0,6C 	E0,F0,6C
U 	    3C 	    F0,3C 	    PG UP 	E0,7D 	E0,F0,7D
V 	    2A 	    F0,2A 	    DELETE	E0,71 	E0,F0,71
W 	    1D 	    F0,1D 	    END 	E0,69 	E0,F0,69
X 	    22 	    F0,22 	    PG DN 	E0,7A 	E0,F0,7A
Y 	    35 	    F0,35 	    UP 	    E0,75 	E0,F0,75
Z 	    1A 	    F0,1A 	    LEFT 	E0,6B 	E0,F0,6B
0 	    45 	    F0,45 	    DOWN 	E0,72 	E0,F0,72
1 	    16 	    F0,16 	    RIGHT 	E0,74 	E0,F0,74
2 	    1E 	    F0,1E 	    NUM 	77 	    F0,77
3 	    26 	    F0,26 	    KP / 	E0,4A 	E0,F0,4A
4 	    25 	    F0,25 	    KP * 	7C 	    F0,7C
5 	    2E 	    F0,2E 	    KP - 	7B 	    F0,7B
6 	    36 	    F0,36 	    KP + 	79 	    F0,79
7 	    3D 	    F0,3D 	    KP EN 	E0,5A 	E0,F0,5A
8 	    3E 	    F0,3E 	    KP . 	71 	    F0,71
9 	    46 	    F0,46 	    KP 0 	70 	    F0,70
` 	    0E 	    F0,0E 	    KP 1 	69 	    F0,69
- 	    4E 	    F0,4E 	    KP 2 	72 	    F0,72
= 	    55 	    FO,55 	    KP 3 	7A 	    F0,7A
\ 	    5D 	    F0,5D 	    KP 4 	6B 	    F0,6B
BKSP 	66 	    F0,66 	    KP 5 	73 	    F0,73
SPACE 	29 	    F0,29 	    KP 6 	74 	    F0,74
TAB 	0D 	    F0,0D 	    KP 7 	6C  	F0,6C
CAPS 	58 	    F0,58 	    KP 8 	75 	    F0,75
L SHFT	12 	    FO,12 	    KP 9 	7D 	    F0,7D
L CTRL	14 	    FO,14 	    ] 	    5B 	    F0,5B
L GUI 	E0,1F 	E0,F0,1F	; 	    4C 	    F0,4C
L ALT 	11 	    F0,11 	    ' 	    52 	    F0,52
R SHFT	59 	    F0,59 	    , 	    41 	    F0,41
R CTRL	E0,14 	E0,F0,14	. 	    49 	    F0,49
R GUI 	E0,27 	E0,F0,27	/ 	    4A 	    F0,4A

*/

/*
Left/Right Shift 	        N/A 	                                                CAPS SHIFT 	Modifies the other keys you’ll press with it
Left/Right CTRL 	        N/A 	                                                SYMBOL SHIFT 	Modifies the other keys you’ll press with it
Left/Right ALT  	        Green cursor in NextBASIC/E cursor on 48K BASICs 	    EXTEND MODE 	Presses SYMBOL SHIFT and CAPS SHIFT together
Cursor Keys 	            Cursor will move 	                                    CAPS SHIFT + / 5,6,7,8 	Emulates the ZX Spectrum + cursor keys
PgUp 	                    Reset Inverse Video for 48K / See Chp. 1 for NextBASIC 	CAPS SHIFT + 3 	Emulates the ZX Spectrum + TRUE VIDEO key
PgDn 	                    Sets Inverse Video for 48K / See Chp. 1 for NextBASIC 	CAPS SHIFT + 4 	Emulates the ZX Spectrum + INVERSE VIDEO key
ESC 	                    Either Space or BREAK INTO PROGRAM if running a Next/Sinclair BASIC program 	CAPS SHIFT + SPACE 	Breaks the execution of a program
Left Windows Key / Home 	{ on NextBASIC / TO on 48K BASIC 	                    SYMBOL SHIFT + F 	Prints a symbol / token
Right Windows Key / End 	} on NextBASIC / THEN on 48K BASIC 	                    SYMBOL SHIFT + G 	Prints a symbol / token
Scroll Lock 	            Magenta Cursor on NextBASIC / G cursor on 48K Basic 	CAPS SHIFT + 9 / GRAPH key on +/128 	Sets / resets GRAPHICS mode
PrtScr 	                    (c) symbol 	                                            EXTEND MODE + P 	Prints a symbol
‘,” key 	                Double Quotes ‘”‘ 	                                    SYMBOL SHIFT + P 	Emulates the ZX Spectrum + ” key
, key 	                    Comma “,” 	                                            SYMBOL SHIFT + N 	Emulates the ZX Spectrum + , key
. key 	                    Period “.” 	                                            SYMBOL SHIFT + M 	Emulates the ZX Spectrum + . key
;,: key 	                Semicolon “;” 	                                        SYMBOL SHIFT + O 	Emulates the ZX Spectrum + ; key
[,{ key 	                [ 	                                                    SYMBOL SHIFT + Y 	Prints a symbol
],} key 	                ] 	                                                    SYMBOL SHIFT + U 	Prints a symbol
/,? key 	                Forward slash “/” 	                                    SYMBOL SHIFT + V 	Prints a symbol
\,| key 	                Backward slash “\” on NextBASIC / STEP on 48K BASIC 	SYMBOL SHIFT + D 	Prints a symbol / token
=,+ key 	                Equals sign “=” 	                                    SYMBOL SHIFT + L 	Prints a symbol
-,_ key 	                Minus sign “-“ 	                                        SYMBOL SHIFT + J 	Prints a symbol
`,~ key 	                Single quote “‘” 	                                    SYMBOL SHIFT + 7 	Prints a symbol
Ins 	                    Tilde “~” in NextBASIC / STOP in 48K BASIC 	            SYMBOL SHIFT + A 	Prints a symbol / token
Del 	                    Question mark “?” 	                                    SYMBOL SHIFT + C 	Prints a symbol
Numeric Keypad keys 	    Number on keyboard 	                                    0…9 	            Prints a number
Numeric “/” 	            Forward slash “/” 	                                    SYMBOL SHIFT + V 	Prints a symbol
Numeric “*” 	            Asterisk “*” 	                                        SYMBOL SHIFT + B 	Prints a symbol
Numeric “-“ 	            Minus “-“ 	                                            SYMBOL SHIFT + J 	Prints a symbol
Numeric “+” 	            Plus “+” 	                                            SYMBOL SHIFT + K 	Prints a symbol
Numeric “.” 	            Period “.” 	                                            SYMBOL SHIFT + M 	Prints a symbol
Numeric “Enter” 	        ENTER 	                                                ENTER 	Presses Enter
ENTER 	                    ENTER 	                                                ENTER 	Presses Enter
A…Z keys 	                A…Z 	A…Z 	Prints character

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
{NC, NC, NC, NC},/*(03) F5*/
{_3, CS, NC, NC},/*(04) F3*/
{NC, NC, NC, NC},/*(05) F1*/
{NC, NC, NC, NC},/*(06) F2*/
{NC, NC, NC, NC},/*(07) F12 */
{NC, NC, NC, NC},/*(08) F13*/
{NC, NC, NC, NC},/*(09) F10 */
{NC, NC, NC, NC},/*(0A) F8 */
{NC, NC, NC, NC},/*(0B) F6*/
{_4, CS, NC, NC},/*(0C) F4*/
{SS, CS, NC, NC},/*(0D) Tab CS+SS*/
{_1, CS, NC, NC},/*(0E) Keyboard Grave Accent and Tilde  / Edit CS+1 */  // ` GRAVE
{NC, NC, NC, NC},/*(0F) Keypad =*/
{NC, NC, NC, NC},/*(10) F14*/
{NC, NC, NC, NC},/*(11) Left ALt*/
{CS, NC, NC, NC},/*(12) Left Shift*/
{NC, NC, NC, NC},/*(13)*/
{SS, NC, NC, NC},/*(14)*/
{_Q, NC, NC, NC},/*(15) Q*/
{_1, NC, NC, NC},/*(16) 1*/
{NC, NC, NC, NC},/*(17)*/
{NC, NC, NC, NC},/*(18) F15*/
{NC, NC, NC, NC},/*(19)*/
{_Z, NC, NC, NC},/*(1A) Z*/
{_S, NC, NC, NC},/*(1B) S*/
{_A, NC, NC, NC},/*(1C) A*/ 	
{_W, NC, NC, NC},/*(1D) W*/
{_2, NC, NC, NC},/*(1E) 2*/ 
{NC, NC, NC, NC},/*(1F)*/
{NC, NC, NC, NC},/*(20) F16*/
{_C, NC, NC, NC},/*(21) C*/
{_X, NC, NC, NC},/*(22) X*/
{_D, NC, NC, NC},/*(23) D*/
{_E, NC, NC, NC},/*(24) E*/
{_4, NC, NC, NC},/*(25) 4*/
{_3, NC, NC, NC},/*(26) 3*/
{NC, NC, NC, NC},/*(27)*/
{NC, NC, NC, NC},/*(28) F17*/
{SP, NC, NC, NC},/*(29) Space*/
{_V, NC, NC, NC},/*(2A) V*/
{_F, NC, NC, NC},/*(2B) F*/
{_T, NC, NC, NC},/*(2C) T*/
{_R, NC, NC, NC},/*(2D) R*/
{_5, NC, NC, NC},/*(2E) 5*/
{NC, NC, NC, NC},/*(2F)*/ //E0 2f APPS	
{NC, NC, NC, NC},/*(30) F18*/
{_N, NC, NC, NC},/*(31) N*/
{_B, NC, NC, NC},/*(32) B*/ 
{_H, NC, NC, NC},/*(33) H*/
{_G, NC, NC, NC},/*(34) G*/
{_Y, NC, NC, NC},/*(35) Y*/
{_6, NC, NC, NC},/*(36) 6*/
{NC, NC, NC, NC},/*(37)*/ //POWER	
{NC, NC, NC, NC},/*(38) F19*/ 
{NC, NC, NC, NC},/*(39)*/
{_M, NC, NC, NC},/*(3A) M*/
{_J, NC, NC, NC},/*(3B) J*/
{_U, NC, NC, NC},/*(3C) U*/
{_7, NC, NC, NC},/*(3D) 7*/
{_8, NC, NC, NC},/*(3E) 8*/
{NC, NC, NC, NC},/*(3F)*/
{NC, NC, NC, NC},/*(40) F20*/ 
{_N, SS, NC, NC},/*(41) . SS+N   , SS+M*/ // .	
{_K, NC, NC, NC},/*(42) K*/
{_I, NC, NC, NC},/*(43) I*/
{_O, NC, NC, NC},/*(44) O*/
{_0, NC, NC, NC},/*(45) 0*/
{_9, NC, NC, NC},/*(46) 9*/
{NC, NC, NC, NC},/*(47)*/
{SS, CS, NC, NC},/*(48) F21 ALT   Ext.Mode SS+CS  */
{_M, SS, NC, NC},/*(49) , SS+M  . SS+N*/ // ,
{_C, SS, _V, SS},/*(4A) ? SS+C*/ // /	/*54 Keypad / SS+V*/
{_L, NC, NC, NC},/*(4B) L*/
{_Z, SS, NC, NC},/*(4С) : SS+Z ; SS+O*/ // :
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
{EN, NC, EN, NC},/*(5A) Enter*/      //E0 5a,EN, NC,/*58 Keypad Enter */
{_L, SS, NC, NC},/*(5B) SS+L*/ // ]	
{NC, NC, NC, NC},/*(5C)*/
{NC, NC, NC, NC},/*(5D) \*/ // BACKSLASH	5d,NC, NC,/*32 `*/ // EUROPE_1
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
{_1, NC, NC, NC},/*(69) Keypad 1 */   //E0 69,NC, NC,/*4D End */
{NC, NC, NC, NC},/*(6A)*/
{_4, NC, _5, CS},/*(6B) Keypad 4 */   //E0 6b,_5, CS,/*50 LeftArrow CS+5*
{_7, NC, NC, NC},/*(6C) Keypad 7 */   //E0 6c,NC, NC,/*4A Home */
{NC, NC, NC, NC},/*(6D)*/
{NC, NC, NC, NC},/*(6E)*/
{NC, NC, NC, NC},/*(6F)*/
{_0, NC, NC, NC},/*(70) Keypad 0 */   //E0 70,NC, NC,/*49 Insert */
{NC, NC, NC, NC},/*(71) Keypad Del.*/ //E0 71,NC, NC,/*4C Del */
{_2, NC, _6, CS},/*(72) Keypad 2 */   //E0 72,_6, CS,/*51 DownArrow  CS+6*/
{_5, NC, NC, NC},/*(73) Keypad 5 */  
{_6, NC, _8, CS},/*(74) Keypad 6 */   //E0 74,_8, CS,/*4F RightArrow CS+8*/
{_8, NC, _7, CS},/*(75) Keypad 8 */   //E0 75,_7, CS,/*52 UpArrow CS+7*/
{SP, CS, NC, NC},/*(76) Escape/ Break CS+Space*/
{NC, NC, NC, NC},/*(77) Keypad Num Lock and Clear*/
{NC, NC, NC, NC},/*(78) F11 */
{_K, SS, NC, NC},/*(79) Keypad + */ 
{_3, NC, _4, CS},/*(7A) Keypad 3 */   //E0 7a,_4, CS,/*4E PageDown CS+4 Inv Video*/
{_I, SS, NC, NC},/*(7B) Keypad - */
{NC, NC ,_9, CS},/*(7C) Keypad * */   //E0 7c,_9, CS,/*46 PrintScreen Graph Mode CS+9*/
{_9, NC, _3, CS},/*(7D) Keypad 9 */   //E0 7D,_3, CS,/*4B PageUp CS+3 True Video*/    
{NC, NC, NC, NC},/*(7E) Scroll Lock*/ //E1 14 77 E1 F0 14 F0 77 7e,NC, NC,/*48 Pause */
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
            kb_send_sc_list(const u8 *macro1);
			break;
        case 0x06:
            kb_send_sc_list(const u8 *macro2);
			break;
        case 0x04:
            kb_send_sc_list(const u8 *macro3);
			break;
        case 0x0C:
            kb_send_sc_list(const u8 *macro4);
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
