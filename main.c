
#include <avr/io.h>

// Define
#define F_CPU 8000000UL
#define MEASURE_MODE 1#define PAUSE_MODE 0volatile uint8_t system_mode = MEASURE_MODE;
unsigned char push_button;		// push_button lưu số thứ tự phím được ấn
unsigned int led_shift =254;		// Biến điều khiển Led_shift

/* delay per digit ~40us, clear/home ~1.5-1.6ms -> 4bit thì delay x2 do Một kí tự = 2 lần ghi nibble*/

// Khai báo file thư viện bổ sung
#include <util/delay.h>
#include "hunget_adc.h"#include "hunget_lcd.h"
#include "Lib.h"

int main(void)
{
	INIT();
	_delay_ms(500);

	// Hiện thị màn hình khởi tạo
	LCD4_CUR_GOTO(1,0);	LCD4_OUT_STR("** MEASURING...");
	_delay_us(1200);
	LCD4_CUR_GOTO(2,0);	LCD4_OUT_STR("Temp: 000.00*C");
	_delay_us(960);
	while(1)
	{
		if (system_mode == MEASURE_MODE)	// đo nhiệt độ
		{
			ADC_2_LCD();
			push_button = PB_CHECK();		// 
			if (push_button == 4) 
			{
				system_mode = PAUSE_MODE;
				_delay_ms(500);
			}
			LED_SHIFT_CTRL();
		}
		else if(system_mode == PAUSE_MODE)		// dừng đo
		{
			SETTING_MODE();
			LCD4_CUR_GOTO(1,0);			LCD4_OUT_STR("** MEASURING...");
			_delay_us(1200);
			LCD4_CUR_GOTO(2,0);			LCD4_OUT_STR("Temp: 000.00*C");
			_delay_us(960);
		}
	}
	return 0;
}

