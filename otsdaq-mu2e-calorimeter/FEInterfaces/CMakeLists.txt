
include(otsdaq::FEInterface)
    
cet_build_plugin(ROCCalorimeterInterface otsdaq::FEInterface LIBRARIES REG otsdaq_mu2e::ROCPolarFireCoreInterface
 )
 
 
install_headers()
install_source()
