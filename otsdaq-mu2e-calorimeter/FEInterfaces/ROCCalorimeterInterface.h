#ifndef _ots_ROCCalorimeterInterface_h_
#define _ots_ROCCalorimeterInterface_h_

#include "otsdaq-mu2e/FEInterfaces/ROCPolarFireCoreInterface.h"

#define MZ_ADDRESS 262
#define MZ_BUFFER_SIZE 46

namespace ots
{
class ROCCalorimeterInterface : public ROCPolarFireCoreInterface
{
	// clang-format off
  public:
	ROCCalorimeterInterface(const std::string&       rocUID,
	                        const ConfigurationTree& theXDAQContextConfigTree,
	                        const std::string&       interfaceConfigurationPath);

	~ROCCalorimeterInterface(void);



	// state machine
	//----------------
	// state machine
	//----------------
	void 								configure					(void) override;
	//void 								halt						(void) override;
	//void 								pause						(void) override;
	//void 								resume						(void) override;
	//void 								start						(std::string runNumber) override;
	//void 								stop						(void) override;
	bool 								running		                (void) override;


	// write and read to registers
	virtual void 							writeEmulatorRegister	(uint16_t address, uint16_t data_to_write) override;
	virtual uint16_t						readEmulatorRegister	(uint16_t address) override;


	virtual void 							readROCBlock			(std::vector<DTCLib::roc_data_t>& data, DTCLib::roc_address_t address, uint16_t wordCount, bool incrementAddress) override; 
	virtual void 						universalBlockRead			(char* address, char* returnValue, unsigned int numberOfBytes) override;

	bool emulatorWorkLoop(void) override;

	enum CaloRegisters
	{

        ROC_ADDRESS_DDRRESET                 = 14,
        ROC_ADDRESS_ANALOGRESET              = 13,
        ROC_ADDRESS_IS_PATTERN               = 8,

        ROC_ADDRESS_ERRCNT                   = 17,

  		ROC_ADDRESS_WORKMODE                 = 122,

		ROC_ADDRESS_EW_LENGHT                = 123,
		ROC_ADDRESS_EW_BLIND                 = 124,
		ROC_ADDRESS_EW_DELAY                 = 125,

		ROC_ADDRESS_MASK_A                   = 120,
		ROC_ADDRESS_MASK_B                   = 121,
		ROC_ADDRESS_BASE_THRESHOLD           = 100,
 
		ROC_ADDRESS_IS_COUNTER               = 79,
		ROC_ADDRESS_COUNTER_IS_FALLING       = 80,
		ROC_ADDRESS_COUNTER_SIZE             = 81,

		ROC_ADDRESS_IS_LASER                 = 78,
		ROC_ADDRESS_LASER_DELAY              = 77,

		ROC_ADDRESS_MZB_BUSY                 = 140, 

		ROC_ADDRESS_SELECTLINE               = 141, 

		ROC_ADDRESS_BOARD_ID                 = 142, 

		ROC_ADDRESS_OSCMODE_FLAG             = 148, 
		ROC_ADDRESS_OSCMODE_LENGHT           = 149, 

		ROC_ADDRESS_BOARD_U_ID_LSB           = 145, 
		ROC_ADDRESS_BOARD_U_ID_CSB           = 146,
		ROC_ADDRESS_BOARD_U_ID_MSB           = 147



	};


	int GetTemperature(int idchannel);
	//	temperature--
	class Thermometer
	{
	  private:
		double mnoiseTemp;

	  public:
		void noiseTemp(double intemp)
		{
			mnoiseTemp =
			    (double)intemp + 0.05 * (intemp * ((double)rand() / (RAND_MAX)) - 0.5);
			return;
		}
		double GetBoardTempC() { return mnoiseTemp; }
	};

	Thermometer temp1_;
	double      inputTemp_;
	
	void Configure									(__ARGS__);
	void ScarsiTest									(__ARGS__);
	void SetVoltageChannel							(__ARGS__);
	void GetVoltageChannel							(__ARGS__);
	void GetTempChannel								(__ARGS__);

	// void SetupForPatternDataTaking					(__ARGS__); // Moved to ROCPolarFireCoreInterface::ROCPolarFireCoreInterface, otsdaq_mu2e/otsdaq-mu2e/FEInterfaces/ROCPolarFireCoreInterfaceImpl.cc
	void SetupForPatternFixedLengthDataTaking		(__ARGS__);
    void SetupForPatternFixedLengthDataTaking       (unsigned int numberOfWords);	
	void SetupForADCsDataTaking		(__ARGS__);
    void SetupForADCsDataTaking		(unsigned int threshold);	

	void SendMzCommand(__ARGS__);
    void SendMzCommand(std::string command, float paramVect[]);
	void EnableAndPowerSiPMs(__ARGS__);
    void EnableAndPowerSiPMs(bool hvonoff, float vbias);
	void SetBoardVoltages(__ARGS__);
    void SetBoardVoltages(bool hvonoff, int boardID, std::string conf);
    void SetupForNoiseTaking(__ARGS__);
    void SetupForNoiseTaking(unsigned int numberOfsamples);
	void RMZB_writeAllSiPMbias(float *hv);

	void ConfigureLink(__ARGS__);
    void ConfigureLink(std::string conf, bool hvonoff, bool doCalibration);
	void CalibrateMZB(__ARGS__);
    void CalibrateMZB(int boardID);


	void ReadROCErrorCounter		(__ARGS__);



	virtual void GetStatus							(__ARGS__) override;

private:


	static const std::set<DTCLib::roc_address_t>		SPECIAL_BLOCK_READ_ADDRS_;

	// clang-format on
};

}  // namespace ots

#endif
