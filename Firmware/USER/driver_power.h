#ifndef  _DRIVER_POWER_H_
#define  _DRIVER_POWER_H_


#include "pca10040.h"





void AVDD_enable(void);
void AVDD_disable(void);

void cpc_ldo1_enable(void);
void cpc_ldo1_disable(void);

void ChargerPin_Init(void);



#endif

