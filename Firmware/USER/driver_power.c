
#include "driver_power.h"
#include "nrf_delay.h"





/*
	模拟电源使能端
*/
void AVDD_enable(void)
{
	nrf_gpio_cfg_output(AVDD_EN_PIN);//AVDD_EN
	nrf_gpio_pin_set(AVDD_EN_PIN);//AVDD_EN
}

/*
	模拟电源使能端
*/
void AVDD_disable(void)
{
	nrf_gpio_pin_clear(AVDD_EN_PIN);
}

/*
	3V3 LDO打开
*/
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

/*
	3V3 LDO关闭
*/
void cpc_ldo1_disable(void)
{
	nrf_gpio_pin_clear(DVDD_EN_PIN);
	nrf_delay_ms(10);	
}

/*
	充电引脚初始化
*/
void ChargerPin_Init(void)
{
	nrf_gpio_cfg_input(CHARGE_VCHECK_PIN,NRF_GPIO_PIN_NOPULL);	//充电检测，接5V/2=2.5v
  nrf_gpio_cfg_input(CHARGE_STA_PIN,NRF_GPIO_PIN_NOPULL);   
}
