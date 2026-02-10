/*************************************************************************************
                      TÓM TẮT CHỨC NĂNG CÁC HÀM

*** Hàm khởi tạo ***

	void INIT();	- Khởi tạo trạng thái ban đầu cho các chân I/O và hệ thống
*** Các hàm điều khiển chính ***	void LED7_CTRL(unsigned char num);	- Hàm điều khiển LED 7 thanh.	- Mỗi khi người dùng bấm một phím ‘num’, LED 7 thanh hiện thị giá trị số thứ tự phím bấm đấy.		void LED_SHIFT_CTRL();	- Hàm điểu khiển dãy LED đơn.
	- Mỗi khi chương trình đo nhiệt độ, các LED sẽ sáng lên theo giá trị nhị phân.
	int PB_CTRL(int num);	- Dùng để tăng giảm giá trị 1 số khi bấm PB1:0	- DÙng để tạm dừng chương trình đo.*** Các hàm phụ ***	void TEMP_CHECK(unsigned int temperature);
	- Kiểm tra nhiệt độ hiện tại với nhiệt độ tham chiếu
	- Nếu nhiệt độ nằm ngoài phạm vi cho phép LCD thông báo và Buzzer bật		unsigned char PB_CHECK();	- Hàm trả về giá trị phím vừa bấm (phím 4:1 nối với chân PB3:0)	+ Khi nhấn, phím sẽ kết nối chân tương ứng của VĐK tới GND (mức logic 0)	+ Khi nhả phím, chân tương ứng của VĐK được treo lên mức logic 1 nhờ các trở kéo có sẵn*************************************************************************************//************************************************************************************/
// DEFINE#define BUZZ_ON()	PORTA |= (1<<PA4)		// Bật còi#define BUZZ_OFF()	PORTA &= ~(1<<PA4)		// Tắt còifloat ALERT_LEVEL[2] = {100,0};/*************************************************************************************/

// Function prototype

void INIT();void LED7_CTRL(unsigned char num);
void LED_SHIFT_CTRL();
void TEMP_CHECK(unsigned int temperature);
unsigned char PB_CHECK();
float PB_CTRL(float num);
void SETTING_MODE();
void ADC_2_LCD();

/*************************************************************************************/// FUNCTION	// 	Trong mạch Kit này, trạng thái logic làm các đèn
	// 	LED sáng/tắt theo quy tắc: 0 – LED sáng, 1 – LED tắt.

/*************************************************************************************/void SETTING_MODE(){	// Tạm thời tắt ADC	ADCSRA &= ~(1 << ADEN);		int n = (int)ALERT_LEVEL[0];	int n2= (int)((ALERT_LEVEL[0]-n)*100);	// Khởi tạo màn hình cài đặt ban đầu
	LCD4_CUR_GOTO(1,0);	LCD4_OUT_STR(">Max Temp: ");	_delay_us(880);	LCD4_OUT_DEC(n,3);	LCD4_OUT_STR(".");	LCD4_OUT_DEC(n2,2);	_delay_us(880);	LCD4_OUT_STR("*C");	_delay_us(160);	n = (int)ALERT_LEVEL[1];	n2= (int)((ALERT_LEVEL[1]-n)*100);	LCD4_CUR_GOTO(2,0);	LCD4_OUT_STR(">Min Temp: ");	_delay_us(880);	LCD4_OUT_DEC(n,3);	LCD4_OUT_STR(".");	LCD4_OUT_DEC(n2,2);	_delay_us(240);	LCD4_OUT_STR("*C");	_delay_us(160);	// Bắt đầu điều chỉnh cài đặt	while(system_mode == PAUSE_MODE)	{		unsigned int push_button = PB_CHECK();		if (push_button == 4)		{			LCD4_CLR();			_delay_ms(5);			ADCSRA |= (1<<ADEN);	// Bật lại ADC
			system_mode = MEASURE_MODE;	// Bật chế độ đo
			_delay_ms(500); break;		}		else if (push_button == 1 || push_button == 2)
		{
			int row = push_button -1;
			LCD4_OUT_CMD(0x0D);		// chế độ con trỏ nháy
			_delay_ms(5);
			int result=0;
			while(1)
			{
				result = PB_CTRL(ALERT_LEVEL[row]);
				if (result == -1)		//PB4
				{
					LCD4_OUT_CMD(0x0C);		// tắt con trỏ
					_delay_ms(5);
					break;
				}
				else
				{
					ALERT_LEVEL[row]=result;
					LCD4_CUR_GOTO(push_button,11);
					LCD4_OUT_DEC(ALERT_LEVEL[row],3);
					_delay_us(240);
				}
			}
		}	}}void ADC_2_LCD(){	ADC_STA_CONVERT();	// Bắt đầu nhận adc		int temp = (ADC/4);	int temp2 = (ADC % 4) * 25;	// In nhiệt độ đo được ra màn hình	LCD4_CUR_GOTO(2,6);	LCD4_OUT_DEC(temp,3);
	LCD4_OUT_STR(".");
	LCD4_OUT_DEC(temp2,2);
	_delay_ms(1000);

	TEMP_CHECK(temp);		// Kiểm tra nhiệt độ với tham chiếu
}float PB_CTRL(float num){	unsigned int push_button = PB_CHECK();		// Lưu kết quả phím ấn vào biến push_button
	switch(push_button)	{		// 3 là số lượng cài đặt mức 0 trong danh sách		case 1: 		{			num +=0.25;		// Tăng giá trị chính 0.25 đơn vị			if (num > 100) num = 100; break;		}		case 2:		{			num -=0.25;		// Giảm giá trị chính 0.25 đơn vị			if (--num < 0) num = 0; break;				}		case 3: break;
		case 4: num = -1; break;
	}
	_delay_ms(50);
	return num;
}unsigned char PB_CHECK(){	// Kiểm tra trạng thái logic của 4 chân PB3:0.	if((PINB & 0x0F) != 0x0F)	// Nếu PB3:0 khác 1111	{		if(!(PINB & (1<<PB0)))	// Nếu PB0 ở mức !=1 (tức =0)			{				LED7_CTRL(1);				return 1;			}		if(!(PINB & (1<<PB1)))	// Nếu PB1 ở mức !=1 (tức =0)			{				LED7_CTRL(2);				return 2;			}		if(!(PINB & (1<<PB2)))	// Nếu PB2 ở mức !=1 (tức =0)			{				LED7_CTRL(3);				return 3;			}		if(!(PINB & (1<<PB3)))	// Nếu PB3 ở mức !=1 (tức =0)			{				LED7_CTRL(4);				return 4;			}	}	// Nếu không phím nào được ấn, hàm kết thúc và trả về 0	LED7_CTRL(0);	 _delay_us(50);	return 0;
}void TEMP_CHECK(unsigned int temperature)
{
	if (temperature > ALERT_LEVEL[0])
	{		LCD4_CUR_GOTO(1,0);		LCD4_OUT_STR("!! HIGH TEMP...");
		_delay_ms(200);
		BUZZ_ON();
	}
	else if (temperature < ALERT_LEVEL[1])	{		LCD4_CUR_GOTO(1,0);		LCD4_OUT_STR("!! LOW TEMP....");		_delay_ms(200);		BUZZ_ON();	}	else	{		LCD4_CUR_GOTO(1,0);		LCD4_OUT_STR("** MEASURING...");		_delay_ms(200);		BUZZ_OFF();	}
	_delay_ms(300);
}void LED_SHIFT_CTRL(){	if(led_shift < 0)			// Nếu led_shift đã bật hết trước đó		led_shift = 255;		// bật toàn bộ led_shift		PORTD = led_shift--;		// Điều khiển các LED đơn qua biến led_shift
}
void LED7_CTRL(unsigned char num){	// Khai báo biến temp lưu trạng thái của PORTC	unsigned char temp = PORTC;	// Các chân vi điều khiển ứng với các thanh LED	// a - PC5			PC5	// b - PC4		PC6		PC4	// c - PC2		PC6		PC4	// d - PC1			PC7	// e - PC0		PC0		PC2	// f - PC6		PC0		PC2	// g - PC7			PC1		PC3	// dot - PC3	temp &= 0B00001000;		// Tất cả các đoạn LED sáng (dot giữ nguyên trạng thái trước đó)	/*****	>>> a&b = 1 <=> a=b=1	(P/s: Phép & thực hiện từng bit kp cả số)	>>> If PC3=1 -> kq PC3 =1 , if PC3=0 -> kq PC3=0		>>> Các bit khác & với 0 nên kq đều = 0	*****/	switch(num)		// Gán mức logic cho 8 bit của biến temp ứng với giá trị của biến num	{		case 0: temp |= 0B10000000; break;		case 1: temp |= 0B11100011; break;		case 2: temp |= 0B01000100; break;		case 3: temp |= 0B01000001; break;		case 4: temp |= 0B00100011; break;		case 5: temp |= 0B00010001; break;		case 6: temp |= 0B00010000; break;		case 7: temp |= 0B11000011; break;
		case 8: temp |= 0B00000000; break;		case 9: temp |= 0B00000001; break;
	}
	
	PORTC = temp;		// Xuất giá trị logic mới ra PORTC để làm sáng LED 7 thanh
}void INIT()
{
	// Khởi tạo cấu hình các chân I/O
	DDRD |= 0xFF;		// Cấu hình Output cho các chân LED đơn
	PORTD = 0xFF;		// Dặt trạng thái logic 1 cho các chân các LED đơn

	DDRC |= 0xFF;		// Cấu hình Output cho các chân các LED 7 thanh
	PORTC = 0xFF;		// Dặt trạng thái logic 1 cho các chân các LED 7 thanh
	
	DDRB |= 0XF0;		// PB7:4 gửi data cho lcd, PB3:0 nhận tín hiệu từ button
	PORTB |= 0x0F;		// bật pull-up cho button

	DDRA |= 0xFE;		// PA7:1 output, PA0 input (nhận data từ LM35)
	PORTA = 0x00;		// Thả nổi đầu vào adc

	// Các hàm khởi tạo tham số cho bộ ADC
	ADC_PRES(128);	// Chia clock cho ADC = F_CPU / 128	ADC_2V56();		// Chọn AVCC (≈2.56V) làm Vref	ADC_IN(0);		// Chọn kênh ADC0 = chân PA0

	// Khởi tạo màn hình LCD	PORTA &= ~(1<<PA6);		// RW=0 (Chế độ ghi)	PORTA |= (1<<PA5);		// RS=1 (Thanh ghi dữ liệu)	LCD4_INIT(0,0);			// Hình dáng con trỏ ( ko gạch dưới, ko nhấp nháy )
}