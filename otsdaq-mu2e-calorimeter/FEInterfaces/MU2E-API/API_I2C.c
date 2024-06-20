

#include "API_I2C.h"

// typedef unsigned char uint8_t;
// typedef unsigned short uint16_t;
// typedef unsigned int uint32_t;

//  Definition of the function should be compiled only here when using this APC
// #include "../../MZB/MCU/APD/MZB_common_ees_checksum.c"
#include "MZB_common_ees_checksum.c"

#include <sys/time.h>

void msleep(int msec){

    struct timeval stop, start;
    gettimeofday(&start, NULL);
    gettimeofday(&stop, NULL);

    while(((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec) < (msec * 1000)){
        gettimeofday(&stop, NULL);
    }

    
}


/*
This function allows to populate the buffer to be written in the MZB Board
via I2C at addresses 620-644 cfr. Doc page 9
*/
uint8_t* MZB_Encode_CMD_Command_raw(MZB_OSCMDCODE_t cmdCode, float* params) {

    int i;


    // Create structure and fill only to ease the checksum calculation

    #ifdef GNU_COMP
        static struct  __attribute__ ((__packed__)) ems {
    #else
        __packed __align(4) struct ems {
    #endif
            short dummy;
            short adr;
            
            #ifdef GNU_COMP
                struct __attribute__ ((__packed__)) {
            #else
                __packed struct {
            #endif
                    unsigned short functionReq_tag;  // 'CD'
                    unsigned short functionReq;
                    float P[CT_MAXP];

            #ifdef GNU_COMP
                // } __attribute__((packed, aligned(4))) 
                } __attribute__ ((aligned (4))) 
            #else        
                } 
            #endif
                cmdReq;
            unsigned chksum;
    #ifdef GNU_COMP
        // } __attribute__((packed, aligned(4))) 
        } __attribute__ ((aligned (4))) 
    #else    
        } 
    #endif
        em;


    // *** Fill the structure ***

    
    em.adr = offsetof(EE_DATABUF_t, functionReq_tag);

    // Structure id for the extended command request ('CD')
    em.cmdReq.functionReq_tag = 'C' | ('D' << 8);

    // Command request (enumerated)
    em.cmdReq.functionReq = cmdCode | 0x8000;


    // 9 parameters (float)
    for (i = 0; i < CT_MAXP; i++) {
        em.cmdReq.P[i] = params[i];
    }

    // Checksum
    em.chksum = 1 + (~ees_chksum((void *)&em.cmdReq, sizeof(em.cmdReq)));

    // Print for debug
    int debug = 0;
    if (debug){
        uint8_t *p = (uint8_t*)&em.adr;
        for(size_t i=0; i < (sizeof(em) - offsetof(struct ems, adr)); i+=8){
            for(int j=0; j<8; j++){
                printf("%02x ", *p++);
            }
            printf("\n");
        }
    }



    return (uint8_t*)&em.adr;

}


/* This function allows to obtain the byte sequence by passing a vector of floats and the number of
passed parameters, leaving the others to the default NAN value
*/
uint8_t* MZB_Encode_CMD_Command(MZB_OSCMDCODE_t cmdCode, float* params, int numParams){

    float paramVect[9];
    static uint8_t* vectToWrite;
    int i;


    // Assign variables based on arguments 
    paramVect[0] = 1;
    for (i=0; i<numParams; i++){
        paramVect[i] = params[i];
    }
    for (; i<CT_MAXP; i++){
        paramVect[i] = NAN;
    }

    // Call the function to obtain the 
    vectToWrite = MZB_Encode_CMD_Command_raw(cmdCode, paramVect);

    return vectToWrite;

}




/* Allows to write on the I2C bus a sequence of bytes (buff) with
bufferSize elements. The buffer is a file at filename
- Return the number of bytes written
- Negative value is an error
*/
ssize_t I2C_Write(uint8_t *buff, ssize_t bufferSize, char* filename) {

    int file;
    ssize_t bytesWritten;


    // Open I2C bus in write mode
    if ((file = open(filename, O_WRONLY)) < 0) {
        perror("Failed to open the i2c bus");
        return -1;
    }

    // Set MZB Address
    if (ioctl(file, I2C_SLAVE, I2C_SLAVE_EE_ADDR) < 0) {
        perror("Failed to acquire bus access and/or talk to slave");
        return -2;
    }

    // Write data on I2C bus
    bytesWritten = write(file, buff, bufferSize);
    if (bytesWritten != bufferSize) {
        perror("Failed to write to the i2c bus (actual data)");
        return -3;
    }

    // Close file
    close(file);

    // Return written data
    return bytesWritten;
    // return 0;

}



/* This funciton allows to read data from the MZB Shared Memory, that is,
   first write the location to read, and then read a sequence of bytes */
int I2C_ReadAtAddr(uint16_t addr, uint8_t* outbuf, ssize_t bufferSize, char* filename){

    int file;
    // char *filename = I2C_BUS;


    // Open I2C bus in write mode
    if ((file = open(filename, O_RDWR)) < 0) {
        perror("Failed to open the i2c bus");
        return 1;
    }

    // Set MZB Address
    if (ioctl(file, I2C_SLAVE, I2C_SLAVE_EE_ADDR) < 0) {
        perror("Failed to acquire bus access and/or talk to slave");
        return 2;
    }

    // Write the location to read on I2C bus
    uint8_t* byte_addr;
    byte_addr = (uint8_t *)&addr;
    if (write(file, byte_addr, 2) != 2) {
        perror("Failed to write to the i2c bus (actual data)");
        return 3;
    }

    // Read data
    if (read(file, outbuf, bufferSize) != bufferSize) {
        perror("Errore nella lettura dei dati dal dispositivo I2C");
        return 4;
    }

    // Close file
    close(file);

    return 0;
    
}



/* This funciton allows to read data from the I2C Bus 
- Return the number of bytes read
- Negative value is an error
*/
ssize_t I2C_Read(uint8_t *outbuf, ssize_t bufferSize, char *filename){

    int file;
    ssize_t bytesRead;


    // Open I2C bus in write mode
    if ((file = open(filename, O_RDONLY)) < 0) {
        perror("Failed to open the i2c bus");
        return -1;
    }

    // Set MZB Address
    if (ioctl(file, I2C_SLAVE, I2C_SLAVE_EE_ADDR) < 0) {
        perror("Failed to acquire bus access and/or talk to slave");
        return -2;
    }


    // Read data
    bytesRead = read(file, outbuf, bufferSize);
    // if (bytesRead != bufferSize) {
    //     perror("Errore nella lettura dei dati dal dispositivo I2C");
    //     return -4;
    // }


    // Close file
    close(file);

    // Return number of bytes read
    return bytesRead;

    // return 0;
    
}



/*Allows to encode a command and then send via the I2C Bus*/
ssize_t MZB_Encode_and_Write(MZB_OSCMDCODE_t cmdCode, float *params, int numParams, char *I2C_bus_file){

    uint8_t *vectToWrite;
    ssize_t res;

    // Obtain byte sequence
    vectToWrite = MZB_Encode_CMD_Command(cmdCode, params, numParams);

    // Write via the I2C Bus
    res = I2C_Write(vectToWrite, BUFFER_SIZE, I2C_bus_file);

    return res;

}







/* =========================================================================== */
/* ENTIRE BLOCK */

/* Get addresses and length of block sections in shared memory */


// Address of VL section
uint16_t GetAddress_Voltage_Block(){
    return offsetof(EE_DATABUF_t, apdBiasV_tag);
}

// Address of CA section
uint16_t GetAddress_Temperature_Block(){
    return offsetof(EE_DATABUF_t, apdBiasA_tag);
}

// Address of TC section
uint16_t GetAddress_Current_Block(){
    return offsetof(EE_DATABUF_t, apdTemp_tag);
}

// Address of HV section
uint16_t GetAddress_VoltageSet_Block(){
    return offsetof(EE_DATABUF_t, biasVreq_tag);
}

// Address of CD section
uint16_t GetAddress_CommandRequest_Block(){
    return offsetof(EE_DATABUF_t, functionReq_tag);
}


// Size of VL section
size_t GetSize_Voltage_Block(){
    return offsetof(EE_DATABUF_t, VL_checksum) - offsetof(EE_DATABUF_t, apdBiasV_tag) + member_size(EE_DATABUF_t, VL_checksum);
}

// Size of CA section
size_t GetSize_Current_Block(){
    return offsetof(EE_DATABUF_t, CA_checksum) - offsetof(EE_DATABUF_t, apdBiasA_tag) + member_size(EE_DATABUF_t, CA_checksum);
}

// Size of TC section
size_t GetSize_Temperature_Block(){
    return offsetof(EE_DATABUF_t, TC_checksum) - offsetof(EE_DATABUF_t, apdTemp_tag) + member_size(EE_DATABUF_t, TC_checksum);
}

// Size of HV section
size_t GetSize_VoltageSet_Block(){
    return offsetof(EE_DATABUF_t, bias_chksum) - offsetof(EE_DATABUF_t, biasVreq_tag) + member_size(EE_DATABUF_t, bias_chksum);
}

// Size of CD section
size_t GetSize_CommandRequest_Block(){
    return offsetof(EE_DATABUF_t, fncReq_chksum) - offsetof(EE_DATABUF_t, functionReq_tag) + member_size(EE_DATABUF_t, fncReq_chksum);
}






/* Read an entire block section in shared memory */

// Read the Voltage (VL) section
uint8_t* Read_Voltage_Block(char* filename){

    // Obtain address and length for Voltage section
    uint16_t tmpAddress = GetAddress_Voltage_Block();
    ssize_t tmpSize = GetSize_Voltage_Block();

    // Allocate output buffer
    uint8_t* tmpOutBuff;
    tmpOutBuff = (uint8_t*)malloc(tmpSize);

    // Return the byte sequence
    if (! I2C_ReadAtAddr(tmpAddress, tmpOutBuff, tmpSize, filename) ){
        return tmpOutBuff;
    }

    return NULL;
}

// Read the Current (CA) section
uint8_t* Read_Current_Block(char* filename){

    // Obtain address and length for Current section
    uint16_t tmpAddress = GetAddress_Current_Block();
    ssize_t tmpSize = GetSize_Voltage_Block();

    // Allocate output buffer
    uint8_t* tmpOutBuff;
    tmpOutBuff = (uint8_t*)malloc(tmpSize);

    // Return the byte sequence
    if (! I2C_ReadAtAddr(tmpAddress, tmpOutBuff, tmpSize, filename) ){
        return tmpOutBuff;
    }

    return NULL;
}

// Read the Temperature (TC) section
uint8_t* Read_Temperature_Block(char* filename){

    // Obtain address and length for Temperature section
    uint16_t tmpAddress = GetAddress_Temperature_Block();
    ssize_t tmpSize = GetSize_Voltage_Block();

    // Allocate output buffer
    uint8_t* tmpOutBuff;
    tmpOutBuff = (uint8_t*)malloc(tmpSize);

    // Return the byte sequence
    if (! I2C_ReadAtAddr(tmpAddress, tmpOutBuff, tmpSize, filename) ){
        return tmpOutBuff;
    }

    return NULL;
}

// Read the VoltageSet (HV) section
uint8_t* Read_VoltageSet_Block(char* filename){

    // Obtain address and length for VoltageSet section
    uint16_t tmpAddress = GetAddress_VoltageSet_Block();
    ssize_t tmpSize = GetSize_Voltage_Block();

    // Allocate output buffer
    uint8_t* tmpOutBuff;
    tmpOutBuff = (uint8_t*)malloc(tmpSize);

    // Return the byte sequence
    if (! I2C_ReadAtAddr(tmpAddress, tmpOutBuff, tmpSize, filename) ){
        return tmpOutBuff;
    }

    return NULL;
}






/* =========================================================================== */
/* ONLY VALUES TO BE READOUT */


// Address of VL 
uint16_t GetAddress_Voltage(){
    return offsetof(EE_DATABUF_t, apdBiasV);
}

// Address of CA 
uint16_t GetAddress_Temperature(){
    return offsetof(EE_DATABUF_t, apdBiasA);
}

// Address of TC 
uint16_t GetAddress_Current(){
    return offsetof(EE_DATABUF_t, apdTemp);
}

// Address of HV 
uint16_t GetAddress_VoltageSet(){
    return offsetof(EE_DATABUF_t, biasVreq);
}




// Size of VL 
size_t GetSize_Voltage(){
    return member_size(EE_DATABUF_t, apdBiasV);
}

// Size of CA 
size_t GetSize_Current(){
    return member_size(EE_DATABUF_t, apdBiasA);
}

// Size of TC 
size_t GetSize_Temperature(){
    return member_size(EE_DATABUF_t, apdTemp);
}

// Size of HV 
size_t GetSize_VoltageSet(){
    return member_size(EE_DATABUF_t, biasVreq);
}







/* Read an entire block section in shared memory */

// Read the Voltage (VL) section
uint16_t* Read_Voltage(char* filename){

    // Obtain address and length for Voltage section
    uint16_t tmpAddress = GetAddress_Voltage();
    ssize_t tmpSize = GetSize_Voltage();

    // Allocate output buffer
    uint8_t* tmpOutBuff;
    tmpOutBuff = (uint8_t*)malloc(tmpSize);

    // Return the byte sequence
    if (! I2C_ReadAtAddr(tmpAddress, tmpOutBuff, tmpSize, filename) ){
        return (uint16_t*)tmpOutBuff;
    }

    return NULL;
}

// Read the Current (CA) section
uint16_t* Read_Current(char* filename){

    // Obtain address and length for Current section
    uint16_t tmpAddress = GetAddress_Current();
    ssize_t tmpSize = GetSize_Voltage();

    // Allocate output buffer
    uint8_t* tmpOutBuff;
    tmpOutBuff = (uint8_t*)malloc(tmpSize);

    // Return the byte sequence
    if (! I2C_ReadAtAddr(tmpAddress, tmpOutBuff, tmpSize, filename) ){
        return (uint16_t*)tmpOutBuff;
    }

    return NULL;
}

// Read the Temperature (TC) section
uint16_t* Read_Temperature(char* filename){

    // Obtain address and length for Temperature section
    uint16_t tmpAddress = GetAddress_Temperature();
    ssize_t tmpSize = GetSize_Voltage();

    // Allocate output buffer
    uint8_t* tmpOutBuff;
    tmpOutBuff = (uint8_t*)malloc(tmpSize);

    // Return the byte sequence
    if (! I2C_ReadAtAddr(tmpAddress, tmpOutBuff, tmpSize, filename) ){
        return (uint16_t*)tmpOutBuff;
    }

    return NULL;
}

// Read the VoltageSet (HV) section
uint16_t* Read_VoltageSet(char* filename){

    // Obtain address and length for VoltageSet section
    uint16_t tmpAddress = GetAddress_VoltageSet();
    ssize_t tmpSize = GetSize_Voltage();

    // Allocate output buffer
    uint8_t* tmpOutBuff;
    tmpOutBuff = (uint8_t*)malloc(tmpSize);

    // Return the byte sequence
    if (! I2C_ReadAtAddr(tmpAddress, tmpOutBuff, tmpSize, filename) ){
        return (uint16_t*)tmpOutBuff;
    }

    return NULL;
}




