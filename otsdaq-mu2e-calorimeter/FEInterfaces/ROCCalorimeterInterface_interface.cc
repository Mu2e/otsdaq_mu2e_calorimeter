#include "otsdaq-mu2e-calorimeter/FEInterfaces/ROCCalorimeterInterface.h"

#include "otsdaq-core/Macros/InterfacePluginMacros.h"

using namespace ots;

#undef __MF_SUBJECT__
#define __MF_SUBJECT__ "FE-ROCCalorimeterInterface"

//=========================================================================================
ROCCalorimeterInterface::ROCCalorimeterInterface(
    const std::string&       rocUID,
    const ConfigurationTree& theXDAQContextConfigTree,
    const std::string&       theConfigurationPath)
    : ROCPolarFireCoreInterface(rocUID, theXDAQContextConfigTree, theConfigurationPath)
{
	INIT_MF("ROCCalorimeterInterface");

	__CFG_COUT__ << "Constructor..." << __E__;

	try
	{
		inputTemp_ = getSelfNode().getNode("inputTemperature").getValue<double>();
	}
	catch(...)
	{
		__CFG_COUT__ << "inputTemperature field not defined. Defaulting..." << __E__;
		inputTemp_ = 15.;
	}

	temp1_.noiseTemp(inputTemp_);
}

//==========================================================================================
ROCCalorimeterInterface::~ROCCalorimeterInterface(void) {}

//==================================================================================================
void ROCCalorimeterInterface::writeEmulatorRegister(unsigned address,
                                                    unsigned data_to_write)
{
	// lockout member variables for the remainder of the scope
	// this guarantees the emulator thread can safely access the members
	//	Note: the ROCCoreVEmulator locks before calling emulatorWorkLoop
	std::lock_guard<std::mutex> lock(workloopMutex_);

	__CFG_COUT__ << "emulator write" << __E__;

	return;

}  // end writeRegister()

//==================================================================================================
int ROCCalorimeterInterface::readEmulatorRegister(unsigned address)
{
	// lockout member variables for the remainder of the scope
	// this guarantees the emulator thread can safely access the members
	//	Note: the ROCCoreVEmulator locks before calling emulatorWorkLoop
	std::lock_guard<std::mutex> lock(workloopMutex_);

	__CFG_COUT__ << "emulator read" << __E__;

	if(address == 6 || address == 7)
		return ROCPolarFireCoreInterface::readEmulatorRegister(address);
	if(address == ADDRESS_FIRMWARE_VERSION)
		return 0x5;
	else if(address == ADDRESS_MYREGISTER)
		return temp1_.GetBoardTempC();
	else
		return -1;

}  // end readRegister()

//==================================================================================================
// return false to stop workloop thread
bool ROCCalorimeterInterface::emulatorWorkLoop(void)
{
	__CFG_COUT__ << "emulator working..." << __E__;

	temp1_.noiseTemp(inputTemp_);

	usleep(1000000 /*microseconds*/);
	return true;  // true to keep workloop going

	//	float input, inputTemp;
	//	int addBoard, a;
	//	//
	//	addBoard = 105;
	//	inputTemp = 15.;
	//	a = 0;
	//	while( a < 20 ) {
	//		temp1.noiseTemp(inputTemp);
	//		temperature = temp1.GetBoardTempC();
	//		a++;
	//		return temperature;
	//		usleep(1000000);
	//		return true;
	//	}
}  // end emulatorWorkLoop()

DEFINE_OTS_INTERFACE(ROCCalorimeterInterface)
