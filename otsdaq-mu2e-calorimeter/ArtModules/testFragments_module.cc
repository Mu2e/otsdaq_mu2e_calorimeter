///////////////////////////////////////////////////////////////////////////////////////
// Class:       testFragments
// Module Type: EDAnalyzer
// File:        testFragments_module.cc
// Description: Prints out mu2eFragments in HWUG Packet format (see mu2e-docdb #4097)
///////////////////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"

#include "canvas/Utilities/Exception.h"

#include "artdaq-core/Data/Fragment.hh"

#include "trace.h"

#include <unistd.h>
#include <iomanip>
#include <iostream>
#include <string>

namespace mu2e {
class testFragments;
}

class mu2e::testFragments : public art::EDAnalyzer
{
public:
	explicit testFragments(fhicl::ParameterSet const& pset);
	virtual void analyze(art::Event const& evt);

private:
	art::InputTag caloFragmentsTag_;
};

mu2e::testFragments::testFragments(fhicl::ParameterSet const& pset)
	: EDAnalyzer{pset}, caloFragmentsTag_{"calo"} {}

void mu2e::testFragments::analyze(art::Event const& e)
{
	std::cout<<" [testFragments] analyzer testing simulated events "<<std::endl;	
	std::vector<art::Handle<artdaq::Fragments>> vah = e.getMany<artdaq::Fragments>();
	//auto calFragments = e.getValidHandle<artdaq::Fragments>("daq");

	std::cout<<"[testFragments] size "<<vah.size()<<std::endl;
	for (auto const& calFragments : vah) {
		const art::Provenance* prov = calFragments.provenance();

		std::string fcn = prov->friendlyClassName();
		std::string modn = prov->moduleLabel();
		std::string instn = prov->processName();

		std::string name = fcn + "_" + prov->moduleLabel() + "_" + instn;
		std::cout<<"[testFragments]  extracting name =  "<<fcn<<" "<<modn<<" "<<instn<<std::endl;  //artdaq::Fragments daq test001
		//std::cout<<"with type =  "<<typeid(prov).name()<<std::endl;
		size_t totalSize = 0;
	
		for (size_t idx = 0; idx < calFragments->size(); ++idx) {
			auto size = ((*calFragments)[idx]).size() * sizeof(artdaq::RawDataType);
			totalSize += size;
			std::cout << "[testFragments]  \tCAL Fragment " << idx << " has size " << size << std::endl;
		}
		
		std::cout << "[testFragments]  \tTotal Size: " << (int)totalSize << " bytes." << std::endl;
	}
	/*std::cout << "Event " << e.event() << " has ";
	
	std::cout << calFragments->size() << " CAL fragments." << std::endl;

	size_t totalSize = 0;
	
	for (size_t idx = 0; idx < calFragments->size(); ++idx) {
		auto size = ((*calFragments)[idx]).size() * sizeof(artdaq::RawDataType);
		totalSize += size;
		std::cout << "\tCAL Fragment " << idx << " has size " << size << std::endl;
	}
	
	std::cout << "\tTotal Size: " << (int)totalSize << " bytes." << std::endl;*/
}

DEFINE_ART_MODULE(mu2e::testFragments)
