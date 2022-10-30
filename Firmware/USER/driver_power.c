
#include "driver_power.h"
#include "nrf_delay.h"





//ģ���Դʹ�ܶ�
void AVDD_enable(void)
{
	nrf_gpio_cfg_output(AVDD_EN_PIN);//AVDD_EN
	nrf_gpio_pin_set(AVDD_EN_PIN);//AVDD_EN
}

void AVDD_disable(void)
{
	nrf_gpio_pin_clear(AVDD_EN_PIN);
}

void cpc_ldo1_enable(void)
{
	nrf_gpio_cfg_output(DVDD_EN_PIN);//DVDD_EN
	nrf_delay_us(10);
	for(uint8_t i=0; i<7; i++)
	{
		nrf_gpio_pin_set(DVDD_EN_PIN);
		nrf_delay_us(500);	
		nrf_gpio_pin_clear(DVDD_EN_PIN);
		nrf_delay_us(100);	
	}	
	nrf_gpio_pin_set(DVDD_EN_PIN);
}


void cpc_ldo1_disable(void)
{
	nrf_gpio_pin_clear(DVDD_EN_PIN);
	nrf_delay_ms(10);	
}
