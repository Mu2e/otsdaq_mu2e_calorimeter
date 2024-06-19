#include "otsdaq-mu2e-calorimeter/FEInterfaces/ROCCalorimeterInterface.h"

#include "otsdaq/Macros/InterfacePluginMacros.h"
#include <fstream>

using namespace ots;

#undef __MF_SUBJECT__
#define __MF_SUBJECT__ "FE-ROCCalorimeterInterface"

// 259 (and others) ==> the number of words in block read is written first as a block write
const std::set<DTCLib::roc_address_t>		ROCCalorimeterInterface::SPECIAL_BLOCK_READ_ADDRS_({263, 256, 257, 261, 262});

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

	registerFEMacroFunction("Read ROC Error Counter",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCCalorimeterInterface::ReadROCErrorCounter),
	                        std::vector<std::string>{"Address to read, Default := 0]"}, //inputs parameters
	                        std::vector<std::string>{"Status"}, //output parameters
	                        1);  // requiredUserPermissions							

	registerFEMacroFunction("Setup for ADCs Data Taking",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCCalorimeterInterface::SetupForADCsDataTaking),
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

	// Scarsi prova
	registerFEMacroFunction("SCARSI TEST FUNCTION",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCCalorimeterInterface::ScarsiTest),
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
uint16_t ROCCalorimeterInterface::readEmulatorRegister(uint16_t address) //not useful, broken by Luca
{
	__CFG_COUT__ << "emulator read" << __E__;

	if(address == 6 || address == 7)
		return ROCPolarFireCoreInterface::readEmulatorRegister(address);
	if(address == ROC_ADDRESS_EW_LENGHT)
		return 0x5;
	else if(address == ROC_ADDRESS_EW_BLIND)
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
void ROCCalorimeterInterface::universalBlockRead(char*        address,
                                                 char*        returnValue,
                                                 unsigned int numberOfBytes)
{
	std::vector<DTCLib::roc_data_t> data;
	readROCBlock(data,
	             *((DTCLib::roc_address_t*)address),
	             numberOfBytes / 2,
	             false /*incAddress*/);
	if(data.size() != numberOfBytes/2)	{
		__FE_SS__ << "Illegal number of bytes: "  <<  data.size() << " not " << numberOfBytes/2 << __E__;
		__FE_SS_THROW__;
	}
    memcpy(returnValue,&data[0],data.size()*2);
	
}

//==================================================================================================
void ROCCalorimeterInterface::readROCBlock(std::vector<DTCLib::roc_data_t>& data,
                                           DTCLib::roc_address_t            address,
                                           uint16_t                   wordCount,
                                           bool incrementAddress)
{
	__FE_COUT__ << "Calling read ROC block: link number " << std::dec << linkID_
	            << ", address = " << address << ", wordCount = " << wordCount
	            << ", incrementAddress = " << incrementAddress << __E__;

	uint16_t u;
	u = thisDTC_->ReadROCRegister(linkID_, 0, 100);

	// check if special Block Write required
	if(ROCCalorimeterInterface::SPECIAL_BLOCK_READ_ADDRS_.find(address) !=
	   ROCCalorimeterInterface::SPECIAL_BLOCK_READ_ADDRS_.end())
	{
		__FE_COUT__ << "Doing special block write!" << __E__;
                
                switch (address){

                  case 261:
                    writeROCBlock({wordCount, 0}, address, false /* incrementAddress*/);
                    break;                

                  case 257:
                    writeROCBlock({wordCount}, address, false /* incrementAddress*/);
                    break;

                  case 263:
                    writeROCBlock({wordCount}, address, false /* incrementAddress*/);
                    break;    

                  default:             
                    writeROCBlock({wordCount}, address, false /* incrementAddress*/);
                    break;                    

                }


		//uint16_t j = 0;
		while((u = thisDTC_->ReadROCRegister(linkID_, 128, 100)) == 0)
		{
/*			usleep(100);
			j++;
			if(j == 100)
			{
				__FE_SS__ << "ROC block failed at 128"  << __E__;
				__FE_SS_THROW__;
			}*/
		}  // when the write operation ends the micropro
		   // cessor writes 0x8000 to register 0x128
	 	__COUT__ << "r_128: 0x" << std::hex << u << __E__;
		usleep(1000);
		
        //j = 0; 
		while((u = thisDTC_->ReadROCRegister(linkID_, 129, 100)) == 0)
		{
			/*usleep(100);
			j++;
			if(j == 100)
			{
				__FE_SS__ << "ROC block failed at 129"  << __E__;
				__FE_SS_THROW__;
			}*/
		}

	 	__COUT__ << "r_129: 0x" << std::hex << u << __E__;

		//wordCount = u - 4;  // number of words to read back
	}
	__FE_COUTV__(data.size());
	__FE_COUTV__(wordCount);
	__FE_COUTV__(u-4);
	thisDTC_->ReadROCBlock(data, linkID_, address, u-4, incrementAddress, 0);
	__FE_COUTV__(data.size());
	//only fix data if received more than needed - TODO fix in ROC firmware
	while(data.size() > wordCount) data.pop_back();


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
bool ROCCalorimeterInterface::running(void)
{

  //SetupForPatternFixedLengthDataTaking(40);
  SetupForADCsDataTaking(40);

  return false;
  
}


//==================================================================================================
void ROCCalorimeterInterface::Configure(__ARGS__)
{
	__MOUT_INFO__ << "Configure called" << __E__;
	configure();
}

//==================================================================================================
void ROCCalorimeterInterface::ScarsiTest(__ARGS__)
{
	__MOUT_INFO__ << "prova 1" << __E__;
	// __FE_COUTV__ << "prova 2" << __E__;
	__FE_COUT__ << "prova 3" << __E__;
	__COUT__ << "prova 4" << __E__;
	__FE_SS__ << "prova 5" << __E__;
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
int ROCCalorimeterInterface::GetTemperature(int idchannel) //wrong address
{
	return readRegister(ROC_ADDRESS_EW_LENGHT);
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
	
unsigned int numberOfWords = __GET_ARG_IN__("Fixed Length of Event [units of 16-bit words, Default := 8]", uint32_t, 8);

SetupForPatternFixedLengthDataTaking(numberOfWords);




} //end SetupForPatternFixedLengthDataTaking()


//==================================================================================================
void ROCCalorimeterInterface::SetupForPatternFixedLengthDataTaking(unsigned int numberOfWords)
{
	__COUT_INFO__ << "SetupForPatternFixedLengthDataTaking()" << __E__;
	writeRegister(ROC_ADDRESS_DDRRESET, 1); 
	writeRegister(ROC_ADDRESS_ANALOGRESET, 1); 
	writeRegister(ROC_ADDRESS_ANALOGRESET, 0); 


	writeRegister(ROC_ADDRESS_IS_PATTERN, 0);

	writeRegister(ROC_ADDRESS_IS_COUNTER, 1);
	writeRegister(ROC_ADDRESS_COUNTER_IS_FALLING, 1);

	writeRegister(ROC_ADDRESS_EW_LENGHT, 1500);


	__FE_COUTV__(numberOfWords);
	writeRegister(ROC_ADDRESS_COUNTER_SIZE, numberOfWords);

	__COUT_INFO__ << "end SetupForPatternFixedLengthDataTaking()" << __E__;
} //end SetupForPatternFixedLengthDataTaking()



//==================================================================================================



//==================================================================================================
void ROCCalorimeterInterface::ReadROCErrorCounter(__ARGS__)
{
	__COUT_INFO__ << "ReadROCErrorCounter()" << __E__;


	unsigned int errAddr = __GET_ARG_IN__("Address to read, Default := 0]", uint16_t, 0);
	__FE_COUTV__(errAddr);

	writeRegister(ROC_ADDRESS_ERRCNT, errAddr);
	writeRegister(ROC_ADDRESS_IS_PATTERN, 64);

	std::stringstream os;
	DTCLib::roc_data_t readVal;
    readVal = readRegister(ROC_ADDRESS_ERRCNT);

	os << std::hex << std::setprecision(4) << std::setfill('0') <<
		"address 0x" << errAddr << " (" << std::dec << errAddr << 
		std::hex << "): data 0x" << readVal << " (" << std::dec << 
		readVal << ")\n" << __E__;


	writeRegister(ROC_ADDRESS_IS_PATTERN, 0);


	__COUT_INFO__ << "end ReadROCErrorCounter()" << __E__;

	__SET_ARG_OUT__("Status",os.str());


} //end ReadROCErrorCounter()


//==================================================================================================




void ROCCalorimeterInterface::SetupForADCsDataTaking(__ARGS__)
{
	unsigned int numberOfWords = __GET_ARG_IN__("Fixed Length of Event [units of 16-bit words, Default := 8]", uint32_t, 8);

    SetupForADCsDataTaking(numberOfWords);


} //end SetupForADCsDataTaking()

//==================================================================================================




void ROCCalorimeterInterface::SetupForADCsDataTaking(unsigned int numberOfWords)
{
	__COUT_INFO__ << "SetupForADCsDataTaking()" << __E__;

	__FE_COUTV__(numberOfWords);

	std::string filename = std::string(__ENV__("USER_DATA")) + "/roc_thr.csv";
	std::ifstream myFile(filename);

	// Create a vector of <string, int vector> pairs to store the result
    std::vector<std::pair<std::string, std::vector<int>>> result;

	if(!myFile.is_open())
	{
		__FE_SS__ << "Could not open file: " << filename << __E__;
		__FE_SS_THROW__;;
	}

	// Read myFile
	std::vector<std::vector<std::string>> csvRows;

	for (std::string line; std::getline(myFile, line);) {

		std::istringstream ss(std::move(line));
		std::vector<std::string> row;

		if (!csvRows.empty())
		{
			// We expect each row to be as big as the first row
			row.reserve(csvRows.front().size());
		}

		// std::getline can split on other characters, here we use ','
		for (std::string value; std::getline(ss, value, ',');)
		{
			row.push_back(std::move(value));
		}

		csvRows.push_back(std::move(row));
	}

    // Close file
    myFile.close();

	writeRegister(ROC_ADDRESS_DDRRESET,  1); 
	writeRegister(ROC_ADDRESS_IS_COUNTER, 0); 
	writeRegister(ROC_ADDRESS_IS_LASER,   0); 

    writeRegister(ROC_ADDRESS_EW_DELAY,   0); 
	writeRegister(ROC_ADDRESS_EW_BLIND,   10); 


	//Write Roc thrsholds using 
	for(int ich=0; ich<20; ich++)
	{
	  writeRegister(ROC_ADDRESS_BASE_THRESHOLD + ich, std::stoi(csvRows[0][ich+1])); 
	}

	for (const std::vector<std::string>& row : csvRows)
	{
    	for (const std::string& value : row)
		{
    		__COUT_INFO__ << std::setw(10) << value;
    	}

		__COUT_INFO__ << "\n";
	}

	__COUT_INFO__ << "end SetupForADCsDataTaking()" << __E__;
} //end SetupForADCsDataTaking()



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


	address = 72;
	readVal = readRegister(address);
	os << std::hex << std::setprecision(4) << std::setfill('0') <<
		"address 0x" << address << " (" << std::dec << address << 
		std::hex << "): hbtag error 0x" << readVal << " (" << std::dec << 
		readVal << ") \n" << __E__;


	address = 73;
	readVal = readRegister(address);
	os << std::hex << std::setprecision(4) << std::setfill('0') <<
		"address 0x" << address << " (" << std::dec << address << 
		std::hex << "): dreq error 0x" << readVal << " (" << std::dec << 
		readVal << ") \n" << __E__;		


	address = 74;
	readVal = readRegister(address);
	os << std::hex << std::setprecision(4) << std::setfill('0') <<
		"address 0x" << address << " (" << std::dec << address << 
		std::hex << "): hblost 0x" << readVal << " (" << std::dec << 
		readVal << ") \n" << __E__;

	address = 75;
	readVal = readRegister(address);
	os << std::hex << std::setprecision(4) << std::setfill('0') <<
		"address 0x" << address << " (" << std::dec << address << 
		std::hex << "): evm lost 0x" << readVal << " (" << std::dec << 
		readVal << ") \n" << __E__;		

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
