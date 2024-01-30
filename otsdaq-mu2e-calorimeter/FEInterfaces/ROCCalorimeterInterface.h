#ifndef _ots_ROCCalorimeterInterface_h_
#define _ots_ROCCalorimeterInterface_h_

#include "otsdaq-mu2e/FEInterfaces/ROCPolarFireCoreInterface.h"

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
	void 									configure				(void) override;


	// write and read to registers
	virtual void 							writeEmulatorRegister	(uint16_t address, uint16_t data_to_write) override;
	virtual uint16_t						readEmulatorRegister	(uint16_t address) override;



	bool emulatorWorkLoop(void) override;

	enum
	{
		ADDRESS_FIRMWARE_VERSION = 5,
		ADDRESS_MYREGISTER       = 0x65,
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
	
	void Configure(__ARGS__);
	void SetVoltageChannel(__ARGS__);
	void GetVoltageChannel(__ARGS__);
	void GetTempChannel(__ARGS__);

	// clang-format on
};

}  // namespace ots

#endif
