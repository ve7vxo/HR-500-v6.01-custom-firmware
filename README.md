# HR-500-v6.01-custom-firmware
Control firmware for HR-500 RF amplifier

** Disclaimer: This is experimental code. Use at your own risk. The author of this code assumes no liability for its use.**
HR-500-v6.00-custom-firmware

Modified firmware for Hardrock HR-500 RF amplifier.

Major re-write of original code. New features, bug fixes.
Scrapped port change interrupt code and wrote polled code. Removed 1ms timer overflow interrupt which was dominating system realtime. Watchdog implemented.

NEWS: YO6SSW has re-written drivers for the display board to reduce latency. This should improve the bargraph display dynamics substantially.
Version 5.00 Notes. -- VE7VXO

-- Changes colour scheme on displays. -- A peak indicator on the bargraph is added. -- Band display text colour changes to match antenna display text colour as a reminder of antenna selection per band as defined in Set_Theme() colour definitions.

-- 57600 and 115200 Baud added to AccBaud menu. -- Default AccBaud set to 38400. -- Default USB Baud set to 115200.

-- Modem added to XCVR menu. -- Default Band set to 160m. -- Incorporated fault latching ideas from Geert Jan deGroot PE1HZG. Trip condition needs mode press to unlatch.

-- There was a bug in some of the display calculations which had a map() function nested inside a constrain() which is a faux-pas for the arduino compiler. Re-wrote these sections with a simple map(). -- Moving average filters replaced with exponential weighted moving average which is easier to tune.

-- All attempts to make the bar graph lively and dynamic in previous versions met with failure.
The TFT display hardware is just too slow to draw a full bar so the previous code's while loop was eliminated and new code written to only draw a four pixel wide vertical line like the peak indicator.
This works very well. :^>

-- Code edits from PE1HZG to put F/C temperature unit selection in menu system and use of snprintf statements for menu items incorporated in this version. Thank you Geert Jan!

-- Improvements made to VSWR meter routines to improve display dynamic. -- T/R switching routine simplified and moved to top of main loop since many other routines are dependent on TX/RX state. -- Some other niceties like having swr display blank until a valid reading, resetting swr display after changing antennas. Hold swr and fault displays after fault trip. Last vswr appears after drive power removed following a trip.

-- Changed colour references to use different colour variable names which are defined in this header file.

-- Menu item code taken from PE1HZG for temperature unit selection.

-- HRTS command (Temperature units Set) added to serial routines for reading/setting display theme. F = Fahrenheit C = Celsius. Thanks to Geert Jan De Groot PE1HZG.

-- Added menu item for theme selection. Currently only two themes supported NORMAL and NIGHT.
Colour definition can be changed in Set_Theme() routine. Used 16 bit colour picker from https://rgbcolorpicker.com/565

-- HRDS command (Display Set) added to serial routines for reading/setting display theme. D = day N = night.

-- This version does not use the port change interrupt for PTT sensing but polls the port bit in the main loop and uses a Chronometer for debouncing. Previously there was an occasional glitch if PTT changed while interrupts were disabled. Debounce time is defined in this header file.

-- intSensor() routine modified to be dependent on minimal drive power sensed. This keeps the exponential filters from de-integrating during CW characters and gives more accurate and smoother power display bargraph.

-- Moved carrier detect routines into top of main loop alogside PTT detection and added chronometer for T/R delay to prevent relay wear during fast CW keying with or without PTT signal.

-- Found out needless timer interrupts every millisecond were preventing the freqcount library from operating properly so removed timer interrupt completely.

Version 6.00 Notes. -- VE7VXO

-- Added array in memory and in EEPROM for ATU state per band. Amplifier now remembers if ATU is ON/BYP per band.

-- Implemented watchdog timer to reset amp in case of stack overflow or other surprise endings.

-- Changed TX indicator to reflect PTT vs COR activation based on colour

-- Mode text and colour changed.

Version 6.01 Notes. -- VE7VXO

-- Fault reset bug fixed.

*/
