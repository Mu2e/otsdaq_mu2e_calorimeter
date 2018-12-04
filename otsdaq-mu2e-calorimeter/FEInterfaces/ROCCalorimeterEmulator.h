#ifndef _ots_ROCCalorimeterEmulator_h_
#define _ots_ROCCalorimeterEmulator_h_

#include "otsdaq-mu2e/FEInterfaces/ROCCoreVEmulator.h"


namespace ots
{


class ROCCalorimeterEmulator : public ROCCoreVEmulator
{

public:
	ROCCalorimeterEmulator (const std::string& rocUID,
			const ConfigurationTree& theXDAQContextConfigTree,
			const std::string& interfaceConfigurationPath);

	~ROCCalorimeterEmulator(void);

	void 					writeRegister		(unsigned address, unsigned data_to_write);

	int 					readRegister		(unsigned address);

	bool 					emulatorWorkLoop	(void) override;

	enum{
		ADDRESS_FIRMWARE_VERSION = 5,
		ADDRESS_MYREGISTER = 0x65,
	};

	//	temperature--
	class Thermometer
	{
	private:
		double mnoiseTemp;
	public:
		void noiseTemp(double intemp)
		{
			mnoiseTemp = (double) intemp + 0.05*(intemp*((double) rand() / (RAND_MAX))-0.5);
			return;
		}
		double GetBoardTempC()
		{
			return mnoiseTemp;
		}
	};


	Thermometer 	temp1_;
	double 			inputTemp_;

};

}

#endif
