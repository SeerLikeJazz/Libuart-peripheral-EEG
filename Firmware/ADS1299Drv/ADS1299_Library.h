/*
insert header here

*/
#ifndef _____ADS1299_Library_h
#define _____ADS1299_Library_h


#include "ADS1299_Definitions.h"
#include <stdint.h>


#define  SPI_SS_PIN    								  6
#define  SPI_SCK_PIN   								  5
#define  SPI_MISO_PIN								    4
#define  SPI_MOSI_PIN 								  10

enum 
{
	Impedance     =1,
	Normal        =2,
	InternalShort =3,
	TestSignal    =4
};

typedef enum {
	SAMPLE_RATE_16000,
	SAMPLE_RATE_8000,
	SAMPLE_RATE_4000,
	SAMPLE_RATE_2000,
	SAMPLE_RATE_1000,
	SAMPLE_RATE_500,
	SAMPLE_RATE_250
} SAMPLE_RATE;


void SPI_User_init(void);

void initialize_ads(SAMPLE_RATE sr);
void ADS_ModeSelect(uint8_t mode);
uint8_t ADS_getDeviceID(void);
//void    RDATA(int);   // read data one-shot
void    ADS_RDATAC(void);  // go into read data continuous mode
void  ADS_RESET(void);   // set all register values to default
uint8_t ADS_RREG(uint8_t _address);            // read one ADS register
//void    RREGS(byte,byte,int);      // read multiple ADS registers
void ADS_SDATAC(void);  // get out of read data continuous mode

//void    STANDBY(int); // go into low power mode
void    ADS_START(void);   // start data acquisition
//void    STOP(int);    // stop data acquisition
//void    WAKEUP(int);  // get out of low power mode
void ADS_WREG(uint8_t _address, uint8_t _value);       // write one ADS register
//void    WREGS(byte,byte,int);      // write multiple ADS registers
uint8_t    ADS_xfer(uint8_t byte);        // SPI Transfer function

void updateBoardData(void);




#endif

