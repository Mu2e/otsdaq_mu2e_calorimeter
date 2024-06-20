#include <stdio.h>

#include "API_I2C.h"



#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>


int updateFW1();

int main() {

    /*

    MZB_OSCMDCODE_t command;
    float paramVect[9];
    uint8_t *vectToWrite;
    int res;


    // HVON
    command = HVONOFF;
    paramVect[0] = 1;
    for (int i=1; i<9; i++){
        paramVect[i] = NAN;
    }

    vectToWrite = MZB_Encode_CMD_Command_raw(command, paramVect);
    res = I2C_Write(vectToWrite, BUFFER_SIZE, I2C_BUS);
    printf("HVON --> Res: %d\n", res);
    sleep (1);


    // DACSET 
    command = DACSET;
    paramVect[0] = 0;
    paramVect[1] = 20;
    for (int i=2; i<9; i++){
        paramVect[i] = NAN;
    }

    vectToWrite = MZB_Encode_CMD_Command_raw(command, paramVect);
    res = I2C_Write(vectToWrite, BUFFER_SIZE, I2C_BUS);
    printf("DACSET --> Res: %d\n", res);
    sleep(1);




	printf("GetSize_Voltage: %ld\n", GetSize_Voltage());
	printf("GetSize_Voltage_Block: %ld\n", GetSize_Voltage_Block());
	printf("GetAddress_Voltage: %d\n", GetAddress_Voltage());
	printf("GetAddress_Voltage_Block: %d\n", GetAddress_Voltage_Block());
    */

    // printf("Sleeping\n");
    // msleep(1000);
    // printf("Awake\n");


    updateFW1();
    return 0;

}


int updateFW1(){

    // int image = 1; // Image to be upgraded


    // uint8_t fnID;
    // fnID =  image==SBL_IMAGE1? SBL_FwUpgrade1_FnID: SBL_FwUpgrade2_FnID;





    FILE *file;
    uint8_t buffer[Firmware_Block_Write_Length];
    size_t bytesRead;

    uint16_t blockNum = 0;  // Number of the block
    uint8_t checkSum;  // Number of the block

    file = fopen("MU2E_SBLMZB_9658.bin", "rb");
    if (file == NULL) {
        fprintf(stderr, "Error opening file.\n");
        return 1;
    }

    // Read a block of byte
    // feof(file) //  ferror(file)
    while ((bytesRead = fread(buffer + Firmware_Block_Write_Offset, 1, Firmware_Block_Read_Length, file)) == Firmware_Block_Read_Length) {

        printf("Block read: %d\n", blockNum);
        
        //
        checkSum = 0;
        buffer[0] = (uint8_t)(blockNum >> 8);
        buffer[1] = (uint8_t)(blockNum & 0xFF);
        
        // Loop over 1026 (2 address + 1024 data) to compute checksum
        for(int i = 0; i < Firmware_Block_Write_Offset + Firmware_Block_Read_Length; i++){
            printf("%02x ", buffer[i]);
            checkSum -= buffer[i];
        }
        
        printf("\n");
        buffer[Firmware_Block_Write_Length - 1] = checkSum;

        // Write data via I2C
        // ... 

        // Wait for response
        // ...
        
        // if (blockNum==1) break;

        blockNum++;
    }

    if (feof(file)) {
        printf("End of file reached\n");
            
            
            // If last read was not a whole block, fill with 0xFF 
            if (bytesRead > 0){
                printf("Last block not completed --> Fill with 0xFF\n");

            }


    } else if (ferror(file)) {
        perror("Error in reading file...\n");
        return 1;
    }




    // Close file
    fclose(file);


    return 0;

    
}