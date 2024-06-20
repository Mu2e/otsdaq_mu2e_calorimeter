import sys, os, re, glob
import pathlib

import ctypes
import pathlib

# Load the API shared object
filePath = pathlib.Path(__file__).parent.resolve()
# my_lib = ctypes.CDLL(os.path.join( filePath, "../bin/SBL_utils.so" ))
my_lib = ctypes.CDLL(os.path.join( filePath, "../API_I2C_Shared.so" ))



def Read_Firmware_Version_ID(busAddr: str):
    
    # Encode a string as a byte sequence
    busAddr = busAddr.encode('utf-8')
    
    # Get the length of the returned buffer
    my_lib.Read_Firmware_Version_ID.argtypes = [ctypes.POINTER(ctypes.c_char)]
    my_lib.Read_Firmware_Version_ID.restype = ctypes.c_int
    
    res = my_lib.Read_Firmware_Version_ID(busAddr)

    print(f"Py Read_Firmware_Version_ID Done - Res {res}")
    

def User_Application_Firmware_Upgrade(busAddr: str, imageNumber: int, binFile: str ):
    
    # Encode a string as a byte sequence
    busAddr = busAddr.encode('utf-8')
    binFile = binFile.encode('utf-8')
    
    # Get the length of the returned buffer
    my_lib.User_Application_Firmware_Upgrade.argtypes = [ctypes.POINTER(ctypes.c_char), ctypes.c_int, ctypes.POINTER(ctypes.c_char)]
    my_lib.User_Application_Firmware_Upgrade.restype = ctypes.c_int
    
    res = my_lib.User_Application_Firmware_Upgrade(busAddr, imageNumber, binFile)

    print(f"Py User_Application_Firmware_Upgrade Done - Res {res}")
    


if __name__ == "__main__":
    
    print("Ciao")
    User_Application_Firmware_Upgrade("/dev/ttyUSB0", 1)