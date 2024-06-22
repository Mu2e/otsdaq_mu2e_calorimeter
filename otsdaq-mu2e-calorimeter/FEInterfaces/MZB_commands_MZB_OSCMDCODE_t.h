#ifndef _MZB_COMMANDS_OSCMDCODE_H
#define _MZB_COMMANDS_OSCMDCODE_H


#ifdef GNU_COMP
  typedef enum {
#else
  typedef enum MZB_OSCMDCODE_t {
#endif
	SYNTAX_ERROR  = 0,//
	ADCONVERT,      // AD128S start conversion
	ADCFG,          // Configure ADC operational mode
	ADINIT,         // Initialize ADC
	ADSTS,          // Show ADC status
	APPCONFIG,      // Show the MEZZANINE configuration
	AUTOCALIBRATE,	// Calibrate ADC on active channels
	AUTOCONFIGURE,	// Search APD sensor on I2C bus
  AUTOSTART,      // Set/clear AUTOSTART flag
	AUTOWDG,        // Watchdog flag, if set HW WDG is activated
	BUSYSRC,        // 1=Force BUSY line from RIT, 0=Read from DIRAC
	BUSYTMO,        // Timeout for BUSY line
	CALADCP1P2,     // ADC two point linear calibration
	CALLPCP1P2,     // Calibrate internal AD (LPC.AD)
	CALDACP1P2,     // DAC two point linear calibration
	CALCARD,        // Set all data calibration for APD cards
	CALDATA,        // Set/get calibration data
	CALSOURCE,      // Set/get the calibration data source 0=FLASH_ROM, 1=RAM
	CALTEMP,        // Calibrate PT1000 on LPC.AD4
	CARDSTS,        // Report the status of all the enabled FEE cards
	COMBPS,         // Set serial port baud rate
	DACSET,         // Set DAC121S DAC output in physical values
	DACSTS,         // Status of DAC devices
	DEBUGMODE,      // Show debug mode status
	DELTAT_ms,      // Set ADC sampling time
	EMACENABLE,	    // Enable DP83848C PHY device
	EMACINIT,       // Initialize DP83848C PHY device
	EMACSTS,        // Dump DP83848 PHY registers
	EMACWRITE,      // Write DP83848 PHY registers
	ERRORS,         // Report and clear error list
	ERRSTS,         // Report a detailed status of the errors
	FRFLAG,         // Set/clear Fault Recovery Flag
	GAIN,           // Set gain of the APD pre-amp
	GPINPUT,        // Report GP input status
	GPOUTPUT,       // Set level on output line #n
	HELLO,          // Hello message
	HELP,           // Display this help screen
	HVOO_AUTOSET,   // Autoset the HV on/off switch after a hot start
	HVONOFF,        // Enable/disable HV
	I2CRESET,       // Reset I2C bus bit banging the SCL line
	I2CSTS,         // Show I2C interface status
	I2CENUMERATE,   // Enumerate I2C devices
	IP4ADR,         // Set IP address
	IRQ,            // Reserved
	IRQENABLE,      // Enable/Disable IRQ on GPI
  MSGLEVEL,       // Application message level
  MPXREG,         // Read I2C I/O expander registers
	MZBDUMP,        // Dump the MEZZANINE register content
	LOGIN ,         // Start session user:ROOT
	LOGOUT,         // Terminate session
	PROCESSES,      // Show active processes
	POST,           // Power on self test
	RESTART,        // MCU reset
	RESTORE,        // Restore settings
	RIT,            // Set RIT pace rate
	ROOT,           // RESERVED
  SBLACTIVATE,    // Activate SBL to upgrade firmware by I2C
	SEU,            // Force a number of Single Bit Upset
	SHOW_TRANSIENT, // Show real time ADC data
	SKIPERROR,      // Skip error messages
	SILENT,         // Enable/disable diagnostic messsages
	SLEWRATE,       // Set HV bias slew rate in units V/s
	STATUS_REG,     // Get status register
	TEST,           // Reserved
	UPDATE,         // Save current parameters
	VERSION,        // Show firmware version
	HWWDG,          // Set and show the HW Watchdog status
	SWWDG,          // Set and show the SW Watchdog status
	DIRACWDG,       // Set and show the DIRAC Watchdog status
	DIRACRST,       // Pulse the DIRAC RESET REQUEST line (0.1s)
	WATCHDOG,       // Show status for the all implemented watchdog
  RESERVED        //
} MZB_OSCMDCODE_t;



#endif