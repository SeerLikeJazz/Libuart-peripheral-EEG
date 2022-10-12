#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_delay.h"
#include "nrf_drv_spi.h"

#include "ADS1299_Library.h"


#define SPI_INSTANCE  0 /**< SPI instance index. */


static volatile bool spi_xfer_done;  //SPI数据传输完成标志
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /* < SPI instance. */

static uint8_t    spi_tx_buf[256];   /**< TX buffer. */
static uint8_t    spi_rx_buf[256];   /**< RX buffer. */

/*	SPI中断事件服务函数	*/
void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context)
{
  //设置SPI传输完成  
	spi_xfer_done = true;
}



void SPI_User_init(void)
{
	/*	关于SPI的四种工作模式
		模式				描述
	模式0			CPOL=0			CPHA=0
	模式1			CPOL=0			CPHA=1
	模式2			CPOL=1			CPHA=0
	模式3			CPOL=1			CPHA=1	
	*/
	//配置定义spi初始化结构体 = 默认配置
	nrf_drv_spi_config_t	spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
	//重写默认配置里的引脚配置
	spi_config.ss_pin = NRF_DRV_SPI_PIN_NOT_USED;									//用户自定义，交给软件处理
	spi_config.mosi_pin = SPI_MOSI_PIN;
	spi_config.miso_pin =	SPI_MISO_PIN;
	spi_config.sck_pin = 	SPI_SCK_PIN;
	
	spi_config.irq_priority = SPI_DEFAULT_CONFIG_IRQ_PRIORITY;		//中断优先级，这个在宏定义里面设置的是6
	spi_config.frequency = NRF_DRV_SPI_FREQ_8M;										//spi频率设置8M
	spi_config.orc = 0xFF;																				//ORC寄存器内容0xff
	spi_config.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;				//数据传输高位在前
	spi_config.mode = NRF_DRV_SPI_MODE_1;													//模式1
	
	  //初始化SPI
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, NULL, NULL));	
	
}


/**
  * @brief  CS 引脚 拉低
  * @param  SS 选择对应的CS引脚
  * @retval None
  */
void csLow(void)
{
	nrf_gpio_pin_clear(SPI_SS_PIN);
	
}
/**
  * @brief  CS 引脚 拉高
  * @param  SS 选择对应的CS引脚
  * @retval None
  */
void csHigh(void)
{
	nrf_gpio_pin_set(SPI_SS_PIN);
}
/**
  * @brief  SPI传输读取一字节
  * @param  byte 要传输的字节
  * @retval 读取的字节
  */

uint8_t    ADS_xfer(uint8_t byte)
{
	uint8_t d_read;						
	
	//8位地址
	spi_tx_buf[0] =	 byte;			
	//启动数据传输
	APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, spi_tx_buf, 1, spi_rx_buf, 1));			
	d_read = spi_rx_buf[0];		
	//返回读取数值
	return	d_read;	
}

uint8_t    regData[24]; // array is used to mirror register data
/**
  * @brief  写寄存器
  * @param  _address  寄存器地址
	* @param  _value    要写入的值
	* @param  target_SS 选择对应的ADS1299
  * @retval None
  */
void ADS_WREG(uint8_t _address, uint8_t _value)
{                                 //  Write ONE register at _address
  uint8_t opcode1 = _address + 0x40; //  WREG expects 010rrrrr where rrrrr = _address
  csLow();               //  open SPI
  ADS_xfer(opcode1);                  //  Send WREG command & address
  ADS_xfer(0x00);                     //  Send number of registers to read -1
  ADS_xfer(_value);                   //  Write the value to the register
  csHigh();              //  close SPI
  regData[_address] = _value;     //  update the mirror array

}
/**
  * @brief  读寄存器
  * @param  _address  寄存器地址
	* @param  target_SS 选择对应的ADS1299
  * @retval 读取的值
  */
uint8_t ADS_RREG(uint8_t _address)
{                                 //  reads ONE register at _address
  uint8_t opcode1 = _address + 0x20; //  RREG expects 001rrrrr where rrrrr = _address
  csLow();                //  open SPI
  ADS_xfer(opcode1);                  //  opcode1
  ADS_xfer(0x00);                     //  opcode2
  regData[_address] = ADS_xfer(0x00); //  update mirror location with returned byte
  csHigh();               //  close SPI

  return regData[_address]; // return requested register value
}
/**
  * @brief  复位
	* @param  target_SS 选择对应的ADS1299
  * @retval None
  */
void ADS_RESET(void)
{ // reset all the registers to default settings
  csLow();
  ADS_xfer(_RESET);
  nrf_delay_ms(1); //must wait 18 tCLK cycles to execute this command (Datasheet, pg. 35)
  csHigh();
}
/**
  * @brief  停止数据连续读取模式
	* @param  target_SS 选择对应的ADS1299
  * @retval None
  */
void ADS_SDATAC(void)
{
  csLow();
  ADS_xfer(_SDATAC);
  csHigh();
  nrf_delay_ms(1); //must wait at least 4 tCLK cycles after executing this command (Datasheet, pg. 37)
}
/**
  * @brief  读取ADS1299的ID
	* @param  target_SS 选择对应的ADS1299
  * @retval ID值
  */
uint8_t ADS_getDeviceID(void)
{ // simple hello world com check
  uint8_t data = ADS_RREG(ID_REG);
  return data;
}
/**
  * @brief  ADS1299开始转换
	* @param  target_SS 选择对应的ADS1299
  * @retval None
  */
void  ADS_START(void)
{ //start data conversion
  csLow();
  ADS_xfer(_START); // KEEP ON-BOARD AND ON-DAISY IN SYNC
	for(int j=250;j>0;j--){};									//4 Tclk delay
  csHigh();
}
/**
  * @brief  数据连续读取模式
	* @param  target_SS 选择对应的ADS1299
  * @retval None
  */
void  ADS_RDATAC(void)
{
  csLow();
  ADS_xfer(_RDATAC); // read data continuous
	for(int j=250;j>0;j--){};						//4 Tclk delay
  csHigh();
		
  nrf_delay_ms(1);
}
/**
  * @brief  初始化Ads1299
	* @param  None
  * @retval None
  */

void initialize_ads(SAMPLE_RATE sr)
{	
	nrf_delay_ms(200);// recommended power up sequence requiers >Tpor (2的18次Tclk) pdf.70
	
	ADS_RESET();
	nrf_delay_ms(10);// recommended to wait 18 Tclk before using device
	
	ADS_SDATAC();
	nrf_delay_ms(10);
	
	if(ADS_getDeviceID() != 0x3E) {
 		while(1);
	};
	nrf_delay_ms(10);
}


/**
  * @brief  设置Ads1299模式（1-Impedance、2-Normal、3-Short noise、4-Test wave）
	* @param  None
  * @retval None
  */
void ADS_ModeSelect(uint8_t mode)
{
	switch(mode) {
//		case Impedance:
//			{
//				ADS_WREG(LOFF,0x02);
//				HAL_Delay(10);

//				ADS_WREG(BIAS_SENSP,0xFF);
//				HAL_Delay(10);
//				ADS_WREG(BIAS_SENSN,0xFF);
//				HAL_Delay(10);
//				ADS_WREG(LOFF_SENSP,0xFF);
//				HAL_Delay(10);
//				ADS_WREG(LOFF_SENSN,0x00, BOTH_ADS);
//				HAL_Delay(10);
//				ADS_WREG(LOFF_FLIP,0x00, BOTH_ADS);
//				HAL_Delay(10);
//				ADS_WREG(MISC1,0x20, BOTH_ADS);//设置SRB1
//				HAL_Delay(10);
//				
//				ADS_WREG(CH1SET,0x60, BOTH_ADS);
//				HAL_Delay(10);
//				ADS_WREG(CH2SET,0x60, BOTH_ADS);
//				HAL_Delay(10);
//				ADS_WREG(CH3SET,0x60, BOTH_ADS);
//				HAL_Delay(10);
//				ADS_WREG(CH4SET,0x60, BOTH_ADS);
//				HAL_Delay(10);
//				ADS_WREG(CH5SET,0x60, BOTH_ADS);
//				HAL_Delay(10);
//				ADS_WREG(CH6SET,0x60, BOTH_ADS);
//				HAL_Delay(10);
//				ADS_WREG(CH7SET,0x60, BOTH_ADS);
//				HAL_Delay(10);
//				ADS_WREG(CH8SET,0x60, BOTH_ADS);
//				HAL_Delay(10);
//				
//				ADS_START(BOTH_ADS);
//				HAL_Delay(40);
//				ADS_RDATAC(BOTH_ADS);
//				HAL_Delay(10);
//			}
//			break;
//		case Normal:
//			{
//				ADS_WREG(BIAS_SENSP,0xFF, BOTH_ADS);
//				HAL_Delay(10);
//				ADS_WREG(BIAS_SENSN,0xFF, BOTH_ADS);
//				HAL_Delay(10);
//				ADS_WREG(MISC1,0x20, BOTH_ADS);//设置SRB1
//				HAL_Delay(10);
//				
//				ADS_WREG(CH1SET,0x60, BOTH_ADS);
//				HAL_Delay(10);
//				ADS_WREG(CH2SET,0x60, BOTH_ADS);
//				HAL_Delay(10);
//				ADS_WREG(CH3SET,0x60, BOTH_ADS);
//				HAL_Delay(10);
//				ADS_WREG(CH4SET,0x60, BOTH_ADS);
//				HAL_Delay(10);
//				ADS_WREG(CH5SET,0x60, BOTH_ADS);
//				HAL_Delay(10);
//				ADS_WREG(CH6SET,0x60, BOTH_ADS);
//				HAL_Delay(10);
//				ADS_WREG(CH7SET,0x60, BOTH_ADS);
//				HAL_Delay(10);
//				ADS_WREG(CH8SET,0x60, BOTH_ADS);
//				HAL_Delay(10);	
//				
//				ADS_START(BOTH_ADS);
//				HAL_Delay(40);
//				ADS_RDATAC(BOTH_ADS);
//				HAL_Delay(10);
//				/*使用DMA前，CS拉低*/
//			//	csLow(BOTH_ADS);
//			}
//			break;
		case InternalShort:
			{
	ADS_WREG(CONFIG3,0xE0);
	nrf_delay_ms(100);
	ADS_WREG(CONFIG1,0x95);
	nrf_delay_ms(100);
				ADS_WREG(CONFIG2,0xC0);
				nrf_delay_ms(10);
								
				ADS_WREG(CH1SET,0x01);
				nrf_delay_ms(10);
				ADS_WREG(CH2SET,0x01);
				nrf_delay_ms(10);
				ADS_WREG(CH3SET,0x01);
				nrf_delay_ms(10);
				ADS_WREG(CH4SET,0x01);
				nrf_delay_ms(10);
				ADS_WREG(CH5SET,0x01);
				nrf_delay_ms(10);
				ADS_WREG(CH6SET,0x01);
				nrf_delay_ms(10);
				ADS_WREG(CH7SET,0x01);
				nrf_delay_ms(10);
				ADS_WREG(CH8SET,0x01);
				nrf_delay_ms(10);

				ADS_START();
				nrf_delay_ms(40);
				ADS_RDATAC();
				nrf_delay_ms(10);
			}
			break;
		case TestSignal:
			{
	ADS_WREG(CONFIG3,0xE0);
	nrf_delay_ms(100);
	ADS_WREG(CONFIG1,0x95);
	nrf_delay_ms(100);
	ADS_WREG(CONFIG2,0xD0);
	nrf_delay_ms(100);
				ADS_WREG(CH1SET,0x05);
				nrf_delay_ms(10);
				ADS_WREG(CH2SET,0x05);
				nrf_delay_ms(10);
				ADS_WREG(CH3SET,0x05);
				nrf_delay_ms(10);
				ADS_WREG(CH4SET,0x05);
				nrf_delay_ms(10);
				ADS_WREG(CH5SET,0x05);
				nrf_delay_ms(10);
				ADS_WREG(CH6SET,0x05);
				nrf_delay_ms(10);
				ADS_WREG(CH7SET,0x05);
				nrf_delay_ms(10);
				ADS_WREG(CH8SET,0x05);
				nrf_delay_ms(10);
				
				ADS_START();
				nrf_delay_ms(40);	
				ADS_RDATAC();
				nrf_delay_ms(10);
			}
			break;
		default:;
	}
}



int  boardStat;
/*数据转换成int*/
int boardChannelDataInt[24] = {0xAABBCCDD};;
/*数据临时存储*/
uint8_t eCon_Message[240] = {0xBB,0xAA};
int byteCounter = 2;//前面2字节为包头
/*包序号*/
uint8_t IDX;

void  data_prepare();

/**
  * @brief  更新ADS1299的数据
  * @retval None
  */
void updateBoardData(void)
{ 
	uint8_t inByte;
	
	csLow();
  for (int i = 0; i < 3; i++)
  {
    inByte = ADS_xfer(0x00); //  read status register (1100 + LOFF_STATP + LOFF_STATN + GPIO[7:4])
    boardStat = (boardStat << 8) | inByte;
  }
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 3; j++)
    { //  read 24 bits of channel data in 8 3 byte chunks
      inByte = ADS_xfer(0x00);
			/*放入数据包*/
			eCon_Message[byteCounter] = inByte;
      byteCounter++;
      boardChannelDataInt[i+1] = (boardChannelDataInt[i+1] << 8) | inByte; // int data goes here
    }
  }

  csHigh(); // close SPI
	
  // need to convert 24bit to 32bit if using the filter
//  for (int i = 0; i < 8; i++)
//  { // convert 3 byte 2's compliment to 4 byte 2's compliment
//    if ((boardChannelDataInt[i+1] & 0x00800000) == 0x00800000)
//    {
//      boardChannelDataInt[i+1] |= 0xFF000000;
//    }
//    else
//    {
//      boardChannelDataInt[i+1] &= 0x00FFFFFF;
//    }
//  }
	if(byteCounter == 218){ //2+24x9
		byteCounter = 2;
		//校验位
		eCon_Message[218] = 0;
		//包序号
		eCon_Message[219] = IDX++;	
		data_prepare();
	}
		

	

}














/*校验位函数*/
uint8_t eCon_Checksum(uint8_t *content,uint8_t len)
{
	uint8_t result = 0;
	for(int i=0;i<len;i++) {
		result += content[i];
	}
	return ~result;
}


