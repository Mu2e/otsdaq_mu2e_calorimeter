#include "otsdaq-mu2e-calorimeter/FEInterfaces/ROCCalorimeterInterface.h"

#include "otsdaq/Macros/InterfacePluginMacros.h"

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

	__MCOUT_INFO__("ROCPolarFireCoreInterface instantiated with link: "
	               << linkID_ << " and EventWindowDelayOffset = " << delay_ << __E__);

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

	registerFEMacroFunction(
	    "SetROCCaloVoltageChannel",
	    static_cast<FEVInterface::frontEndMacroFunction_t>(
	        &ROCCalorimeterInterface::SetVoltageChannel),
	    std::vector<std::string>{"channelNumber", "value"},  // inputs parameters
	    std::vector<std::string>{},                          // output parameters
	    1);                                                  // requiredUserPermissions

	registerFEMacroFunction(
	    "GetROCCaloVoltageChannel",
	    static_cast<FEVInterface::frontEndMacroFunction_t>(
	        &ROCCalorimeterInterface::GetVoltageChannel),
	    std::vector<std::string>{"channelNumber"},  // inputs parameters
	    std::vector<std::string>{"readValue"},      // output parameters
	    1);                                         // requiredUserPermissions
}

//==========================================================================================
ROCCalorimeterInterface::~ROCCalorimeterInterface(void) {}

//==================================================================================================
void ROCCalorimeterInterface::writeEmulatorRegister(uint16_t address,
                                                    uint16_t data_to_write)
{
	__CFG_COUT__ << "emulator write" << __E__;

	return;

}  // end writeRegister()

//==================================================================================================
uint16_t ROCCalorimeterInterface::readEmulatorRegister(uint16_t address)
{
	__CFG_COUT__ << "emulator read" << __E__;

	if(address == 6 || address == 7)
		return ROCPolarFireCoreInterface::readEmulatorRegister(address);
	if(address == ADDRESS_FIRMWARE_VERSION)
		return 0x5;
	else if(address == ADDRESS_MYREGISTER)
		return temp1_.GetBoardTempC();
	else
		return 0xBAAD;

}  // end readRegister()

//==================================================================================================
// return false to stop workloop thread
bool ROCCalorimeterInterface::emulatorWorkLoop(void)
{
	//__CFG_COUT__ << "emulator working..." << __E__;

	temp1_.noiseTemp(inputTemp_);
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

//==================================================================================================
void ROCCalorimeterInterface::configure(void) try
{
	ROCPolarFireCoreInterface::configure();
}
catch(const std::runtime_error& e)
{
	__FE_MOUT__ << "Error caught: " << e.what() << __E__;
	throw;
}
catch(...)
{
	__FE_SS__ << "Unknown error caught. Check printouts!" << __E__;
	__FE_MOUT__ << ss.str();
	__FE_SS_THROW__;
}

//==================================================================================================
void ROCCalorimeterInterface::SetVoltageChannel(__ARGS__)
{
	__MOUT_INFO__ << "Set called" << __E__;
}

//==================================================================================================
void ROCCalorimeterInterface::GetVoltageChannel(__ARGS__)
{
	__MOUT_INFO__ << "Get called" << __E__;
	__SET_ARG_OUT__("readValue", 12);
}

DEFINE_OTS_INTERFACE(ROCCalorimeterInterface)
