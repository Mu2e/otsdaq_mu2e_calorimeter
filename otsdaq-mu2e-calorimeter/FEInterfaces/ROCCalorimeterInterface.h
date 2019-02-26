#ifndef _ots_ROCCalorimeterInterface_h_
#define _ots_ROCCalorimeterInterface_h_

#include "otsdaq-mu2e/FEInterfaces/ROCPolarFireCoreInterface.h"

namespace ots
{
class ROCCalorimeterInterface : public ROCPolarFireCoreInterface
{
  public:
	ROCCalorimeterInterface(const std::string&       rocUID,
	                        const ConfigurationTree& theXDAQContextConfigTree,
	                        const std::string&       interfaceConfigurationPath);

	~ROCCalorimeterInterface(void);

	// write and read to registers
	virtual void writeEmulatorRegister(unsigned address, unsigned data_to_write) override;
	virtual int  readEmulatorRegister(unsigned address) override;

	void configure(void) override;

	bool emulatorWorkLoop(void) override;

	enum
	{
		ADDRESS_FIRMWARE_VERSION = 5,
		ADDRESS_MYREGISTER       = 0x65,
	};

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
	
	void SetVoltageChannel(__ARGS__);
	void GetVoltageChannel(__ARGS__);
};

}  // namespace ots

#endif
