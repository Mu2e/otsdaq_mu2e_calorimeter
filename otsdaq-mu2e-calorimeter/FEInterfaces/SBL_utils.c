
#include "SBL_utils.h"
#include "API_I2C.h"


/* === I2C Firmware Upgrade === */


// Function to read the firmware version ID
// TODO: Upgrade, this is only for fw 1
int Read_Firmware_Version_ID(char *filename){

    printf("Entering --> Read_Firmware_Version_ID\n");

    uint8_t ready;
    uint8_t funcID = Read_Firmware_Version_ID_CODE;
    uint8_t outbuf[4];
    ssize_t res;


    // 1. Wait for 0xAA, for SBL to be ready
    int cnt = 0;
    while(1){
        
        res = I2C_Read(&ready, 1, filename);

        // if (ready == 0xAA){
        if ((res ==1) && (ready == SBL_READY_BYTE)){
            printf("0xAA Captured\n");
            break;
        }

        // Quit after 20 sec
        if (cnt++ == Max_iter){
            printf("Aborting --> Read_Firmware_Version_ID\n");
            return -1;
        }

        msleep(Sleep_iter); // 50 ms
    }


    
    // 2. Write the function ID (0x31)
    res  = I2C_Write(&funcID, 1, filename);


    // 3. Read the firmware version ID
    if (I2C_Read(outbuf, sizeof(outbuf), filename) == sizeof(outbuf)){

        for (size_t i = 0; i < sizeof(outbuf); i++){
            printf("%d ", outbuf[i]);
        }
        printf("\n");


        // Done
        printf("Done --> Read_Firmware_Version_ID\n");

        return 0;
    } else {
        printf("Not read 4 Bytes...\n");
        return -2;
    }

}


int User_Application_Firmware_Upgrade(char* filename, int image, char* binFile){

    printf("Entering --> %s (%s:%d)\n", __FUNCTION__, __FILE__, __LINE__);

    uint8_t ready;
    // uint8_t funcID = User_Application_Firmware_Upgrade_CODE;
    uint8_t funcID = ((image==SBL_IMAGE1)? SBL_FwUpgrade1_FnID: SBL_FwUpgrade2_FnID);
    // uint8_t funcID = 0x32;;

    ssize_t res;


    // 1. Wait for 0xAA, for SBL to be ready
    int cnt = 0;
    while(1){
        
        res = I2C_Read(&ready, 1, filename);

        // if (ready == 0xAA){
        if ((res == 1) && (ready == SBL_READY_BYTE)){
            printf("0xAA Captured\n");
            break;
        }

        // Quit after 20 sec
        // if (cnt == 20){
        if (cnt++ == Max_iter){
            printf("Aborting --> %s (%s:%d)\n", __FUNCTION__, __FILE__, __LINE__);
            printf("0xAA non detected within 20 seconds\n");
            return 1;
        }

        // sleep(1); 
        msleep(Sleep_iter); // 50 ms
    }


    
    // 2. Write the function ID (0x31)
    res = I2C_Write(&funcID, 1, filename);
    if (res != 1) {
        printf("Failed to write function ID --> %x\n", funcID);
        return 2;
    }

    msleep(500);


    // 3. Read one block at a time from file
    FILE *file;
    uint8_t buffer[Firmware_Block_Write_Length]; // Each block transferred
    size_t bytesRead;

    uint16_t blockNum = 1;  // Number of the block
    uint8_t checkSum;  // Number of the block

    size_t i;

    // file = fopen("MU2E_SBLMZB_9658.bin", "rb");
    file = fopen(binFile, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error opening bin file.\n");
        return 3;
    }

    /* Breaks if
       - Not read from file a whole 1024 byte block
       - Reached the maximum size for the firmware ((0x40000 - 0x8000)/0x400 = 224)
    */
    while (blockNum < SBL_FW_CODE_BLOCK_NUM_MAX){

        // Try to read a block of data
        bytesRead = fread(buffer + Firmware_Block_Write_Offset, 1, Firmware_Block_Read_Length, file);

        // Break if not read a full block
        if (! (bytesRead == Firmware_Block_Read_Length)){
            break;
        }

        printf("Block read: %d\n", blockNum);
        
        // Initialize block
        checkSum = 0;
        buffer[0] = (uint8_t)(blockNum >> 8);
        buffer[1] = (uint8_t)(blockNum & 0xFF);
        
        // Loop over 1026 (2 address + 1024 data) to compute checksum
        for(i = 0; i < Firmware_Block_Write_Offset + Firmware_Block_Read_Length; i++){
            printf("%02x ", buffer[i]);
            checkSum -= buffer[i];
        }
        printf("\n");

        // Add checksum
        buffer[Firmware_Block_Write_Length - 1] = checkSum;

        // Write data block and wait for result byte
        res = Write_Wait_Result(buffer, Firmware_Block_Write_Length, filename);

        if (res < 0) {
            printf("Error (%ld) in --> %s (%s:%d)\n", res, __FUNCTION__, __FILE__, __LINE__);
            return 4;
        }

        blockNum++;

    }


    // 4. Block not completed --> Fill with 0xff
    if (feof(file)) {
        printf("End of file reached\n");

        // If last read was not a whole block, fill with 0xFF
        if (bytesRead > 0) {
            printf("Last block not completed --> Fill with 0xFF\n");

            printf("Block (partially) read: %d\n", blockNum);

            // Initialize block
            checkSum = 0;
            buffer[0] = (uint8_t)(blockNum >> 8);
            buffer[1] = (uint8_t)(blockNum & 0xFF);
            
            // Loop over 1026 (2 address + 1024 data) to compute checksum
            for( i = 0; i < Firmware_Block_Write_Offset + bytesRead; i++){
                printf("%02x ", buffer[i]);
                checkSum -= buffer[i];
            }
            // Fill with 0xFF
            for(; i < Firmware_Block_Write_Offset + Firmware_Block_Read_Length; i++){
                buffer[i] = 0xFF;
                printf("%02x ", 0xFF);
                checkSum -= buffer[i];
            }
            printf("\n");

            // Add checksum
            buffer[Firmware_Block_Write_Length - 1] = checkSum;

            // Write data block and wait for result byte
            res = Write_Wait_Result(buffer, Firmware_Block_Write_Length, filename);

            if (res < 0) {
                printf("Error (%ld) in --> %s (%s:%d)\n", res, __FUNCTION__, __FILE__, __LINE__);
                return 5;
            }

            blockNum++;


        }

        // 5. Fill remaining space with 0xff
        while (blockNum < SBL_FW_CODE_BLOCK_NUM_MAX){

            printf("Block -read-: %d\n", blockNum);

            // Initialize block
            checkSum = 0;
            buffer[0] = (uint8_t)(blockNum >> 8);
            buffer[1] = (uint8_t)(blockNum & 0xFF);
            
            // Loop over 1026 (2 address + 1024 data) to compute checksum
            // Fill with 0xFF
            for( i = 2; i < Firmware_Block_Write_Offset + Firmware_Block_Read_Length; i++){
                buffer[i] = 0xFF;
            }
            // Print and compute checksum
            for( i = 0; i < Firmware_Block_Write_Offset + Firmware_Block_Read_Length; i++){
                printf("%02x ", 0xFF);
                checkSum -= buffer[i];
            }
            printf("\n");

            // Add checksum
            buffer[Firmware_Block_Write_Length - 1] = checkSum;

            // Write data block and wait for result byte
            res = Write_Wait_Result(buffer, Firmware_Block_Write_Length, filename);

            if (res < 0) {
                printf("Error (%ld) in --> %s (%s:%d)\n", res, __FUNCTION__, __FILE__, __LINE__);
                return 6;
            }

            blockNum++;
        }



    } else if (ferror(file)) {
        perror("Error in reading file...\n");
        return 7;
    }

    // Close file
    fclose(file);


    /* At the end, write 0x00 0x00 0x00 to inform the SBL slave that there are no more data 
    to send from the SBL master
    This causes the MZB to reboot
    */

    uint8_t endBuf[numEndBytes] = {EndByte};
    res = I2C_Write(endBuf, numEndBytes, filename);

    if (res != numEndBytes){
        printf("Failed to write end sequence\n");
        return 8;
    }

    printf("Done! --> %s (%s:%d)\n", __FUNCTION__, __FILE__, __LINE__);

    return 0;

}


/* Writes a single block of memory and wait for the confirmation sequence
If not received, send the same block back again
- After 20 tries, the operation is aborted
- Return 0 if everything okay, negative error code otherwise*/
int Write_Wait_Result(uint8_t *buff, ssize_t bufferSize, char* filename){

    printf("Entering --> %s (%s:%d)\n", __FUNCTION__, __FILE__, __LINE__);

    ssize_t res;
    int macrocnt = 0;

    // Loop unless the comunication is okay (max 20 retry)
    while(macrocnt++ < 20){
        // Try to write buffer
        res = I2C_Write(buff, bufferSize, filename);

        if (res != bufferSize){
            printf("Error in writing block!!\n");
            // return -1;
            continue;
        }


        int numBytes = 0;
        uint8_t receivedBytes[3];
        // uint8_t* receivedBytes_p = &receivedBytes;

        int cnt = 0; // Insert a timeout

        // Wait for 3-bytes response
        while ((numBytes < 3) && (cnt++ < Max_iter)){

            // Read a maximum of 3 bytes
            // res = I2C_Read(receivedBytes_p, 3, filename);
            numBytes += I2C_Read(&receivedBytes[numBytes], 3 - numBytes, filename);

            // receivedBytes_p += res;
            // numBytes += res;

            msleep(Sleep_iter); // 50 ms
        }

        if (cnt == Max_iter){
            printf("Timeout reached!!\n");
            printf("Aborting!!\n");
            return -2;
        }

        // Check the response
        if ((receivedBytes[0]==SBL_Status_OK) && (receivedBytes[1]==SBL_Status_OK) && (receivedBytes[2]==SBL_Status_OK)){
            printf("Block correctly transferred\n");
            return 0;
        } else if ((receivedBytes[0]==SBL_Status_ERR) && (receivedBytes[1]==SBL_Status_ERR) && (receivedBytes[2]==SBL_Status_ERR)) {
            printf("Error code returned\n");
            printf("Retry\n");   
        } else {
            printf("Response not clear...\n");  
        }

    }

    return -3;

}


