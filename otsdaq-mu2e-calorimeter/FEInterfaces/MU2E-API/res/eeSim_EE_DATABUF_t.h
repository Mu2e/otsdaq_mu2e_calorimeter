#ifndef _EESIM_DATABUF_H
#define _EESIM_DATABUF_H

#define I2C_SLAVE_EE_ADDR  (0x59)



#ifdef GNU_COMP
  typedef struct __attribute__((__packed__)) {
#else
  typedef __packed struct EE_DATABUF_tag {
#endif
    uint16_t ridx;
    uint16_t reserved1[3];

    uint16_t updateCounter_tag;           // 'RR'
    uint16_t reserved2;
    uint32_t updateCounter;
    uint32_t stsreg;                      // Status register
    uint32_t errreg;                      // Error register
    uint32_t errcnt;
    uint32_t ch_ensts;
    uint32_t gpi;
    uint32_t gpo;
    uint16_t ladc[MZB_LADC_CHANNELS];     // LPC internal ADC
    uint16_t reserved3[8- MZB_LADC_CHANNELS];
    uint16_t lastError;
    uint8_t  slewRate;
    uint8_t  hvmfsm;                      // HV modulator state
    uint16_t auxadc[MCADC_MAX_CHANNELS];  // Auxiliary ADC
    uint8_t  flipCount[MCADC_MAX_CHANNELS];// Flip status of AIO
    uint32_t RR_checksum;                 // RR section checksum

    uint16_t apdBiasV_tag;                // 'VL'
    uint16_t reserved4;
    uint16_t apdBiasV[FEE_NUM];
    uint32_t VL_checksum;                 // VL section checksum

    uint16_t apdBiasA_tag;                // 'CA'
    uint16_t reserved5;
    uint16_t apdBiasA[FEE_NUM];
    uint32_t CA_checksum;                 // CA section checksum

    uint16_t apdTemp_tag;                 // 'TC'
    uint16_t reserved6;
    uint16_t apdTemp[FEE_NUM];
    uint32_t TC_checksum;                 // TC section checksum

    uint16_t apdPwrs_tag;                 // 'PS'
    uint16_t reserved7;
    uint16_t apdPwrs [FEE_NUM];
    uint16_t apdPwrsE[FEE_NUM];
    uint16_t apdPwrsF[FEE_NUM];
    uint16_t apdPwrsG[FEE_NUM];
    uint16_t apdPwrsH[FEE_NUM];
    uint32_t PS_checksum;                 // PS section checksum

    uint16_t apdError_tag;                // 'ER'
    uint16_t reserved8;
    uint16_t ioe_errcnt,  ioe_lastError;
    uint16_t spi_errcnt,  spi_lastError;
    uint16_t i2cm_errcnt, i2cm_lastError;
    uint16_t i2cs_errcnt, i2cs_lastError;
    uint16_t ladc_errcnt, ladc_lastError;
    #ifdef GNU_COMP
      struct __attribute__((__packed__)) {
    #else
      __packed struct {
    #endif
        uint16_t errcnt;
        uint16_t lastErr;
        uint16_t adc_errcnt;
        uint16_t adc_lastErr;
        uint16_t dac_errcnt;
        uint16_t dac_lastErr;
    } apd_err[FEE_NUM];
    uint16_t reserved8_1[2];
    uint32_t ER_checksum;                 // ER section checksum

    uint16_t apdCalib_tag;                // 'CL'
    uint16_t reserved9[3];
    #ifdef GNU_COMP
      struct __attribute__((__packed__)) {
    #else
      __packed struct {
    #endif
        double slope;
        double offset;
    } edac_cal[FEE_NUM];
    #ifdef GNU_COMP
      struct __attribute__((__packed__)) {
    #else
      __packed struct {
    #endif
        double slope;
        double offset;
    } eads0_cal[FEE_NUM];
    #ifdef GNU_COMP
      struct __attribute__((__packed__)) {
    #else
      __packed struct {
    #endif
        double slope;
        double offset;
    } eads1_cal[FEE_NUM];
    #ifdef GNU_COMP
      struct __attribute__((__packed__)) {
    #else
      __packed struct {
    #endif
        double slope;
        double offset;
    } eads2_cal[FEE_NUM];
    uint32_t CL_checksum;                 // CL section checksum

    uint16_t cmdReqIssued;
    uint16_t reserved11;

    uint16_t ch_enstsReq_tag;                // 'EN'
    uint16_t reserved12;
    uint32_t ch_enstsReq;
    uint32_t reserved12_1;
    uint32_t ensts_chksum;

    uint16_t commandReq_tag;                 // 'W1'
    uint16_t reserved13;
    uint16_t commandReq;
    uint16_t reserved14[3];
    uint32_t cmdReq_chksum;

    uint16_t biasVreq_tag;                  // 'HV'
    uint16_t reserved15;
    uint16_t biasVreq[FEE_NUM];
    uint32_t bias_chksum;

    uint16_t functionReq_tag;               // 'CD'
    uint16_t functionReq;
    float P[CT_MAXP];
    uint32_t fncReq_chksum;

    uint32_t reserved17;

    uint32_t sentinel_tag;
    uint32_t reserved18[3];
    } EE_DATABUF_t;



#endif