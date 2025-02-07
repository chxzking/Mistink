#include "stm32f10x.h"                  // Device header

#include "Mistink.h"
#include "M_stm32f103c8t6.h"

const unsigned char kun2[]={//48*48
	
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0x3F,0x1F,0x0F,0x07,0x03,0x03,0x01,0x03,0x83,0x83,0x81,0x01,0x03,0x07,0x07,0x07,
0x0F,0x17,0x2F,0x3F,0x7F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x3F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0x03,0x01,0x02,
0x00,0x00,0x00,0x00,0x00,0x00,0x8C,0xFF,0xFF,0xEF,0xEF,0x27,0xA4,0xA0,0x80,0x80,
0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,
0x6B,0xFB,0xF5,0xE3,0xFB,0xFC,0xFF,0x20,0x8F,0xFF,0xFF,0xFF,0xFF,0xFE,0xFC,0xF0,
0xFC,0xFE,0xFF,0xFF,0xFF,0x7F,0x6F,0x27,0xA7,0xA7,0x7F,0x7F,0x7F,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0x7C,0xF0,0xE8,0xEC,0xF8,0xF8,0xFC,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0x06,0x0E,0x0F,0x2F,0x07,0x07,0x07,0x0E,0x0F,0x19,0x03,0x03,0x01,0x01,0x01,0x01,
0x01,0x03,0x03,0x0F,0x1F,0x3E,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x1F,0x1F,0x0F,0x0F,
0x47,0x77,0x3F,0x1E,0x00,0x06,0x02,0x7F,0x9F,0xEF,0xEF,0x73,0xBB,0xD9,0xEE,0xF2,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x0C,0x5E,0xBE,0xBC,0xB8,0xDC,0xDC,0xDE,0xDF,0xFF,0xFD,0xFE,0x1A,0xB1,0xFF,0xFF,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x08,0x00,
0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x03,0x03,0x02,0x00,0x00,0x00,0x07,0x03,0x03
	
};


volatile uint32_t frame_count = 0;
volatile uint32_t fps = 0;

//创建帧率计算模块
void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        fps = frame_count;
        frame_count = 0;
    }
}

void init_timer(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Prescaler = 7200 - 1;  // 10 kHz
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = 10000 - 1;  // 1 second
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &timerInitStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);

    NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
    nvicStructure.NVIC_IRQChannelSubPriority = 1;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicStructure);
}

void frame_capture(void) {
    frame_count++;
}

int main(){
	init_timer();
	
	//创建显示器实例
	//连接驱动
	MistinkPltfrmDrv_t MistinkPlatformDrivers;
	MistinkPlatformDrivers.__Init = OLED_Init;
	MistinkPlatformDrivers.__BufferOutput = OLED_BufferOutput;
	MistinkPlatformDrivers.__Clear = OLED_Clear;
	//创建128*64的显示器实例
	MistinkDisplayer_t* displayer = MistinkDisplayer_Create(64,128,&MistinkPlatformDrivers);
	
	
	while(1){
		
		M_PictureLoad(displayer,8,40,kun2,48,48);//绘制图片
		M_Int(displayer,0,0,fps);//显示帧率
		frame_capture();//计算帧率
		MistinkDisplayer_BufferOutput(displayer);//缓存区发送到屏幕
	}
		
}
