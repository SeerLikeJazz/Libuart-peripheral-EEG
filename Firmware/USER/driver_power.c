
#include "driver_power.h"
#include "nrf_delay.h"
#include "app_timer.h"



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

/*
	sleep前关闭耗电的部分
*/
void SPI_uninit();
void prepare_before_sleep(void)
{
	app_timer_stop_all();
	nrf_gpio_pin_clear(ADS_PWDN_PIN); //1299芯片	PWDN
	SPI_uninit();
	
	AVDD_disable();
	cpc_ldo1_disable();	
	
	nrf_gpio_cfg_default(2);
	nrf_gpio_cfg_default(DRDY_PIN);
	nrf_gpio_cfg_default(SPI_MISO_PIN);
	nrf_gpio_cfg_default(SPI_SCK_PIN);
	nrf_gpio_cfg_default(SPI_SS_PIN);
	nrf_gpio_cfg_default(ADS_START_PIN);
	nrf_gpio_cfg_default(ADS_RESET_PIN);
	nrf_gpio_cfg_default(ADS_PWDN_PIN);
	nrf_gpio_cfg_default(SPI_MOSI_PIN);
	
//	nrf_gpio_cfg_default(BUTTON_2);
	nrf_gpio_cfg_default(BUTTON_3);
	nrf_gpio_cfg_default(BUTTON_4);
	
	nrf_gpio_cfg_default(DVDD_EN_PIN);
//	nrf_gpio_cfg_default(AVDD_EN_PIN);

		
	nrf_gpio_cfg_default(LED_1);			//LED1
	nrf_gpio_cfg_default(LED_2);			//LED2		
	nrf_gpio_cfg_default(LED_3);	
	nrf_gpio_cfg_default(LED_4);	
}



