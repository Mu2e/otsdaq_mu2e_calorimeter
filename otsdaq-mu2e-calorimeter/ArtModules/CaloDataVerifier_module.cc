#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "TRACE/tracemf.h"
#include "artdaq/DAQdata/Globals.hh"
#define TRACE_NAME "CaloDataVerifier"

#include "canvas/Utilities/InputTag.h"

#include <artdaq-core/Data/ContainerFragment.hh>
#include "artdaq-core/Data/Fragment.hh"

#include "artdaq-core-mu2e/Overlays/DTCEventFragment.hh"
#include "artdaq-core-mu2e/Overlays/FragmentType.hh"
#include "artdaq-core-mu2e/Data/EventHeader.hh"
#include "artdaq-core-mu2e/Data/CalorimeterTestDataDecoder.hh"

#include "cetlib_except/exception.h"

#include <iomanip>
#include <sstream>

namespace mu2e {
  class CaloDataVerifier : public art::EDFilter
  {
  public:
    struct Config {
      fhicl::Atom<int>  diagLevel     {fhicl::Name("diagLevel")     , fhicl::Comment("diagnostic level")};
      fhicl::Atom<int>  metrics_level {fhicl::Name("metricsLevel" ) , fhicl::Comment("Metrics reporting level"), 1};
    };

    explicit CaloDataVerifier(const art::EDFilter::Table<Config>& config);

    bool filter(art::Event & e) override;
    virtual bool endRun(art::Run& run ) override;


  private:
    std::set<int> dtcs_;
    int           diagLevel_;
    int           metrics_reporting_level_;
    bool          isFirstEvent_;
  };
}  // namespace mu2e

mu2e::CaloDataVerifier::CaloDataVerifier(const art::EDFilter::Table<Config>& config)
  : art::EDFilter{config}, 
  diagLevel_(config().diagLevel()),
  isFirstEvent_(true)    
  {
    produces<mu2e::EventHeader>();
  }

bool mu2e::CaloDataVerifier::filter(art::Event& event)
{

  art::EventNumber_t eventNumber = event.event();

  // Collection of CaloHits for the event
  std::unique_ptr<std::vector<mu2e::CalorimeterTestDataDecoder>> caloFragColl(
      new std::vector<mu2e::CalorimeterTestDataDecoder>);


  artdaq::Fragments fragments;
  artdaq::FragmentPtrs containerFragments;

  std::vector<art::Handle<artdaq::Fragments>> fragmentHandles;
  fragmentHandles = event.getMany<std::vector<artdaq::Fragment>>();

  for (const auto& handle : fragmentHandles) {
    if (!handle.isValid() || handle->empty()) {
      continue;
    }

    if (handle->front().type() == artdaq::Fragment::ContainerFragmentType) {
      for (const auto& cont : *handle) {
        artdaq::ContainerFragment contf(cont);
        if (contf.fragment_type() != mu2e::FragmentType::DTCEVT) {
          break;
        }

        for (size_t ii = 0; ii < contf.block_count(); ++ii) {
          containerFragments.push_back(contf[ii]);
          fragments.push_back(*containerFragments.back());
        }
      }
    } else {
      if (handle->front().type() == mu2e::FragmentType::DTCEVT) {
        for (auto frag : *handle) {
          fragments.emplace_back(frag);
        }
      }
    }
  }

  if (diagLevel_ > 0) {
    std::cout << "[CaloDataVerifier::filter] Found nHandlesnFragments  "
              << fragments.size() << std::endl;
  }

  size_t nFrags(0);


  for (const auto& frag : fragments) {
    mu2e::DTCEventFragment bb(frag);

    auto caloSEvents = bb.getSubsystemData(DTCLib::DTC_Subsystem::DTC_Subsystem_Calorimeter);
    for (const auto& subevent : caloSEvents) {
      mu2e::CalorimeterTestDataDecoder cf(subevent);
      caloFragColl->emplace_back(cf);
      ++nFrags;

      // iterate over the data blocks
      std::vector<DTCLib::DTC_DataBlock> dataBlocks = subevent.GetDataBlocks();
      for (unsigned int j = 0; j < dataBlocks.size(); ++j){
        std::cout << "Data block [" << j << "]:" << std::endl;
        // print the data block header
        DTCLib::DTC_DataHeaderPacket* dataHeader = dataBlocks[j].GetHeader().get();
        std::cout << dataHeader->toJSON() << std::endl;

        // print the data block payload
        const void* dataPtr = dataBlocks[j].GetData();
        std::cout << "Data payload:" << std::endl;
        for (int l = 0; l < dataHeader->GetByteCount() - 16; l += 2){
          auto thisWord = reinterpret_cast<const uint16_t*>(dataPtr)[l];
          std::cout << "\t0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(thisWord) << std::endl;
        }

        // Print the calo data decoder info
        auto CaloPacketColl = cf.GetCalorimeterHitData(j);
        uint npackets = CaloPacketColl->size();
        std::cout << "There are " << npackets << " hit packets" << std::endl;
        for (uint packet_i = 0; packet_i<npackets; packet_i++){
          mu2e::CalorimeterTestDataDecoder::CalorimeterHitTestDataPacket hit = CaloPacketColl->at(packet_i).first;
          std::vector<uint16_t> hit_bytes = CaloPacketColl->at(packet_i).second;

          std::cout << "Hit packet "   << packet_i << " :"<<std::endl;
          std::cout << "BeginMarker: " << hit.BeginMarker << std::endl;
          std::cout << "BoardID: "     << hit.BoardID     << std::endl;
          std::cout << "ChannelID: "   << hit.ChannelID   << std::endl;
          std::cout << "InPayloadEventWindowTag: "   << hit.InPayloadEventWindowTag << std::endl;
          std::cout << "LastSampleMarker: "          << hit.LastSampleMarker << std::endl;
          std::cout << "ErrorFlags: "                << hit.ErrorFlags << std::endl;
          std::cout << "Time: "                      << hit.Time << std::endl;
          std::cout << "IndexOfMaxDigitizerSample: " << hit.IndexOfMaxDigitizerSample << std::endl;
          std::cout << "NumberOfSamples: "           << hit.NumberOfSamples << std::endl;

          std::cout << "Dumping the waveform: " << std::endl;
          for (uint wi=0; wi<hit_bytes.size(); wi++){
            std::cout << hit_bytes[wi] << " ";
          }
          std::cout << std::endl;
        }
      }
    }
  }



  if ( (diagLevel_ > 0) && (nFrags == 0)) {
    std::cout << "[CaloDataVerifier::filter] found no fragments!" << std::endl;
  }

  if (diagLevel_ > 0) {
    std::cout << "mu2e::CaloDataVerifier::filter exiting eventNumber="
              << (int)(event.event()) << " / timestamp=" << (int)eventNumber << std::endl;
  }


  if (metricMan != nullptr)
    {
      metricMan->sendMetric("nFragments", fragments.size(), "Fragments",
          metrics_reporting_level_, artdaq::MetricMode::LastPoint);
    }

  return true;
}


bool mu2e::CaloDataVerifier::endRun( art::Run&  ) {
  return true;
}

DEFINE_ART_MODULE(mu2e::CaloDataVerifier)
