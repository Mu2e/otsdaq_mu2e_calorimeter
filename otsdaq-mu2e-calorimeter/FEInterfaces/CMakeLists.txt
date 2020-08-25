include_directories(${MPI_INCLUDE_PATH})
include_directories(${CMAKE_BINARY_DIR}/artdaq)
    
    
simple_plugin(ROCCalorimeterInterface "interface"	
	otsdaq-mu2e_FEInterfaces_ROCPolarFireCoreInterface_interface
	FECore
	ROCCore
	TableCore
	Macros
	Configurable
	ConfigurationInterface
	WorkLoopManager
	artdaq-core_Utilities
	${XDAQ_XERCES-C}
	${MF_MESSAGELOGGER}
	pthread
 )
 
 
install_headers()
install_source()
