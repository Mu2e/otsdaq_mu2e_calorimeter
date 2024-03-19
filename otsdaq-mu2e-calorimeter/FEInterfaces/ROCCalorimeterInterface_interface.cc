#include "otsdaq-mu2e-calorimeter/FEInterfaces/ROCCalorimeterInterface.h"

#include "otsdaq/Macros/InterfacePluginMacros.h"

using namespace ots;

#undef __MF_SUBJECT__
#define __MF_SUBJECT__ "FE-ROCCalorimeterInterface"

// 259 (and others) ==> the number of words in block read is written first as a block write
const std::set<DTCLib::roc_address_t>		ROCCalorimeterInterface::SPECIAL_BLOCK_READ_ADDRS_({263});

//=========================================================================================
ROCCalorimeterInterface::ROCCalorimeterInterface(
    const std::string&       rocUID,
    const ConfigurationTree& theXDAQContextConfigTree,
    const std::string&       theConfigurationPath)
    : ROCPolarFireCoreInterface(rocUID, theXDAQContextConfigTree, theConfigurationPath)
{
	INIT_MF("." /*directory used is USER_DATA/LOG/.*/);

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
	

	// Moved to ROCPolarFireCoreInterface::ROCPolarFireCoreInterface
	// registerFEMacroFunction("Setup for Pattern Data Taking", //Moved to otsdaq_mu2e/otsdaq-mu2e/FEInterfaces/ROCPolarFireCoreInterfaceImpl.cc
	//                         static_cast<FEVInterface::frontEndMacroFunction_t>(
	//                             &ROCCalorimeterInterface::SetupForPatternDataTaking),
	//                         std::vector<std::string>{}, //inputs parameters
	//                         std::vector<std::string>{}, //output parameters
	//                         1);  // requiredUserPermissions

	registerFEMacroFunction("Setup for Fixed-length Pattern Data Taking",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCCalorimeterInterface::SetupForPatternFixedLengthDataTaking),
	                        std::vector<std::string>{"Fixed Length of Event [units of 16-bit words, Default := 8]"}, //inputs parameters
	                        std::vector<std::string>{}, //output parameters
	                        1);  // requiredUserPermissions


	registerFEMacroFunction("ROC Status",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCCalorimeterInterface::GetStatus),
	                        std::vector<std::string>{}, //inputs parameters
	                        std::vector<std::string>{"Status"}, //output parameters
	                        1);  // requiredUserPermissions

	
	// registerFEMacroFunction("SetROCCaloVoltageChannel",
	//                         static_cast<FEVInterface::frontEndMacroFunction_t>(
	//                             &ROCCalorimeterInterface::SetVoltageChannel),
	//                         std::vector<std::string>{"channelNumber", "value"}, //inputs parameters
	//                         std::vector<std::string>{}, //output parameters
	//                         1);  // requiredUserPermissions
	       

	
	// registerFEMacroFunction("GetROCCaloVoltageChannel",
	//                         static_cast<FEVInterface::frontEndMacroFunction_t>(
	//                             &ROCCalorimeterInterface::GetVoltageChannel),
	//                         std::vector<std::string>{"channelNumber"}, //inputs parameters
	//                         std::vector<std::string>{"readValue"}, //output parameters
	//                         1);  // requiredUserPermissions
	
	registerFEMacroFunction("Configure State Machine",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCCalorimeterInterface::Configure),
	                        std::vector<std::string>{}, //inputs parameters
	                        std::vector<std::string>{}, //output parameters
	                        1);  // requiredUserPermissions

// // function for webgui
// 	registerFEMacroFunction("GetROCCaloTemperatureChannel",
// 	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
// 	                            &ROCCalorimeterInterface::GetTempChannel),
// 	                        std::vector<std::string>{"channelNumber"}, //inputs parameters
// 	                        std::vector<std::string>{"readValue"}, //output parameters
// 	                        1);  // requiredUserPermissions

} //end constructor()

//==========================================================================================
ROCCalorimeterInterface::~ROCCalorimeterInterface(void) {
  // NOTE:: be careful not to call __FE_COUT__ decoration because it uses the
  // tree and it may already be destructed partially
  __COUT__ << FEVInterface::interfaceUID_ << "Destructed." << __E__;
} //end destructor()

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
	{	
		temp1_.noiseTemp(temp1_.GetBoardTempC());
		return temp1_.GetBoardTempC()*256;	
	}
	else
		return 0xBAAD;

}  // end readRegister()

//==================================================================================================
// return false to stop workloop thread
bool ROCCalorimeterInterface::emulatorWorkLoop(void)
{
	__COUT__ << FEVInterface::interfaceUID_ << "emulator working..." << __E__;

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
void ROCCalorimeterInterface::readROCBlock(std::vector<DTCLib::roc_data_t>& data,
                                  DTCLib::roc_address_t            address,
                                  uint16_t                         wordCount,
                                  bool                             incrementAddress)
{
	__FE_COUT__ << "Calling read ROC block: link number " << std::dec << linkID_
	            << ", address = " << address << ", wordCount = " << wordCount
	            << ", incrementAddress = " << incrementAddress << __E__;

	//check if special Block Write required
	if(ROCCalorimeterInterface::SPECIAL_BLOCK_READ_ADDRS_.find(address) != ROCCalorimeterInterface::SPECIAL_BLOCK_READ_ADDRS_.end())
	{
		__FE_COUT__ << "Doing special block write!" << __E__;
		writeROCBlock({wordCount}, address, false /* incrementAddress*/);
	}
	__FE_COUTV__(data.size());
	thisDTC_->ReadROCBlock(data, linkID_, address, wordCount, incrementAddress, 0);
	__FE_COUTV__(data.size());

	if(data.size() != wordCount)
	{
		__FE_SS__ << "ROC block read failed, expecting " << wordCount 
			<< " words, and read " << data.size() << " words." << __E__;
		__FE_SS_THROW__;		
	}

}  // end readBlock()

//==================================================================================================
void ROCCalorimeterInterface::configure(void) try
{
	ROCPolarFireCoreInterface::configure();

    // consider that we know all the init files
    // all the init information are stored in the configuration tree    // 
    // set parameter 
    // int linkID = getSelfNode().getNode("linkID").getValue<int>();
    //	__MOUTV__(linkID);

	runSequenceOfCommands("ROCTypeLinkTable/LinkToConfigureSequence"); /*Run Configure Sequence Commands*/
    
//    int myTemp = GetTemperature(1);
// 	__MOUTV__(myTemp);
// 	__MOUT__<<"my temp"<<myTemp<<__E__;
//     __COUTV__(myTemp);
//     __MCOUTV__(myTemp);
}
catch(const std::runtime_error& e)
{
	__FE_MOUT__ << "Error caught: " << e.what() << __E__;
	throw;
}
catch(...)
{
	__FE_SS__ << "Unknown error caught. Check printouts!" << __E__;
	try	{ throw; } //one more try to printout extra info
	catch(const std::exception &e)
	{
		ss << "Exception message: " << e.what();
	}
	catch(...){}
	__FE_SS_THROW__;
}

//==================================================================================================
void ROCCalorimeterInterface::Configure(__ARGS__)
{
	__MOUT_INFO__ << "Configure called" << __E__;
	configure();
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
//==================================================================================================
void ROCCalorimeterInterface::GetTempChannel(__ARGS__)
{
	__MOUT_INFO__ << "Temp is" << __E__;
    int channelnumber = __GET_ARG_IN__("channelNumber",int);
	__SET_ARG_OUT__("readValue",GetTemperature(channelnumber));
}

//==================================================================================================
int ROCCalorimeterInterface::GetTemperature(int idchannel)
{
	return readRegister(ADDRESS_MYREGISTER);
}

// //==================================================================================================
	// Moved to ROCPolarFireCoreInterface::ROCPolarFireCoreInterface
	// Moved to otsdaq_mu2e/otsdaq-mu2e/FEInterfaces/ROCPolarFireCoreInterfaceImpl.cc
// void ROCCalorimeterInterface::SetupForPatternDataTaking(__ARGS__)
// {
// 	__COUT_INFO__ << "SetupForPatternDataTaking()" << __E__;

// 	//For future, to get link ID of this ROC:
// 	__FE_COUTV__(getLinkID());

// 	writeRegister(14,1);  //ROC reset
// 	writeRegister(8,1 << 4);
// 	writeRegister(30,0);
// 	writeRegister(29,1);

// 	__COUT_INFO__ << "end SetupForPatternDataTaking()" << __E__;


// 	// __SET_ARG_OUT__("readValue",GetTemperature(channelnumber));
// } //end SetupForPatternDataTaking()


//==================================================================================================
void ROCCalorimeterInterface::SetupForPatternFixedLengthDataTaking(__ARGS__)
{
	__COUT_INFO__ << "SetupForPatternFixedLengthDataTaking()" << __E__;
	writeRegister(14,1); //ROC reset
	writeRegister(30,0);

	writeRegister(79,1);

	unsigned int numberOfWords = __GET_ARG_IN__("Fixed Length of Event [units of 16-bit words, Default := 8]", uint32_t, 8);
	__FE_COUTV__(numberOfWords);
	writeRegister(81,numberOfWords);

	__COUT_INFO__ << "end SetupForPatternFixedLengthDataTaking()" << __E__;
} //end SetupForPatternFixedLengthDataTaking()

//==================================================================================================
void ROCCalorimeterInterface::GetStatus(__ARGS__)
{
	//copied from Monica's va_read_all.sh

	DTCLib::roc_data_t readVal;
	
	std::stringstream os;
	DTCLib::roc_address_t address;

	address = 0x0;
	readVal = readRegister(address);
	os << std::hex << std::setprecision(4) << std::setfill('0') <<
		"address 0x" << address << " (" << std::dec << address << 
		std::hex << "): data 0x" << readVal << " (" << std::dec << 
		readVal << ")\n" << __E__;

	
	address = 0x8;
	readVal = readRegister(address);
	os << std::hex << std::setprecision(4) << std::setfill('0') <<
		"address 0x" << address << " (" << std::dec << address << 
		std::hex << "): data 0x" << readVal << " (" << std::dec << 
		readVal << ")" << __E__;
	os << "\t\t" << "bit[9:8]=[enable_marker,enable_clock]"
			"\n\t\t bit[7:4]=[en_int_ewm,en_free_ewm,error_en,pattern_en]"
			"\n\t\t bit[3:0]=en_lanes[HV1,HV0,CAl1,CAL0]\n" << __E__;


	address = 18;
	readVal = readRegister(address);
	os << std::hex << std::setprecision(4) << std::setfill('0') <<
		"address 0x" << address << " (" << std::dec << address << 
		std::hex << "): data 0x" << readVal << " (" << std::dec << 
		readVal << ")" << __E__;
	os << "\t\t" << "bit[9:8]=[enable_marker,enable_clock]"
			"\n\t\t bit[7:4]=[en_int_ewm,en_free_ewm,error_en,pattern_en]"
			"\n\t\t bit[3:0]=en_lanes[HV1,HV0,CAl1,CAL0]\n" << __E__;

	uint32_t doubleRegVal = 0;


	std::vector<DTCLib::roc_address_t> doubleReads = {
		23,
		25,
		64,
		27,
		29,
		31,
		33,
		35,
		37,
		39,
		41,
		43,
		45,
		48,
		51,
		54,
		57
	};

	std::vector<std::string> doubleReadCaptions = {
		"SIZE_FIFO_FULL[28]+STORE_POS[25:24]+STORE_CNT[19:0]", 	//23,
		"SIZE_FIFO_EMPTY[28]+FETCH_POS[25:24]+FETCH_CNT[19:0]", //25,
		"no. EVM seen", //64,
		"no. HB seen", //27,
		"no. null HB seen:", //29,
		"no. HB on hold", //31,
		"no. PREFETCH seen", //33,
		"no. DATA REQ seen", //35,
		"no. DATA REQ read from DDR", //37,
		"no. DATA REQ sent to DTC", //39,
		"no. DATA REQ with null data", //41,
		"last SPILL TAG", //43,
		"last HB tag", //45,
		"last PREFETCH tag", //48,
		"last FETCHED tag", //51,
		"last DATA REQ tag", //54,
		"OFFSET tag", //57
	};

	for(size_t i=0; i<doubleReads.size(); ++i)
	{
		address = doubleReads[i];
		readVal = readRegister(address);
		doubleRegVal = readVal;	
		readVal = readRegister(++address);
		doubleRegVal |= readVal << 16;

		os << std::hex << std::setprecision(4) << std::setfill('0') <<
			"address 0x" << address-1 << " (" << std::dec << address-1 << 
			std::setprecision(8) <<
			std::hex << "): data 0x" << doubleRegVal << " (" << std::dec << 
			doubleRegVal << ")" << __E__;
		os << "\t\t" << doubleReadCaptions[i] << "\n" << __E__;
	} //end double read register loop


	__SET_ARG_OUT__("Status",os.str());

} //end GetStatus()

DEFINE_OTS_INTERFACE(ROCCalorimeterInterface)
