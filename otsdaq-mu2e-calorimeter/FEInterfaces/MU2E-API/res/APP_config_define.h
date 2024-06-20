#ifndef _APP_CONFIG_DEFINE_H
#define _APP_CONFIG_DEFINE_H


#define CT_MAJOR                2       /* MU2E-SPI major release version        */
#define CT_MINOR                2       /* MU2E-SPI minor release version        */
#define APP_MSG_BUFLEN         128
#define CT_MAXP                 9       /* Number of allowed parameters    */

#define MCADC_MAX_CHANNELS       (8)    /* Maximum number of channels for multichannel ADC */

#define FEE_NUM			    	      ( 20)
#define FEE_ADC_CHANNELS 	      (  8)
#define MZB_LADC_CHANNELS				(  5)

#define MU2E_HV_MAX_V           (250)

#define APP_MAGICID (0x135BU)


// Console assigned to UART0
#define CONSOLE_baudrate      115200
#define CONSOLE_IRQHandler    UART0_IRQHandler
#define CONSOLE_UART          LPC_UART0
#define CONSOLE_IRQn          UART0_IRQn

#define MU2E_FIRMWARE_DESC        "(C) ENEA-INFN 2012-2024 - MU2E-ADC board (SiPM SPI version)"



#endif