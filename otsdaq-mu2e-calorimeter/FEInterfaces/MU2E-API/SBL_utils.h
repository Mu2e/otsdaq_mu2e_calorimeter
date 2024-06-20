#ifndef _SBL_UTILS_H
#define _SBL_UTILS_H


#include <stdint.h>
#include <unistd.h>


/* From sbl.h */

/********Definitions ***********/
/* Communication */
#define SBL_SPI                     (0x01)
#define SBL_I2C                     (0x02)
#define SBL_CONNECTION              (SBL_I2C)

#if (SBL_CONNECTION == SBL_I2C)
#define I2C_SLV_ADDR                (0x60)
#endif

/** Memory Map */
/** Secondary bootloader code max size */
#define SBL_SECTOR0_CODE_SIZE    	(0x8000) 

/** User firmware max size (code + parameteres) */
#define SBL_FW_TOTAL_SIZE_MAX	    (0x40000 - SBL_SECTOR0_CODE_SIZE) 
/** User firmware parameter size */
#define SBL_FW_PARAM_SIZE   	    (0x8000)
/** User firmware parameter size */
#define SBL_FW_CODE_SIZE_MAX	    (SBL_FW_TOTAL_SIZE_MAX) 

/** User firmware code offset */
#define SBL_IMAGEGOLD                     (0)
#define SBL_IMAGE1  	                    (1) 
#define SBL_IMAGE2  	                    (2) 
#define SBL_FW_IMAGE0_OFFSET  	          (0) 
#define SBL_FW_IMAGE1_OFFSET  	          (SBL_FW_IMAGE0_OFFSET+SBL_SECTOR0_CODE_SIZE) 
#define SBL_FW_IMAGE2_OFFSET  	          (SBL_FW_IMAGE1_OFFSET+SBL_FW_TOTAL_SIZE_MAX) 

#define SBL_FIRMWARE_VERSION_ADDR       	(0xCC)
#define SBL_FIRMWARE_CHKSUM_ADDR       		(0xD0)


/** Block size 1024B */
#define SBL_BLOCK_SIZE		(0x400)

/** User firmware in blocks */
#define SBL_FW_CODE_BLOCK_NUM_MAX	    (SBL_FW_CODE_SIZE_MAX/SBL_BLOCK_SIZE) 

#define SBL_SECTOR0_BLOCK_NUM_MAX		  (SBL_SECTOR0_CODE_SIZE/SBL_BLOCK_SIZE)


/** Function ID */
/**  Firmware Version ID Function */
#define SBL_ReadFirmVersionID1_FnID	(0x31)
#define SBL_ReadFirmVersionID2_FnID	(0x41)
#define SBL_FwVersionID_ParamSize	(4)
#define SBL_FwVersionID_Major_Idx 	(0)
#define SBL_FwVersionID_Minor_Idx	(2.0)
#define SBL_FwVersionID_Rev_Idx		(2.4)
#define SBL_FwVersionID_Mag_Idx		(3)

/** Upgrade Firmware Function */
#define SBL_FwUpgrade1_FnID			(0x32) 
#define SBL_FwUpgrade2_FnID			(0x42) 
#define SBL_FwUpgrade_ParamSize	    (SBL_FW_DATA_BLOCK_SIZE+3)
#define SBL_FwUpgrade_MSB_Idx       (0)
#define SBL_FwUpgrade_LSB_Idx       (1)
#define SBL_FwUpgrade_Fw_Idx        (2)
#define SBL_FwUpgrade_Checksum_Idx  (SBL_FwUpgrade_Fw_Idx+SBL_FW_DATA_BLOCK_SIZE) 
#define SBL_FwUpgrade_Checksum_Done_Idx  (2) 

#define SBL_Status_ParamSize		(3)
#define SBL_Status_OK				(0x55)
#define SBL_Status_ERR				(0xFF)

/** Sector 0 programming */
#define SBL_Sector0Upgrade_FnID		(0x33)
#define SBL_Sector0Upgrade_ParamSize	(SBL_FW_DATA_BLOCK_SIZE+4)
#define SBL_Sector0Upgrade_FnID_Idx      (0)
#define SBL_Sector0Upgrade_MSB_Idx       (1)
#define SBL_Sector0Upgrade_LSB_Idx       (2)
#define SBL_Sector0Upgrade_Data_Idx		 (3)
#define SBL_Sector0Upgrade_Checksum_Idx  (SBL_Sector0Upgrade_Data_Idx+SBL_FW_DATA_BLOCK_SIZE)
#define SBL_Sector0Upgrade_Checksum_Done_Idx  (3) 

/** Run User firmware */
#define SBL_RunUserFirmware		(0x34)
#define SBL_ForceImage1Firmware		(0x35)
#define SBL_ForceImage2Firmware		(0x45)

/** SBL ready */
#define WAIT_SLAVE_READY            (1)
#define SBL_READY_BYTE				(0xAA)

/** SBL Return Code Values */
/* Function runs ok */
#define SBL_OK					(0x0000)	
/* All firmware data is received */
#define SBL_FW_UPGRADE_END		(0x0001)
/* There is an error when running the function */
#define SBL_ERR					(0xFFFF)
/* The input parameter values are invalid */
#define SBL_INVALID_PARAM 		(0x1001)
/* Communication error: ACK isn't received */
#define	SBL_COMM_ERR			(0x1002)
/* Slave has error when writing to flash memory */
#define SBL_FLASH_WRITE_ERR			(0x1004)
/* The Command ID in Upgrade Firmware function is incorrect */
#define SBL_INVALID_CMD_ID	(0x1008)
/* Check sum is incorrect */
#define SBL_CHECKSUM_ERR		(0x1010)
/* Timeout error */
#define SBL_TIMEOUT_ERR		    (0x1020)
/* Invalid image */
#define SBL_INVALID_IMAGE		    (0x1120)

#define SBL_RETRIES_MAX_NUM		(10)
#define SBL_TIMEOUT             (2000)          // about 2s

/********Data Types ***********/
/** SBL Function ID Type */
typedef uint8_t SBL_FUNC_ID;
/** SBL Status Type */
typedef uint8_t SBL_STATUS;
/** SBL return code Type */
typedef int16_t SBL_RET_CODE;

/** SBL Transfer Information */
typedef struct
{
	uint8_t* TxBuf;			// pointer to transmit buffer
	uint8_t* RxBuf;			// Pointer to receive buffer
	uint32_t TxLen;			// size of transmit buffer
	uint32_t RxLen;			// size of receive buffer
} SBL_TRANSER_Type, *PSBL_TRANSER_Type;

/** Firmware Version ID structure */
typedef struct
{
	uint16_t buildNo;			    // Build Number
	uint8_t Minor  :4;		    // Monor revision
	uint8_t Major  :4;		  	// Major revision
	uint8_t Magic;      		  // Magic number
} SBL_FirmVersion_Info, *PSBL_FirmVersion_Info;





/* From sbl_master.h */
// static __inline void * SBL_getFirmareCodeStartAddr(int i)  {return (void*)(i==SBL_IMAGE1?SBL_FW_IMAGE1_OFFSET:SBL_FW_IMAGE2_OFFSET);}
// static __inline void * SBL_getFirmareCodeEndAddr(int i)    {return (void*)((unsigned)SBL_getFirmareCodeStartAddr(i) + SBL_FW_CODE_SIZE_MAX);}
// static __inline void * SBL_getFirmareVersionAddr(int i)    {return (void*)((unsigned)SBL_getFirmareCodeStartAddr(i) + SBL_FIRMWARE_VERSION_ADDR);}
// static __inline void * SBL_getChksumAddr(int i)            {return (void*)((unsigned)SBL_getFirmareCodeStartAddr(i) + SBL_FIRMWARE_CHKSUM_ADDR);}



/* Defined by me */

/* I2C Firmware Upgrade */
#define Read_Firmware_Version_ID_CODE 0x31
#define User_Application_Firmware_Upgrade_CODE 0x32

#define Firmware_Block_Read_Length 1024 // Block to be read from bin file
#define Firmware_Block_Write_Length 1027 // Block num (2) + Data (1024) + Checksum (1) - Each block to be write
#define Firmware_Block_Write_Offset 2 // 

#define numEndBytes 3
#define EndByte 0x00


#define Sleep_iter (50) // msleep(50) ms
#define Max_iter (20*20) // Max 20 sec = 400 iter

int Read_Firmware_Version_ID(char* filename);
int User_Application_Firmware_Upgrade(char* filename, int image, char* binFile);

int Write_Wait_Result(uint8_t* buff, ssize_t bufferSize, char* filename );


#endif