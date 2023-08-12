//
// Definitions for symbols related to process management classes
//

#ifndef PROCESSDEFS_H_
#define PROCESSDEFS_H_

// Defined type for process code
typedef long PROCESS;

// Process error codes
#define PROC_OK                         0x00
#define PROC_UNKNOWNPROCESS             0x01
#define PROC_TIMEOUT                    0x02
#define BREW_ERR_TIMEOUT_MASHER         0x10
#define BREW_ERR_TIMEOUT_EXTRACTOR      0x11
#define BREW_ERR_TIMEOUT_CONVEYOR       0x12
#define BREW_ERR_TIMEOUT_HEAT           0x13
#define BREW_ERR_TIMEOUT_BOIL           0x14
#define BREW_ERR_TIMEOUT_CUPFILL        0x15
#define BREW_ERR_TIMEOUT_PROBE          0x16
#define CONVEYOR_ERR_TIMEOUT_ADVANCE    0x20
#define MILK_ERR_TIMEOUT_RAISE			0x30
#define MILK_ERR_TIMEOUT_FULLCYCLE		0x31
#define MILK_ERR_TIMEOUT_ZONEACCESS		0x32
#define STIR_ERR_TIMEOUT_RAISE			0x40
#define STIR_ERR_TIMEOUT_LOWER			0x41
#define STIR_ERR_TIMEOUT_ZONEACCESS		0x42
#define SUGAR_ERR_TIMEOUT_FULLCYCLE		0x50

// Process timeout values
#define BREW_TIMEOUT_MASHER             5000
#define BREW_TIMEOUT_EXTRACTOR          11000
#define BREW_TIMEOUT_CONVEYOR           3000
#define BREW_TIMEOUT_HEAT               200000
#define BREW_TIMEOUT_BOIL               300000
#define BREW_TIMEOUT_CUPFILL            30000
#define BREW_TIMEOUT_PROBE              5000
#define CONVEYOR_TIMEOUT_ADVANCE        10000
#define MILK_TIMEOUT_RAISE				5000
#define MILK_TIMEOUT_FULLCYCLE			6000
#define MILK_LOAD_DELAY					1200
#define MILK_TIMEOUT_ZONEACCESS			60000
#define STIR_TIMEOUT_RAISE				7000
#define STIR_TIMEOUT_LOWER				7000
#define STIR_TIMEOUT_ZONEACCESS			60000
#define SUGAR_TIMEOUT_FULLCYCLE			5000

// Minimum water temperature for brewing (centigrade)
#define BREW_WATERTEMPERATURE_MIN       80

// Number of mashes per unit of tea strength
#define BREW_MASHFACTOR                 2

// Time delay for partial raise of masher (mS)
#define BREW_PARTIALRAISEDELAY          500

// Delay for periodic cup conveyor test
#define CONVEYOR_TEST_PERIOD			10000

// Interval between multiple milk dispenses
#define MILK_DISPENSE_INTERVAL			800

// Number of dispenses applied for each milk level unit requested
#define	DISPENSE_FACTOR					2

// Stirring period (mS)
#define STIR_PERIOD						7000

#endif /* PROCESSDEFS_H_ */