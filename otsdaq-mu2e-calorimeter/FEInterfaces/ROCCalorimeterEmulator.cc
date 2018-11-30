#include "otsdaq-mu2e-calorimeter/FEInterfaces/ROCCalorimeterEmulator.h"
#include "otsdaq-core/Macros/CoutMacros.h"


using namespace ots;

#undef 	__MF_SUBJECT__
#define __MF_SUBJECT__ getIdString()

//=========================================================================================
//ROCCalorimeterEmulator::ROCCalorimeterEmulator(const unsigned int linkID,
//			   DTCLib::DTC* thisDTC, 
//			   const unsigned int delay,
//			   const ConfigurationTree& theXDAQContextConfigTree,
//			   const std::string& theConfigurationPath)
//: Configurable(theXDAQContextConfigTree,theConfigurationPath)
/*{
	INIT_MF("ROCCalorimeterEmulator");

	linkID_  = DTCLib::DTC_Link_ID(linkID);  
	thisDTC_ = thisDTC;	
	delay_   = delay;
	__COUTV__(delay_);
	
//	try
//	{
//		delay_ = getSelfNode().getNode("DelayOffset").getValue<unsigned int>();
//	}
//	catch(...)
//	{
//		__COUT__ << "Field does not exist?" << __E__;
//	}

	__MCOUT_INFO__("ROCCalorimeterEmulator instantiated with link: " << linkID_ << __E__);

}

//==========================================================================================
ROCCalorimeterEmulator::~ROCCalorimeterEmulator(void)
{
}*/

//==================================================================================================
void ROCCalorimeterEmulator::writeRegister(unsigned address, unsigned data_to_write) {

  __COUT__ << "emulator write" << __E__;

  return;

}

//==================================================================================================
int ROCCalorimeterEmulator::readRegister(unsigned address) {
	

	__COUT__ << "emulator read" << __E__;

  if (address == ADDRESS_FIRMWARE_VERSION)
  	return 0x5;
  else if (address == ADDRESS_TEMPERATURE0)
 	return temperature[0];
  else
 	return -1;

}

//==================================================================================================

void DTCFrontEndInterface::emulatorWorkLoop(void)

{	
//	while(1)

//	temperature--
class Thermometer{
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

Thermometer temp1;
     
   float input, inputTemp;
   int addBoard, a;
// 
   addBoard = 105;
   inputTemp = 15.;
   a = 0;
   while( a < 20 ) {
      temp1.noiseTemp(inputTemp);
      temperature = temp1.GetBoardTempC();
      a++;
      return temperature;
      usleep(1000000);
   }
}

