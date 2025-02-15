/***************************************************************************************************************
  ------------  Hobbypcb HR-500 Hardrock 500Watt RF amplifier control firmware --------------
  
  Project notes:

  LCD1 is the left LCD and LCD2 is the one on the right.
  Serial is USB, Serial2 is ACC serial for CAT interface, Serial3 is for ATU if installed
  Low pass filter board uses a serial shift register I/O expander for relay control using signals
  R_CS, and SPI port MOSI, SCK.
  
  The system uses an LTC2945 wide range I2C power monitor for monitoring power supply voltage and current.
  
 Version 5.00 Notes. -- VE7VXO
  ------------------
  -- Changes colour scheme on displays.
  -- A peak indicator on the bargraph is added. 
  -- Band display text colour changes to match antenna display text colour as a reminder 
      of antenna selection per band as defined in Set_Theme() colour definitions.
  
  -- 57600 and 115200 Baud added to AccBaud menu.
  -- Default AccBaud set to 38400.
  -- Default USB Baud set to 115200.
  
  -- Modem added to XCVR menu.
  -- Default Band set to 160m.
  -- Incorporated fault latching ideas from Geert Jan deGroot PE1HZG.  Trip condition needs mode press to unlatch.
  

  -- There was a bug in some of the display calculations which had a map() function nested inside a constrain()
      which is a faux-pas for the arduino compiler.  Re-wrote these sections with a simple map().
  -- Moving average filters replaced with exponential weighted moving average which is easier to tune.

  -- All attempts to make the bar graph lively and dynamic in previous versions met with failure.  
      The TFT display hardware is just too slow to draw a full bar so the previous code's while loop
      was eliminated and new code written to only draw a four pixel wide vertical line like the peak indicator.  
      This works very well. :^>  

  -- Code edits from PE1HZG to put F/C temperature unit selection in menu system and use of snprintf statements
      for menu items incorporated in this version.  Thank you Geert Jan!  
  
  -- Improvements made to VSWR meter routines to improve display dynamic.
  -- T/R switching routine simplified and moved to top of main loop since many other routines are dependent on TX/RX state.
  -- Some other niceties like having swr display blank until a valid reading, resetting swr 
      display after changing antennas.  Hold swr and fault displays after fault trip.  Last vswr appears after
      drive power removed following a trip.

  -- Changed colour references to use different colour variable names which are defined in this header file.

  -- Menu item code taken from PE1HZG for temperature unit selection.    
  
  -- HRTS command (Temperature units Set) added to serial routines for reading/setting display theme.
      F = Fahrenheit  C = Celsius. Thanks to Geert Jan De Groot PE1HZG.
  
  -- Added menu item for theme selection. Currently only two themes supported NORMAL and NIGHT.  
      Colour definition can be changed in Set_Theme() routine. 
      Used 16 bit colour picker from https://rgbcolorpicker.com/565

  -- HRDS command (Display Set) added to serial routines for reading/setting display theme.
      D = day  N = night.
  
  -- This version does not use the port change interrupt for PTT sensing but polls the port bit in the main 
      loop and uses a Chronometer for debouncing.  Previously there was an occasional glitch if PTT changed while 
      interrupts were disabled.  Debounce time is defined in this header file.

  -- intSensor() routine modified to be dependent on minimal drive power sensed.  This keeps the exponential
      filters from de-integrating during CW characters and gives more accurate and smoother power display bargraph.

  -- Moved carrier detect routines into top of main loop alogside PTT detection and added chronometer for T/R delay
      to prevent relay wear during fast CW keying with or without PTT signal.

  -- Found out needless timer interrupts every millisecond were preventing the freqcount library from operating properly
      so removed timer interrupt completely.
      
 Version 6.00 Notes. -- VE7VXO
  ------------------
  -- Added array in memory and in EEPROM for ATU state per band.  Amplifier now remembers if ATU is ON/BYP per band.

  -- Implemented watchdog timer to reset amp in case of stack overflow or other surprise endings.

  -- Changed TX indicator to reflect PTT vs COR activation based on colour

  -- Mode text and colour changed.
      
 Version 6.01 Notes. -- VE7VXO
  ------------------

  -- Fault reset bug fixed.
  
 * *************************************************************************************************************
 */


 
#define VERSION "6.01"


#define Debounce 10                                 // PTT contact debounce time (milliseconds).
#define TR_dly 300                                  // Transmit-Recieve delay time (milliseconds).

//#define DEVELOPMENT                               // if defined then compile code blocks for development

//**************  Hardware pin definitions:
// LCD and Touch Panel:
#define LCD1_CS 35                                // Left LCD chip select
#define LCD2_CS 53                                // Right LCD chip select
#define SD1_CS 39                                 // Left LCD SD chip select
#define SD2_CS 44                                 // Right LCD SD chip select
#define TP1_CS 41                                 // Left LCD touch panel chip select
#define TP2_CS 42                                 // Right LCD touch panel chip select
#define LCD_BL 5                                  // Backlight for both LCD's
#define BIAS_EN 4                                 // Amplifier bias enable
#define TEMP_S 14                                 // Temperature sense analog input
#define TTL_PU 6                                  // Pullup pin for TTL serial data ACC serial (Serial2)
#define SER_POL 7                                 // Ouput to NOR gate to invert ACC serial polarity (Serial2)
#define FT817_V 11                                // Analog input for detecting FT-817
#define RST_OUT 8                                 // Output to drive reset pin
#define ATU_TUNE 45                               // Digital output for ATU control (labeled ATU_BYP on schematic)PORTL4
#define ATU_BUSY 43                               // Digital input for ATU control ( labeled ATU_CS on schematic)PORTL6
#define ATTN_INST 24                              // Digital input to detect optional attenuator installed
#define ATTN_ON 22                                // Digital output for attenuator control

//
// Rear Panel
#define FAN1 27                                   // Fan speed control outputs
#define FAN2 29
#define BYP_RLY 31                                // RF bypass relay control
#define ANT_RLY 33                                // Antenna select relay control
#define COR_DET 10                                // Carrier power detect
#define INPUT_RF 15                               // Drive power sense analog input
#define FWD_RF 12                                 // Forward power sense analog input
#define RFL_RF 13                                 // Reflected power sense analog input
#define F_COUNT 47                                // Frequency counter input

//
// Power Board
#define PTT_DET 11
#define RESET 23                                  // Output to reset amplifier trip logic (flip-flop on power board)
#define LTCADDR B1101111                          // LTC2945 power monitor Table 1 both LOW (7bit address)

//
// LPF Board
#define RELAY_CS 25                               // Chip select for LPF board serial I/O expander
#define TRIP_FWD 3                                // Input for trip detectiom on forward power
#define TRIGGER 3                                 // For development work re-use TRIP_FWD pin as scope trigger PORTL4
#define TRIP_RFL 2                                // Input for trip detectiom on reflected power

//
//EEPROM LOCATIONS
#define eeband 1
#define eetheme 2
#define eeantsel 3
#define eemetsel 15
#define eecelsius 16
#define eeaccbaud 18
#define eecordelay 19
#define eexcvr 20
#define eeusbbaud 21
#define eemcal 22
#define eeatub 23                                 // Added locations to store ATU button state per band
#define eeattn 35


//*********** output control 'macro' defines

#define SETUP_LCD1_CS pinMode(LCD1_CS, OUTPUT);               // Macro defines and sets port pin for left display
#define LCD1_CS_HIGH digitalWrite(LCD1_CS, HIGH);
#define LCD1_CS_LOW digitalWrite(LCD1_CS, LOW);

#define SETUP_LCD2_CS pinMode(LCD2_CS, OUTPUT);               // Macro defines and sets port pin for right display
#define LCD2_CS_HIGH digitalWrite(LCD2_CS, HIGH);
#define LCD2_CS_LOW digitalWrite(LCD2_CS, LOW);

#define SETUP_SD1_CS pinMode(SD1_CS, OUTPUT);                 // Macro defines and sets port pin for left display SD card
#define SD1_CS_HIGH digitalWrite(SD1_CS, HIGH);
#define SD1_CS_LOW digitalWrite(SD1_CS, LOW);

#define SETUP_SD2_CS pinMode(SD2_CS, OUTPUT);                 // Macro defines and sets port pin for right display SD card
#define SD2_CS_HIGH digitalWrite(SD2_CS, HIGH);
#define SD2_CS_LOW digitalWrite(SD2_CS, LOW);

#define SETUP_TP1_CS pinMode(TP1_CS, OUTPUT);                 // Macro defines and sets port pin for left display touch panel
#define TP1_CS_HIGH digitalWrite(TP1_CS, HIGH);
#define TP1_CS_LOW digitalWrite(TP1_CS, LOW);

#define SETUP_TP2_CS pinMode(TP2_CS, OUTPUT);                 // Macro defines and sets port pin for right display touch panel
#define TP2_CS_HIGH digitalWrite(TP2_CS, HIGH);
#define TP2_CS_LOW digitalWrite(TP2_CS, LOW);

#define SETUP_LCD_BL pinMode(LCD_BL, OUTPUT);                 // Macro to control displays backlight

#define SETUP_BIAS pinMode(BIAS_EN, OUTPUT);                  // Macro to control power amplifier bias                  
#define BIAS_ON digitalWrite(BIAS_EN, HIGH);
#define BIAS_OFF digitalWrite(BIAS_EN, LOW);

#define SETUP_TTL_PU pinMode(TTL_PU, INPUT);                  // Macro to initialize TTL pullup for serial as input 

#define SETUP_ATU_TUNE pinMode(ATU_TUNE, OUTPUT);             // Macro to control ATU 
#define ATU_TUNE_HIGH digitalWrite(ATU_TUNE, HIGH);
#define ATU_TUNE_LOW digitalWrite(ATU_TUNE, LOW);

#define SETUP ATU_BUSY pinMode(ATU_BUSY, INPUT);              // Macro to check ATU
#define GET ATU_BUSY digitalRead(ATU_BUSY);

#define S_POL_SETUP pinMode(SER_POL, OUTPUT);                 // Macro to control serial polarity through hardware NOR gate
#define S_POL_NORM digitalWrite(SER_POL, LOW);
#define S_POL_REV digitalWrite(SER_POL, HIGH);

#define SETUP_ATTN_ON pinMode(ATTN_ON, OUTPUT);               // Macro for controlling optional drive attenuator
#define ATTN_ON_HIGH digitalWrite(ATTN_ON, HIGH);
#define ATTN_ON_LOW digitalWrite(ATTN_ON, LOW);

#define SETUP_ATTN_INST pinMode(ATTN_INST, INPUT_PULLUP);     // Macro to detect optional drive attenuator
#define ATTN_INST_READ digitalRead(ATTN_INST)

#define SETUP_FAN1 pinMode(FAN1, OUTPUT);                     // Macro for controlling RF pallet fan speed
#define SETUP_FAN2 pinMode(FAN2, OUTPUT);

#define SETUP_F_COUNT pinMode(F_COUNT, INPUT_PULLUP);         // Macro to make input for frequency counting

#define SETUP_BYP_RLY pinMode(BYP_RLY, OUTPUT);               // Macro to control bypass relay on back panel PCB
#define RF_BYPASS digitalWrite(BYP_RLY, LOW);
#define RF_ACTIVE digitalWrite(BYP_RLY, HIGH);

#define SETUP_ANT_RLY pinMode(ANT_RLY, OUTPUT);               // Macro for antenna select relays
#define SEL_ANT1 digitalWrite(ANT_RLY, LOW);
#define SEL_ANT2 digitalWrite(ANT_RLY, HIGH);

#define SETUP_COR pinMode(COR_DET, INPUT_PULLUP);             // Macro makes input for carrier detect

#define SETUP_PTT pinMode(PTT_DET, INPUT_PULLUP);             // Macro makes input for PTT detect

#define SETUP_RESET pinMode(RESET, OUTPUT); digitalWrite(RESET, HIGH);                // Make reset line high
#define RESET_PULSE digitalWrite(RESET, LOW); delay(1); digitalWrite(RESET, HIGH);    // Pulse reset low 1 msec

#define SETUP_RELAY_CS pinMode(RELAY_CS, OUTPUT);             // Macro to make relay chip select output
#define RELAY_CS_HIGH digitalWrite(RELAY_CS, HIGH);           // Macro to make relay chip select high
#define RELAY_CS_LOW digitalWrite(RELAY_CS, LOW);             // Macro to make relay chip select low
#define SETUP_TRIP_FWD pinMode(TRIP_FWD, INPUT_PULLUP);       // Input appears to be unused
#define SETUP_TRIP_RFL pinMode(TRIP_RFL, INPUT_PULLUP);       // Input appears to be unused

#ifdef DEVELOPMENT
#define SETUP_TRIGGER pinMode(TRIGGER, OUTPUT);
#endif

#define DKRED 0x5800                                          // Additional custom colours
#define LTRED 0xa000                                          
#define DKGRN 0x0280
#define DKYEL 0xc621
#define DKBLU 0x004c
#define DKGRY 0x4a48
#define LTGRY 0x6b4b


enum{ mACCbaud,                                               // Create numbered list for menu routines
      mTheme,
      mXCVR,
      mMCAL,
      mCELSIUS,
      mSETbias,
      mUSBbaud,
      mATWfwl,
      mATTN};
      
#define menu_max 8

const char* menu_items[] = {
  " ACC Baud Rate  ",
  " Display Theme  ",
  "  Transceiver   ",
  "   Meter Cal    ",
  "   Temp Unit    ",
  "    Set Bias    ",
  " USB Baud Rate  ",
  " ATU FW Update  ",
  "  Input Atten   "
};

#define IDSZ 17                                               // Item display string 16 chars + terminator
char id1[IDSZ];
char id2[IDSZ];
char id3[IDSZ];
char id4[IDSZ];
char id5[IDSZ];
char id6[IDSZ];
char id7[IDSZ];
char id8[IDSZ];
char id9[IDSZ];

char *item_disp[] = {id1, id2, id3, id4, id5, id6, id7, id8, id9};

enum{                                                       // Create numbered list for meter selection
  fwd_p,
  rfl_p,
  drv_p,
  vswr
};


//*************** Menu level array for transciever menu selection 

#define xcvr_max 7
enum{                                                       // Create numbered list for xcvr menu item selection
  xnone,
  xhobby,
  xkx23,
  xic705,
  xft817,
  xelad,
  xxieg,
  xmodem
};


//*************** Display character structure for transciever menu selection

const char *xcvr_disp[] = {                                 // Array of display text for xcvr menu items                             
  "     None       ",
  " RS-HFIQ / IQ32 ",
  "ELECRAFT  KX2/3 ",
  "  ICOM IC705    ",
  " YAESU FT-817/8 ",
  "  ELAD FDM DOU  ",
  "  XIEGU  XCVRS  ",
  "     MODEM      "
};


//*************** Linearization constants for drive power calculations
//*************** Referenced to INPUT_RF analog read.  One per band.
const unsigned int d_lin[] = {0,  116, 114, 111, 109, 106, 104, 102, 101, 100,  99};



//*************** SWR lookup table referenced to RL_CH calculation in ATU function block.
const unsigned int swr[] = {
    999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,999,
    999,999,968,917,872,831,793,759,728,699,673,648,625,604,584,566,
    548,532,517,503,489,476,464,452,441,431,421,412,403,394,386,378,
    371,363,356,350,343,337,331,326,320,315,310,305,300,296,291,287,
    283,279,275,272,268,264,261,258,254,251,248,245,242,240,237,234,
    232,229,227,224,222,220,218,216,214,211,209,208,206,204,202,200,
    199,197,195,194,192,190,189,187,186,185,183,182,181,179,178,177,
    176,174,173,172,171,170,169,168,167,166,165,164,163,162,161,160,
    159,158,157,156,156,155,154,153,152,152,151,150,149,149,148,147,
    147,146,145,145,144,143,143,142,142,141,140,140,139,139,138,138,
    137,137,136,136,135,135,134,134,133,133,132,132,132,131,131,130,
    130,129,129,129,128,128,128,127,127,126,126,126,125,125,125,124,
    124,124,124,123,123,123,122,122,122,121,121,121,121,120,120,120,
    120,119,119,119,119,118,118,118,118,117,117,117,117,117,116,116,
    116,116,116,115,115,115,115,115,114,114,114,114,114,113,113,113,
    113,113,113,112,112,112,112,112,112,112,111,111,110,106,103,100
    };
 
