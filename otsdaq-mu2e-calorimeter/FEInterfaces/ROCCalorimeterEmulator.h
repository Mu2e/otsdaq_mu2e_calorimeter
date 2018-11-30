#ifndef _ots_ROCCalorimeterEmulator_h_
#define _ots_ROCCalorimeterEmulator_h_

#include "otsdaq-mu2e/FEInterfaces/ROCInterface.h"


namespace ots
{
	

class ROCCalorimeterEmulator : public ROCInterface
{
  
 public:
  //ROCCalorimeterEmulator (const unsigned int linkID, DTCLib::DTC* thisDTC, const unsigned int delay, const ConfigurationTree& theXDAQContextConfigTree, const std::string& theConfigurationPath);
  
  //~ROCCalorimeterEmulator(void);
	

	void writeRegister(unsigned address, unsigned data_to_write);

	int readRegister(unsigned address);
	
	enum{
		ADDRESS_FIRMWARE_VERSION = 5,
		ADDRESS_MYREGISTER = 0x65,
	};
    int temperature[5];
}
};

}

#endif
