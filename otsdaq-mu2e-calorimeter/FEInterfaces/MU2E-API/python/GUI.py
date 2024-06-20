#!/usr/bin/env python3 

from MainWindow import *
from PyQt5.QtCore import pyqtSlot

from API_I2C import *
from SBL_utils import *

import sys, os, re, glob
import pathlib

# sudo apt install python3-pyuic5
# sudo apt-get install pyqt5-dev-tools


# Number of channels per each MZB
MZB_numChannels = 20


class MainWindow(QtWidgets.QMainWindow, Ui_MainWindow):    
    def __init__(self):
        super(MainWindow, self).__init__()
        self.setupUi(self)
        
        
        # Populate combobox with all available commands
        filePath = pathlib.Path(__file__).parent.resolve()
        # self.dictCommands = parseEnumFile(os.path.join( filePath, "../res/MZB_commands_MZB_OSCMDCODE_t.h" ))
        self.dictCommands = parseEnumFileStandard()
        self.comboBox.addItems(self.dictCommands.keys())
        
        
        for btn in self.frameSetSingleBias.findChildren(QtWidgets.QPushButton):
            btn.clicked.connect(self.btnSetBias)
            


    # Function for obtaining up to 9 arguments set in 1st tab
    def getParamLst(self):
        tmpLstParams = []
        
        # Loop over each textbox
        for tmpTextBox in self.frame.findChildren(QtWidgets.QLineEdit):
            
            # Append the values of non empty text boxes
            tmpText = tmpTextBox.text().strip()
            if not tmpText == "":
                tmpLstParams.append(tmpText)
                
        return tmpLstParams

        
        
        
    # Function for encoding a command and writing in the textbox
    @pyqtSlot()
    def on_btnGenerateByteSeq_clicked(self):
        
        tmpLstParams = self.getParamLst()
                
        # Call the function to obtain byte seq
        print(self.dictCommands[self.comboBox.currentText()])
        res = MZB_Encode_CMD_Command(self.dictCommands[self.comboBox.currentText()], tmpLstParams)
        # print(res.hex())
        resAscii = ' '.join([f'{byte:02X}' for byte in res])
        
        
        self.resultTextBox.setPlainText(resAscii)
        
        
          
          
    # Function for encoding and writing a command
    @pyqtSlot()
    def on_btn_I2Caddr_clicked(self):      
        
        tmpLstParams = self.getParamLst()
        
        # Call the function to encode and send a command
        tmpI2Caddr = self.txt_I2Caddr.text().strip()
        MZB_Encode_and_Write(self.dictCommands[self.comboBox.currentText()], tmpLstParams, tmpI2Caddr)
        
        
        
    # Function for setting the bias of an individual channel (2nd tab)
    @pyqtSlot()
    def btnSetBias(self):
        
        sender = self.sender()  # Ottiene il bottone che ha generato l'evento
        numChan = int(sender.objectName().split("_")[-1]) 
        
        # Cerco la corrispondente casella di testo
        tmpTxtBias = self.findChild(QtWidgets.QLineEdit, f"txtBiasSetChan_{numChan}")
        tmpBiasValue = float(tmpTxtBias.text().strip())
        
        print(f"Setterò il bias del canale {numChan} a {tmpBiasValue}" )
        
                
        # Call the function to encode and set the bias
        tmpI2Caddr = self.txt_I2Caddr.text().strip()
        MZB_Encode_and_Write(self.dictCommands["DACSET"], (numChan, tmpBiasValue), tmpI2Caddr)
        
        # Refresh labels
        self.btn_refreshReadback.click()

        
        
    # Function for setting the bias of all channels 
    @pyqtSlot()
    def on_btn_setAllBias_clicked(self):
        
        # tmpTxtBias = float(self.findChild(QtWidgets.QLineEdit, f"txt_setAllBias"))
        tmpTxtBias = float(self.txt_setAllBias.text().strip())

        # Call the function to encode and set the bias
        tmpI2Caddr = self.txt_I2Caddr.text().strip()
        MZB_Encode_and_Write(self.dictCommands["DACSET"], (0, tmpTxtBias), tmpI2Caddr)
        
        # Refresh labels
        self.btn_refreshReadback.click()
        
        
        
    # Refresh all the readback values in tab 2
    @pyqtSlot()
    def on_btn_refreshReadback_clicked(self):
        
        tmpI2Caddr = self.txt_I2Caddr.text().strip()
        
        lstVL = Read_Voltage(tmpI2Caddr)
        lstCA = Read_Current(tmpI2Caddr)
        lstTC = Read_Temperature(tmpI2Caddr)
        lstHV = Read_VoltageSet(tmpI2Caddr)
        
        # Update all readback values
        for i in range(MZB_numChannels):
            self.findChild(QtWidgets.QLabel, f"lblReadVolt_{i+1}").setText(f"{lstVL[i]/100:.2f}") # V
            self.findChild(QtWidgets.QLabel, f"lblReadCurr_{i+1}").setText(f"{lstCA[i]/100:.2f}") # uA
            self.findChild(QtWidgets.QLabel, f"lblReadTemp_{i+1}").setText(f"{lstTC[i]/100:.2f}") #°C
            self.findChild(QtWidgets.QLabel, f"lblReadVoltSet_{i+1}").setText(f"{lstHV[i]/10:.2f}") #V
        
        
        
    @pyqtSlot()
    def on_btn_HVON_clicked(self):
        
        tmpI2Caddr = self.txt_I2Caddr.text().strip()

        print("Switching HV ON...")
        res = MZB_Encode_and_Write(self.dictCommands["HVONOFF"], (1,), tmpI2Caddr)

        if res:
            print(f"An error occurred -- Error: {res}")
        else:
            print("Done!")
        
        # Refresh labels
        self.btn_refreshReadback.click()
        
        
        
    @pyqtSlot()
    def on_btn_HVOFF_clicked(self):
        
        tmpI2Caddr = self.txt_I2Caddr.text().strip()

        print("Switching HV OFF...")
        res = MZB_Encode_and_Write(self.dictCommands["HVONOFF"], (0,), tmpI2Caddr)

        if res:
            print(f"An error occurred -- Error: {res}")
        else:
            print("Done!")

        # Refresh labels
        self.btn_refreshReadback.click()
        
        
        
        
    @pyqtSlot()
    def on_btnDebug_clicked(self):
        print("DEBUG!!!")
        
                
    @pyqtSlot()
    def on_btn_versionID_clicked(self):
        
        tmpI2Caddr = self.txt_I2Caddr.text().strip()

        Read_Firmware_Version_ID(tmpI2Caddr)
        
        
    # @pyqtSlot()
    # def on_btn_upgradeFW_clicked(self):
        
    #     tmpI2Caddr = self.txt_I2Caddr.text().strip()

    #     # User_Application_Firmware_Upgrade(tmpI2Caddr,1)
    #     User_Application_Firmware_Upgrade(tmpI2Caddr,1, self.txt_upgradeFW.text().strip())
        
       
    @pyqtSlot()
    def on_btn_upgradeFW_1_clicked(self):
        
        tmpI2Caddr = self.txt_I2Caddr.text().strip()

        # User_Application_Firmware_Upgrade(tmpI2Caddr,1)
        User_Application_Firmware_Upgrade(tmpI2Caddr,1, self.txt_upgradeFW_1.text().strip())
        
       
    @pyqtSlot()
    def on_btn_upgradeFW_2_clicked(self):
        
        tmpI2Caddr = self.txt_I2Caddr.text().strip()

        # User_Application_Firmware_Upgrade(tmpI2Caddr,1)
        User_Application_Firmware_Upgrade(tmpI2Caddr,2, self.txt_upgradeFW_2.text().strip())
        
       



# Run the GUI
if __name__ == '__main__':
    app = QtWidgets.QApplication(sys.argv)
    app.setApplicationName("Mu2e MZB contol")

    mainWindow = MainWindow()
    mainWindow.show()
    sys.exit(app.exec_())
