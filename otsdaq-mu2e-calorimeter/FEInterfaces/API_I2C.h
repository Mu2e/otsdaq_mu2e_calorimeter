// #ifndef _API_I2C_H
// #define _API_I2C_H

#define GNU_COMP

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>


#include <fcntl.h>
#include <unistd.h>
// #include <time.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>


// For CT_MAXP. Place before eeSim_EE_DATABUF_t.h
// #include "../../ALB/CBRD/APP/ALB_config.h"
// #include "../../LIB/APP_config_define.h" 
#include "APP_config_define.h" 

// Definition of the memory-mapped structure of the mezanine
// #include "../../MZB/MCU/Drivers/eeSim.h"
// #include "../../MZB/MCU/Drivers/eeSim_EE_DATABUF_t.h" 
#include "eeSim_EE_DATABUF_t.h" 

// Command list
// #include "../../MZB/MCU/APD/MZB_commands.h"
// #include "../../MZB/MCU/APD/MZB_commands_MZB_OSCMDCODE_t.h"
#include "MZB_commands_MZB_OSCMDCODE_t.h"


// For checksum
// #include "../../MZB/MCU/APD/MZB_common.h"
// #include "../../MZB/MCU/APD/MZB_common_ees_checksum.h"
#include "MZB_common_ees_checksum.h"



#include "SBL_utils.h"




#define BUFFER_SIZE (2 + 2 + 2 + 9*4 + 4)
#define I2C_BUS "/dev/i2c-1"
#define member_size(type, member) (sizeof( ((type *)0)->member ))

void msleep(int msec);


// Function prototypes
uint8_t* MZB_Encode_CMD_Command_raw(MZB_OSCMDCODE_t cmdCode, float* params);
uint8_t* MZB_Encode_CMD_Command(MZB_OSCMDCODE_t cmdCode, float* params, int numParams);


ssize_t I2C_Write(uint8_t *buff, ssize_t bufferSize, char* filename);
int I2C_ReadAtAddr(uint16_t addr, uint8_t *outbuf, ssize_t bufferSize, char *filename);
ssize_t I2C_Read(uint8_t *outbuf, ssize_t bufferSize, char *filename);

ssize_t MZB_Encode_and_Write(MZB_OSCMDCODE_t cmdCode, float *params, int numParams, char *I2C_bus_file);




/* =========================================================================== */
/* ENTIRE BLOCK */

/* Get addresses and length of block sections in shared memory */
uint16_t GetAddress_Voltage_Block();          // VC
uint16_t GetAddress_Current_Block();          // CA
uint16_t GetAddress_Temperature_Block();      // TC
uint16_t GetAddress_VoltageSet_Block();       // HV
uint16_t GetAddress_CommandRequest_Block();   // CD

size_t GetSize_Voltage_Block();               // VC
size_t GetSize_Current_Block();               // CA
size_t GetSize_Temperature_Block();           // TC
size_t GetSize_VoltageSet_Block();            // HV
size_t GetSize_CommandRequest_Block();        // CD


/* Read an entire block section in shared memory */
uint8_t* Read_Voltage_Block(char* filename);
uint8_t* Read_Current_Block(char* filename);
uint8_t* Read_Temperature_Block(char* filename);
uint8_t* Read_VoltageSet_Block(char* filename);


/* =========================================================================== */
/* ONLY VALUES TO BE READOUT */

/* Get addresses and length of block sections in shared memory */
uint16_t GetAddress_Voltage();          // VC
uint16_t GetAddress_Current();          // CA
uint16_t GetAddress_Temperature();      // TC
uint16_t GetAddress_VoltageSet();       // HV

size_t GetSize_Voltage();               // VC
size_t GetSize_Current();               // CA
size_t GetSize_Temperature();           // TC
size_t GetSize_VoltageSet();            // HV


/* Read an entire block section in shared memory */
uint16_t* Read_Voltage(char* filename);
uint16_t* Read_Current(char* filename);
uint16_t* Read_Temperature(char* filename);
uint16_t* Read_VoltageSet(char* filename);



// #endif


