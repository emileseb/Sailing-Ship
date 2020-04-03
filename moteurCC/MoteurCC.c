
#include "MoteurCC.h"
#include "ModuleRF.h"
#include "MyTimer.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_bus.h" // Pour l'activation des horloges

// pwm sur pa1 et gpio sur pa2
void MCC_GPIO_Conf(void){
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
	// conf gpio sens
	LL_GPIO_InitTypeDef initStartPa2 = {LL_GPIO_PIN_2, LL_GPIO_MODE_OUTPUT, LL_GPIO_MODE_OUTPUT_50MHz, LL_GPIO_OUTPUT_PUSHPULL, 0};
	LL_GPIO_Init(GPIOA, &initStartPa2);
	
	// conf gpio pwm
	LL_GPIO_InitTypeDef initStartPa1 = {LL_GPIO_PIN_1, LL_GPIO_MODE_ALTERNATE, LL_GPIO_MODE_OUTPUT_50MHz, LL_GPIO_OUTPUT_PUSHPULL, 0};
	LL_GPIO_Init(GPIOA, &initStartPa1);
}

void MCC_Conf(void){
	//tim2, ch 2, arr = 99, psc = 719
	MyTimer_Conf(TIM2,99,29);

	//pwm mode 1
	TIM2->CCMR1 &= ~TIM_CCMR1_OC2M_0;
	TIM2->CCMR1 |= TIM_CCMR1_OC2M_1;
	TIM2->CCMR1 |= TIM_CCMR1_OC2M_2;
	
	TIM2->CCMR1 |= TIM_CCMR1_OC2PE;
	TIM2->CCER |= TIM_CCER_CC2E;
	
	TIM2->CR1 |= TIM_CR1_ARPE;
	
	TIM2->EGR |= TIM_EGR_UG;
	
	MyTimer_Start(TIM2);
	
	MCC_GPIO_Conf();
}

void Tourner_Plateau(void){
	int vitesse = getVitesseRotation();
	if (vitesse > 0){
		TIM2->CCR2 = vitesse;
		LL_GPIO_WriteOutputPort(GPIOA, ~GPIO_ODR_ODR2);
	}
	else{
		TIM2->CCR2 = -vitesse;
		LL_GPIO_WriteOutputPort(GPIOA, GPIO_ODR_ODR2);
	}
}
