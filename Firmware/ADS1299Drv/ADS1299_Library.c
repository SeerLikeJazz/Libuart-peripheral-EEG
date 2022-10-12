#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_delay.h"
#include "nrf_drv_spi.h"

#include "ADS1299_Library.h"


#define SPI_INSTANCE  0 /**< SPI instance index. */


static volatile bool spi_xfer_done;  //SPI���ݴ�����ɱ�־
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);  /* < SPI instance. */

static uint8_t    spi_tx_buf[256];   /**< TX buffer. */
static uint8_t    spi_rx_buf[256];   /**< RX buffer. */

/*	SPI�ж��¼�������	*/
void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
                       void *                    p_context)
{
  //����SPI�������  
	spi_xfer_done = true;
}



void SPI_User_init(void)
{
	/*	����SPI�����ֹ���ģʽ
		ģʽ				����
	ģʽ0			CPOL=0			CPHA=0
	ģʽ1			CPOL=0			CPHA=1
	ģʽ2			CPOL=1			CPHA=0
	ģʽ3			CPOL=1			CPHA=1	
	*/
	//���ö���spi��ʼ���ṹ�� = Ĭ������
	nrf_drv_spi_config_t	spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
	//��дĬ�����������������
	spi_config.ss_pin = NRF_DRV_SPI_PIN_NOT_USED;									//�û��Զ��壬�����������
	spi_config.mosi_pin = SPI_MOSI_PIN;
	spi_config.miso_pin =	SPI_MISO_PIN;
	spi_config.sck_pin = 	SPI_SCK_PIN;
	
	spi_config.irq_priority = SPI_DEFAULT_CONFIG_IRQ_PRIORITY;		//�ж����ȼ�������ں궨���������õ���6
	spi_config.frequency = NRF_DRV_SPI_FREQ_8M;										//spiƵ������8M
	spi_config.orc = 0xFF;																				//ORC�Ĵ�������0xff
	spi_config.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;				//���ݴ����λ��ǰ
	spi_config.mode = NRF_DRV_SPI_MODE_1;													//ģʽ1
	
	  //��ʼ��SPI
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, NULL, NULL));	
	
}


/**
  * @brief  CS ���� ����
  * @param  SS ѡ���Ӧ��CS����
  * @retval None
  */
void csLow(void)
{
	nrf_gpio_pin_clear(SPI_SS_PIN);
	
}
/**
  * @brief  CS ���� ����
  * @param  SS ѡ���Ӧ��CS����
  * @retval None
  */
void csHigh(void)
{
	nrf_gpio_pin_set(SPI_SS_PIN);
}
/**
  * @brief  SPI�����ȡһ�ֽ�
  * @param  byte Ҫ������ֽ�
  * @retval ��ȡ���ֽ�
  */

uint8_t    ADS_xfer(uint8_t byte)
{
	uint8_t d_read;						
	
	//8λ��ַ
	spi_tx_buf[0] =	 byte;			
	//�������ݴ���
	APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, spi_tx_buf, 1, spi_rx_buf, 1));			
	d_read = spi_rx_buf[0];		
	//���ض�ȡ��ֵ
	return	d_read;	
}

uint8_t    regData[24]; // array is used to mirror register data
/**
  * @brief  д�Ĵ���
  * @param  _address  �Ĵ�����ַ
	* @param  _value    Ҫд���ֵ
	* @param  target_SS ѡ���Ӧ��ADS1299
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
  * @brief  ���Ĵ���
  * @param  _address  �Ĵ�����ַ
	* @param  target_SS ѡ���Ӧ��ADS1299
  * @retval ��ȡ��ֵ
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
  * @brief  ��λ
	* @param  target_SS ѡ���Ӧ��ADS1299
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
  * @brief  ֹͣ����������ȡģʽ
	* @param  target_SS ѡ���Ӧ��ADS1299
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
  * @brief  ��ȡADS1299��ID
	* @param  target_SS ѡ���Ӧ��ADS1299
  * @retval IDֵ
  */
uint8_t ADS_getDeviceID(void)
{ // simple hello world com check
  uint8_t data = ADS_RREG(ID_REG);
  return data;
}
/**
  * @brief  ADS1299��ʼת��
	* @param  target_SS ѡ���Ӧ��ADS1299
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
  * @brief  ����������ȡģʽ
	* @param  target_SS ѡ���Ӧ��ADS1299
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
  * @brief  ��ʼ��Ads1299
	* @param  None
  * @retval None
  */

void initialize_ads(SAMPLE_RATE sr)
{	
	nrf_delay_ms(200);// recommended power up sequence requiers >Tpor (2��18��Tclk) pdf.70
	
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
  * @brief  ����Ads1299ģʽ��1-Impedance��2-Normal��3-Short noise��4-Test wave��
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
//				ADS_WREG(MISC1,0x20, BOTH_ADS);//����SRB1
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
//				ADS_WREG(MISC1,0x20, BOTH_ADS);//����SRB1
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
//				/*ʹ��DMAǰ��CS����*/
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
/*����ת����int*/
int boardChannelDataInt[24] = {0xAABBCCDD};;
/*������ʱ�洢*/
uint8_t eCon_Message[240] = {0xBB,0xAA};
int byteCounter = 2;//ǰ��2�ֽ�Ϊ��ͷ
/*�����*/
uint8_t IDX;

void  data_prepare();

/**
  * @brief  ����ADS1299������
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
			/*�������ݰ�*/
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
		//У��λ
		eCon_Message[218] = 0;
		//�����
		eCon_Message[219] = IDX++;	
		data_prepare();
	}
		

	

}














/*У��λ����*/
uint8_t eCon_Checksum(uint8_t *content,uint8_t len)
{
	uint8_t result = 0;
	for(int i=0;i<len;i++) {
		result += content[i];
	}
	return ~result;
}


