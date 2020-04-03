
#ifndef MRF_H
#define MRF_H

#include "stm32f103xb.h" 

void GPIO_Conf(void);

void MRF_Conf(void);

float getCycle(void);

//retoune la vitesse à appliquer au moteur entre -100 et 100 (valeur absolue = puissance, signe = sens)
float getVitesseRotation(void);
#endif
