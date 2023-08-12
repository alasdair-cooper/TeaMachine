//
//	HardwareDefs.h
//
//	Created: 05/02/2017 12:11:35
//	Author: Nick
//

#ifndef HARDWAREDEFS_H_
#define HARDWAREDEFS_H_

// Digital I/O pin usage
#define INPUTREG_CLOCK		            3			// OUT: Input register clock
#define INPUTREG_LATCH		            4			// OUT: Input register latch
#define INPUTREG_DATA		            5			// IN:  Input register data
#define OUTPUTREG_CLOCK		            7			// OUT: Main output register clock
#define OUTPUTREG_LATCH		            6			// OUT: Main output register latch
#define OUTPUTREG_DATA		            2			// OUT: Main output register data
#define ONE_WIRE_BUS				    8			// I/O: Serial connection to temperature sensor
#define CUPDETECT_TRIGGER               9			// OUT: Cup detection trigger signal
#define DISPLAYREG_CLOCK                10			// OUT: Display output register clock
#define DISPLAYREG_LATCH                11			// OUT: Display output register latch
#define DISPLAYREG_DATA                 12			// OUT: Display output register  data
#define CUPDETECT_ECHO                  13			// IN: Cup detector echo response
#define SYSMODE_BIT0					32			// IN: Rotary mode switch setting (LSB)
#define SYSMODE_BIT1					34			// IN: Rotary mode switch setting (MSB)
#define STIRRER_LOWERED					36			// IN: Lower limit switch signal for stirrer
#define SUGAR_DONE						41			// IN: Cycle complete limit switch signal for sugar dispense

// System operating modes
#define SYSMODE_NORMAL				    0x00
#define SYSMODE_TESTGROUP1			    0x01
#define SYSMODE_TESTGROUP2			    0x02
#define SYSMODE_LOAD				    0x03

// Group 1 tests
#define TESTCODE_EXTRACTOR				0x01
#define TESTCODE_BAGCONVEYOR			0x02
#define TESTCODE_MASHER					0x03
#define TESTCODE_BOILERCNTRL			0x04
#define TESTCODE_WATERVALVE				0x05
#define TESTCODE_CUPCONVEYOR			0x06
#define TESTCODE_CUPLEVELARM			0x07
#define TESTCODE_DISPLAY    			0x08
#define TESTCODE_CUPDETECTOR 			0x09
#define TESTCODE_USERCONTROLS 			0x0A
#define TESTCODE_MILKDISPENSER 			0x0B
#define TESTCODE_STIRRER 				0x0C
#define TESTCODE_SUGARDISPENSER			0x0D

// Group 2 tests
#define TESTCODE_INSERVICE_WITHLOGGING	0x00
#define TESTCODE_USERINTERFACE			0x01
#define TESTCODE_BREWPROCESS			0x02
#define TESTCODE_MILKPROCESS			0x03
#define TESTCODE_STIRPROCESS			0x04

// Input register masks
#define INREGMASK_MODE					0x000F
#define INREGMASK_EXTRACTOR_HILIMITSW	0x0010
#define INREGMASK_EXTRACTOR_LOLIMITSW	0x0020
#define INREGMASK_BAGCON_STATIONARY		0x0040
#define INREGMASK_MASHER_HILIMITSW		0x0080
#define INREGMASK_MASHER_LOLIMITSW		0x0100
#define INREGMASK_CUPLEVEL_HILIMIT		0x0200
#define INREGMASK_CUPCON_STATIONARY	    0x0400
#define INREGMASK_ACTIONSWITCH			0x0800
#define INREGMASK_MILKDISP_RAISING		0x1000
#define INREGMASK_MILKDISP_HILIMITSW	0x2000
#define INREGMASK_CUPPROBE_RETRACTED	0x4000
#define INREGMASK_STIRRER_HILIMITSW		0x8000

// Output register bit assignments
#define OUTREG_BAGCON_ADVANCE		    0	// Pulse high to advance teabag conveyor by one position
#define OUTREG_BOILERCTRL			    1	// True to activate water heater
#define OUTREG_VALVECTRL			    2	// True to open water valve
#define OUTREG_CUPCON_ADVANCE		    3	// Pulse high to advance cup conveyor by one position
#define OUTREG_LED_FAULT			    4	// Active low
#define OUTREG_LED_TESTMODE			    5	// Active high
#define OUTREG_EXTRACTOR_UPDOWN		    6	// Extractor direction - High=raise, Low=lower
#define OUTREG_EXTRACTOR_EN			    7	// Active high extractor motor enable
#define OUTREG_MASHER_UPDOWN		    8	// Masher direction - High=raise, Low=lower
#define OUTREG_MASHER_EN			    9	// Active high masher motor enable
#define OUTREG_CUPDETECT_S0             12  // Cup detector station select bit 0
#define OUTREG_CUPDETECT_S1             13  // Cup detector station select bit 1
#define OUTREG_CUPDETECT_S2             14  // Cup detector station select bit 2
#define OUTREG_CUPDETECT_S3             15  // Cup detector station select bit 3
#define OUTREG_CUPPROBE_EN			    16	// Active high cup probe arm motor enable
#define OUTREG_CUPPROBE_UPDOWN		    17	// Cup probe arm action - High=lower, Low=raise
#define OUTREG_MILKDISP_EN				18	// Active high milk raise/lower motor enable
#define OUTREG_MILKDISP_RAISE			19	// Milk dispenser bottle action - Pulse high to raise
#define OUTREG_MILKDISP_LOWER			20	// Milk dispenser bottle action - Pulse high to lower
#define OUTREG_STIRRER_UPDOWN			21	// Stirrer arm control - High=lower, Low = raise
#define OUTREG_STIRRER_EN   			22	// Stirrer arm motor enable
#define OUTREG_SUGAR_DISPENSE		    23	// Pulse high to start the sugar dispense cycle

#define DISPLAY_DRINKNAMELENGTH         4   // Number of characters in the displayed drink name
#define BLANK                           -1  // Value for a blank numeric display
#define ERROR                           -2  // Value for a numeric display indicating error condition
#define UNEXPECTED						-3	// Value for a numeric display indicating the unexpected presence of a cup

// Station information
#define STATIONS                        9   // Number of stations
#define ST_RECEPTION_C                  1   // Station number of reception station C
#define ST_RECEPTION_B                  2   // Station number of reception station B
#define ST_RECEPTION_A                  3   // Station number of reception station A
#define ST_BREW                         4   // Station number of brew station
#define ST_MILK                         5   // Station number of milk station
#define ST_STIR                         6   // Station number of stir station
#define ST_COLLECTION_C                 7   // Station number of collection station C
#define ST_COLLECTION_B                 8   // Station number of collection station B
#define ST_COLLECTION_A                 9   // Station number of collection station A
#define ST_FARLEFT                      9   // Station number of the station on the left hand end
#define ST_FARRIGHT                     1   // Station number of the station on the right hand end
#define ST_UNKNOWN                      -1  // Used to indicate an unknown station

// Numeric LED display segment codes
#define SEGMENTCODE_0                   0x81
#define SEGMENTCODE_1                   0xED
#define SEGMENTCODE_2                   0x43
#define SEGMENTCODE_3                   0x49
#define SEGMENTCODE_4                   0x2D
#define SEGMENTCODE_5                   0x19
#define SEGMENTCODE_6                   0x11
#define SEGMENTCODE_7                   0xCD
#define SEGMENTCODE_8                   0x01
#define SEGMENTCODE_9                   0x09
#define SEGMENTCODE_A                   0x05
#define SEGMENTCODE_B                   0x31
#define SEGMENTCODE_C                   0x93
#define SEGMENTCODE_D                   0x61
#define SEGMENTCODE_E                   0x13
#define SEGMENTCODE_F                   0x17
#define SEGMENTCODE_ERROR               0x13
#define SEGMENTCODE_UNEXPECTED          0x7F
#define SEGMENTCODE_BLANK               0xFF

// Alphanumeric LED display segment codes
#define SEGMENTCODEHI_A                 0x66
#define SEGMENTCODELO_A                 0xD2
#define SEGMENTCODEHI_C                 0x76
#define SEGMENTCODELO_C                 0x3E
#define SEGMENTCODEHI_D                 0xDC
#define SEGMENTCODELO_D                 0x1A
#define SEGMENTCODEHI_E                 0x66
#define SEGMENTCODELO_E                 0x36
#define SEGMENTCODEHI_F                 0x66
#define SEGMENTCODELO_F                 0xF6
#define SEGMENTCODEHI_I                 0xDC
#define SEGMENTCODELO_I                 0x3E
#define SEGMENTCODEHI_K                 0x67
#define SEGMENTCODELO_K                 0xED
#define SEGMENTCODEHI_L                 0x77
#define SEGMENTCODELO_L                 0x3F
#define SEGMENTCODEHI_N                 0x73
#define SEGMENTCODELO_N                 0xCB
#define SEGMENTCODEHI_O                 0x76
#define SEGMENTCODELO_O                 0x1A
#define SEGMENTCODEHI_R                 0x66
#define SEGMENTCODELO_R                 0xE2
#define SEGMENTCODEHI_S                 0xE6
#define SEGMENTCODELO_S                 0x16
#define SEGMENTCODEHI_T                 0xDC
#define SEGMENTCODELO_T                 0xFE
#define SEGMENTCODEHI_W                 0x37
#define SEGMENTCODELO_W                 0xCB

// Cup detection
#define CUPDETECT_MAXDISTANCE           3      // Max cup distance in cm

// User controls
#define UC_CUP_INC                      0
#define UC_CUP_DEC                      1
#define UC_DRINK_INC                    2
#define UC_DRINK_DEC                    3
#define UC_MILK_INC                     4
#define UC_MILK_DEC                     5
#define UC_SUGAR_INC                    6
#define UC_SUGAR_DEC                    7
#define UC_STRENGTH_INC                 8
#define UC_STRENGTH_DEC                 9
#define UC_GO                           10
#define UC_CANCEL                       11

#define UC_MAXCONTROLS                  12
#define UC_DEBOUNCE_DELAY               50     // Debounce delay in mS

// Serial communications
#define SERIAL_BAUDRATE					9600

#endif /* HARDWAREDEFS_H_ */