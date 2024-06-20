
include(otsdaq::FEInterface)

include_directories(MU2E-API)
include_directories(MU2E-API/res)
    
cet_build_plugin(ROCCalorimeterInterface otsdaq::FEInterface LIBRARIES REG otsdaq_mu2e::ROCPolarFireCoreInterface
 )
 
 
install_headers()
install_source()
