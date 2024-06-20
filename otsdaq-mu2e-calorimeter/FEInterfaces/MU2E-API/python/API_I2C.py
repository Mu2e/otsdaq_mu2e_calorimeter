#!/usr/bin/env python3

import sys, os, re, glob
import pathlib

import ctypes
import pathlib

# Load the API shared object
filePath = pathlib.Path(__file__).parent.resolve()
# my_lib = ctypes.CDLL(os.path.join( filePath, "../bin/API_I2C.so" ))
my_lib = ctypes.CDLL(os.path.join( filePath, "../API_I2C_Shared.so" ))



# Function for extracting all the COMMAND labels from the enumerative type
def parseEnumFile(filePath: str) -> dict:
    
    # Whether or not inside the enum items
    insideEnum = False
    
    # Return dict. Cmd are keys, values are the corresponding int
    enumDict = {}
    
    with open(filePath, "r") as f:
        for line in f.readlines():
            
            # Labels start where = 0 is present and end with a }
            if re.search("=.*0.*,", line):
                insideEnum = True
                i=0
            if re.search("}", line):
                insideEnum = False
                
            # Inside the enum, append to a dict the label and the corresponding int
            if insideEnum:
                tmpCmd = line.split(",")[0].split("//")[0].split("=")[0].strip()
                
                enumDict[tmpCmd] = i
                i+=1
                
    return enumDict



# Default file implemented
def parseEnumFileStandard() -> dict:
    return parseEnumFile(os.path.join( filePath, "../res/MZB_commands_MZB_OSCMDCODE_t.h" ))



# Obtain the byte seq from a command and a parameter list
def MZB_Encode_CMD_Command(MZB_OSCMDCODE: int, params: list[float]) -> bytearray:
    
    # Cast all elements to float
    params = [float(p) for p in params]
    
    
    
    # Define args and return types for the c function
    my_lib.MZB_Encode_CMD_Command.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_float), ctypes.c_int]
    my_lib.MZB_Encode_CMD_Command.restype = ctypes.POINTER(ctypes.c_ubyte) 
    
    
    
    # Parameters to be passed to the C function
    command = MZB_OSCMDCODE #enumDict["DACSET"]
    numParams = len(params)
    params = (ctypes.c_float * numParams)(*params)  
    
    
    # Call the function, obtain a pointer
    result_ptr = my_lib.MZB_Encode_CMD_Command(command, params, numParams)

    # Decode the result from the pointer and print it out
    result = bytearray()  
    for i in range(46):
        if i%8 == 0: print("\n", end = "")
        
        result.append(result_ptr[i])
        print(f"{result_ptr[i]:02x}", end = " ")
    print("\n", end = "")
    
    return result




# Call the function that encodes and send a command
def MZB_Encode_and_Write(MZB_OSCMDCODE: int, params: list[float], busAddr: str):
    
    # Encode a string as a byte sequence
    busAddr = busAddr.encode('utf-8')
    
    # Cast all elements to float
    params = [float(p) for p in params]
    
    
    
    # Define args and return types for the c function
    my_lib.MZB_Encode_and_Write.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_float), ctypes.c_int, ctypes.POINTER(ctypes.c_char)]
    my_lib.MZB_Encode_and_Write.restype = ctypes.c_ssize_t
    
    
    
    # Parameters to be passed to the C function
    command = MZB_OSCMDCODE #enumDict["DACSET"]
    numParams = len(params)
    params = (ctypes.c_float * numParams)(*params)  
    
    
    # Call the function, obtain a pointer
    result = my_lib.MZB_Encode_and_Write(command, params, numParams, busAddr)

    return result



"""
def Read_Voltage_Block(busAddr: str):
    
    # Encode a string as a byte sequence
    busAddr = busAddr.encode('utf-8')
    
    # Get the length of the returned buffer
    my_lib.GetSize_Voltage_Block.argtypes = []
    my_lib.GetSize_Voltage_Block.restype = ctypes.c_size_t
    
    tmpSize = my_lib.GetSize_Voltage_Block()
    
    
    # Read the VL block
    my_lib.Read_Voltage.argtypes = [ctypes.POINTER(ctypes.c_char)]
    my_lib.Read_Voltage.restype = ctypes.POINTER(ctypes.c_ubyte)
      
    result_ptr = my_lib.Read_Voltage(busAddr)
    
    
    
    # Decode the result from the pointer and print it out
    for i in range(int(tmpSize/2)):
        print(int.from_bytes(result_ptr[2*i:2*i+2], byteorder = "little"))
    
    my_lib.free(result_ptr)
    
    return 0
"""
    

# Function for reading the 20 VL Values
def Read_Voltage(busAddr: str) -> list  :
    
    # Encode a string as a byte sequence
    busAddr = busAddr.encode('utf-8')
    
    # Get the length of the returned buffer
    my_lib.GetSize_Voltage.argtypes = []
    my_lib.GetSize_Voltage.restype = ctypes.c_size_t
    
    tmpSize = my_lib.GetSize_Voltage()
    
    
    # Read the VL values
    my_lib.Read_Voltage.argtypes = [ctypes.POINTER(ctypes.c_char)]
    my_lib.Read_Voltage.restype = ctypes.POINTER(ctypes.c_uint16)
      
    result_ptr = my_lib.Read_Voltage(busAddr)
    
    
    # Obtain the list of returned values
    outLst = [result_ptr[i] for i in range(int(tmpSize/ctypes.sizeof(ctypes.c_uint16)))]
    
    my_lib.free(result_ptr)
    
    print("---- Reading Voltage VL Values ----")
    print(outLst)
    print("----")
    
    return outLst



# Function for reading the 20 CA Values
def Read_Current(busAddr: str) -> list  :
    
    # Encode a string as a byte sequence
    busAddr = busAddr.encode('utf-8')
    
    # Get the length of the returned buffer
    my_lib.GetSize_Current.argtypes = []
    my_lib.GetSize_Current.restype = ctypes.c_size_t
    
    tmpSize = my_lib.GetSize_Current()
    
    
    # Read the CA values
    my_lib.Read_Current.argtypes = [ctypes.POINTER(ctypes.c_char)]
    my_lib.Read_Current.restype = ctypes.POINTER(ctypes.c_uint16)
      
    result_ptr = my_lib.Read_Current(busAddr)
    
    
    # Obtain the list of returned values
    outLst = [result_ptr[i] for i in range(int(tmpSize/ctypes.sizeof(ctypes.c_uint16)))]
    
    my_lib.free(result_ptr)
    
    print("---- Reading Current CA Values ----")
    print(outLst)
    print("----")
    
    return outLst



# Function for reading the 20 TC Values
def Read_Temperature(busAddr: str) -> list  :
    
    # Encode a string as a byte sequence
    busAddr = busAddr.encode('utf-8')
    
    # Get the length of the returned buffer
    my_lib.GetSize_Temperature.argtypes = []
    my_lib.GetSize_Temperature.restype = ctypes.c_size_t
    
    tmpSize = my_lib.GetSize_Temperature()
    
    
    # Read the TC values
    my_lib.Read_Temperature.argtypes = [ctypes.POINTER(ctypes.c_char)]
    my_lib.Read_Temperature.restype = ctypes.POINTER(ctypes.c_uint16)
      
    result_ptr = my_lib.Read_Temperature(busAddr)
    
    
    # Obtain the list of returned values
    outLst = [result_ptr[i] for i in range(int(tmpSize/ctypes.sizeof(ctypes.c_uint16)))]
    
    my_lib.free(result_ptr)
    
    print("---- Reading Temperature TC Values ----")
    print(outLst)
    print("----")
    
    return outLst



# Function for reading the 20 HV Values
def Read_VoltageSet(busAddr: str) -> list  :
    
    # Encode a string as a byte sequence
    busAddr = busAddr.encode('utf-8')
    
    # Get the length of the returned buffer
    my_lib.GetSize_VoltageSet.argtypes = []
    my_lib.GetSize_VoltageSet.restype = ctypes.c_size_t
    
    tmpSize = my_lib.GetSize_VoltageSet()
    
    
    # Read the HV values
    my_lib.Read_VoltageSet.argtypes = [ctypes.POINTER(ctypes.c_char)]
    my_lib.Read_VoltageSet.restype = ctypes.POINTER(ctypes.c_uint16)
      
    result_ptr = my_lib.Read_VoltageSet(busAddr)
    
    
    # Obtain the list of returned values
    outLst = [result_ptr[i] for i in range(int(tmpSize/ctypes.sizeof(ctypes.c_uint16)))]
    
    my_lib.free(result_ptr)
    
    print("---- Reading Voltage Set HV Values ----")
    print(outLst)
    print("----")
    
    return outLst



    

      

if __name__ == "__main__":
    
    enumDict = parseEnumFileStandard()
    res = MZB_Encode_CMD_Command(enumDict["DACSET"], (0, 20))
    
    
    
    Read_Voltage("/dev/i2c-1")
    Read_Current("/dev/i2c-1")
    Read_Temperature("/dev/i2c-1")
    Read_VoltageSet("/dev/i2c-1")
