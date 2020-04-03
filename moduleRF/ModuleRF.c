

#include "ModuleRF.h"
#include "MyTimer.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_tim.h"

// R:\LLGDB\CubeF1\Drivers\STM32F1xx_HAL_Driver\HTML

// variable privée qui mémorise pour le module le timer utilisé par le module
static TIM_TypeDef * MRF_Timer = TIM4;

void GPIO_Conf(void){
	// timer 4 channel 1 = pb6
	LL_GPIO_InitTypeDef initStartPb6 = {LL_GPIO_PIN_6, LL_GPIO_MODE_INPUT, 0, 0, 0};
	LL_GPIO_Init(GPIOB, &initStartPb6);
	
	// timer 4 channel 2 = pb7
	LL_GPIO_InitTypeDef initStartPb7 = {LL_GPIO_PIN_7, LL_GPIO_MODE_INPUT, 0, 0, 0};
	LL_GPIO_Init(GPIOB, &initStartPb7);
}

/**
	* @brief  Configure le mrf. 
  * @note   A lancer avant toute autre fonction.
	* @param  Timer : indique le timer à utiliser par le chronomètre, TIM1, TIM2, TIM3 ou TIM4
  * @retval None
  */
void MRF_Conf(void)
{
	// Réglage Timer pour un débordement à 10ms, timer clk = 72 Mhz
	//Le timer doit deborder a la frequence de la pwm (20 ms), arr = 14399 et psc = 9
	MyTimer_Conf(MRF_Timer,14399,9);
	
	//configuration du timer en input pwm
	/*LL_TIM_IC_InitTypeDef initPwmInput = {LL_TIM_IC_POLARITY_FALLING, LL_TIM_ACTIVEINPUT_DIRECTTI, LL_TIM_ICPSC_DIV1, LL_TIM_IC_FILTER_FDIV1};
	LL_TIM_IC_Init (MRF_Timer, LL_TIM_CHANNEL_CH2, &initPwmInput); 	
	
	LL_TIM_IC_InitTypeDef initPwmInputReseter = {LL_TIM_IC_POLARITY_RISING, LL_TIM_ACTIVEINPUT_DIRECTTI, LL_TIM_ICPSC_DIV1, LL_TIM_IC_FILTER_FDIV1};
	LL_TIM_IC_Init (MRF_Timer, LL_TIM_CHANNEL_CH1, &initPwmInputReseter);*/
	
	// Select the active input for TIMx_CCR1: write the CC1S bits to 01 in the TIMx_CCMR1 register (TI1 selected).
	MRF_Timer->CCMR1 |= TIM_CCMR1_CC1S_0;
	MRF_Timer->CCMR1 &= ~TIM_CCMR1_CC1S_1;
	
	// Select the active polarity for TI1FP1 (used both for capture in TIMx_CCR1 and counter clear): write the CC1P to ‘0’ (active on rising edge).
	MRF_Timer->CCER &= ~TIM_CCER_CC1P;
	
	// Select the active input for TIMx_CCR2: write the CC2S bits to 10 in the TIMx_CCMR1 register (TI1 selected).
	MRF_Timer->CCMR1 &= ~TIM_CCMR1_CC2S_0;
	MRF_Timer->CCMR1 |= TIM_CCMR1_CC2S_1;
	
	// Select the active polarity for TI1FP2 (used for capture in TIMx_CCR2): write the CC2P bit to ‘1’ (active on falling edge).
	MRF_Timer->CCER |= TIM_CCER_CC2P;

	// Select the valid trigger input: write the TS bits to 101 in the TIMx_SMCR register (TI1FP1 selected).
	MRF_Timer->SMCR |= TIM_SMCR_TS_0;
	MRF_Timer->SMCR &= ~TIM_SMCR_TS_1;
	MRF_Timer->SMCR |= TIM_SMCR_TS_2;
	
	// Configure the slave mode controller in reset mode: write the SMS bits to 100 in the TIMx_SMCR register.
	MRF_Timer->SMCR &= ~TIM_SMCR_SMS_0;
	MRF_Timer->SMCR &= ~TIM_SMCR_SMS_1;
	MRF_Timer->SMCR |= TIM_SMCR_SMS_2;
	
	// Enable the captures: write the CC1E and CC2E bits to ‘1 in the TIMx_CCER register.
	MRF_Timer->CCER |= TIM_CCER_CC1E;
	MRF_Timer->CCER |= TIM_CCER_CC2E;
	
	GPIO_Conf();
	
	MyTimer_Start(MRF_Timer);
}

float getCycle(void){
	int periode = MRF_Timer->CCR1;
	int valTim = MRF_Timer->CCR2;
	float cycle = (1.5 / 0.94)* (float)valTim / (float)periode;
	return cycle;
}

float getVitesseRotation(void){
	float vitesse = (getCycle() - 1.5) * 100 / 0.4; // 0.4 : ecart entre position neutre et extremas (exemple 1.5 et 1.9)
	float marge = 14.0;
	if ((vitesse < marge) && (vitesse > -marge)){
		vitesse = 0;
	}
	return vitesse;
}
