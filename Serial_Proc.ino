long freqLong = 0;
unsigned short oldBand;
unsigned int old_tuner_freq;
const char crlfsemi[] = ";\r\n";
const char HRTM[] = "HRTM";
char freqStr[6];

void uartGrabBuffer(){                                      // Buffer USB serial data
   int z = 0;
   char lastChar[1];
   lastChar[0] = 'C';

   while (lastChar[0] != 0x3B) {                            // Read characters into array till ; encountered
      workingString[z] = rxbuff[readStart];
      rxbuff[readStart] = 0x00;
      readStart++;
      lastChar[0] =  workingString[z];
      z++;
      if (readStart > 127){                                 // Ignore a long string of garbabe and exit while loop
        readStart = 0;
      }
   }

   workingString[z] = 0x00;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void uartGrabBuffer2(){                                     // Buffer ACC serial data
   int z = 0;
   char lastChar[1];
   lastChar[0] = 'C';

   while (lastChar[0] != 0x3B) {                            // Read characters into array till ; encountered
      workingString2[z] = rxbuff2[readStart2];
      rxbuff2[readStart2] = 0x00;
      readStart2++;
      lastChar[0] =  workingString2[z];
      z++;
      if (readStart2 > 127) {                               // Ignore a long string of garbabe and exit while loop
        readStart2 = 0;
      }
   }

   workingString2[z] = 0x00;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void findBand(short uart) {
   char *found;
   char* workStringPtr;
   int i, wsl;
   
   if (uart == 1) {                                                           // get recieved data from USB or ACC
      workStringPtr = workingString;
   } else {
      workStringPtr = workingString2;
   }
   wsl = strlen(workStringPtr);
   for (i = 0; i <= wsl; i++) workStringPtr[i] = toupper(workStringPtr[i]);   // convert working string to uppercase

   found = strstr(workStringPtr,"FA");
   if (found == 0) {
      found = strstr(workStringPtr,"IF");
   }
      
//fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
      
   if (found != 0) {
      for (i = 0; i <= 5; i++) {
         freqStr[i] = found[i + 4];                                           // Parse numerical part of string
      }
      freqStr[i] = 0;
      freqLong = atol(freqStr);                                               // convert to numeric value and evaluate per band
      if (freqLong >= 1750 && freqLong <= 2100) {
           BAND = 10;
      } else if (freqLong >= 3200 && freqLong <= 4200) {
           BAND = 9;
      } else if (freqLong >= 5000 && freqLong <= 5500) {
           BAND = 8;
      } else if (freqLong >= 6900 && freqLong <= 7500) {
           BAND = 7;
      } else if (freqLong >= 10000 && freqLong <= 10200) {
           BAND = 6;
      } else if (freqLong >= 13900 && freqLong <= 14500) {
           BAND = 5;
      } else if (freqLong >= 18000 && freqLong <= 18200) {
           BAND = 4;
      } else if (freqLong >= 20900 && freqLong <= 21500) {
           BAND = 3;
      } else if (freqLong >= 24840 && freqLong <= 25100) {
           BAND = 2;
      } else if (freqLong >= 27900 && freqLong <= 29800) {
           BAND = 1;
      } else {
           BAND = 0;
      }//endif
      if (BAND != NBAND)NBAND = BAND;                                       // Change band only after two consecutive matching 
      else if(NBAND != OBAND) SetBand();                                    // frequency strings to ignore corrupted serial data
      //if (BAND != OBAND) SetBand();
   }
      
//fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
      
   
   found = strstr(workStringPtr,"HR");                                        // Parse Hobby PCB command structure:
   if (found != 0) {
        
                                                                              // Band setting
      found = strstr(workStringPtr,"HRBN");
      if (found != 0) {
        if (found[4] == ';'){
           UART_send(uart, (char*)"HRBN");
           UART_send_num(uart, BAND);
           UART_send_line(uart);
        }
        else if (found[5] == ';') BAND = found[4] - 0x30;
        else BAND = (found[4] - 0x30) * 10 + (found[5] - 0x30);
        if (BAND > 10) BAND = 0;
        if (BAND != OBAND) SetBand();
      }
      
      
//fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
      
                                                                              // Mode setting
      found = strstr(workStringPtr,"HRMD");
      if (found != 0) {
        if (found[4] == ';'){
           UART_send(uart, (char*)"HRMD");
           UART_send_num(uart, MODE);
           UART_send_line(uart);
        }
        if (found[4] == '0'){
          MODE = 0;
          DrawMode();
        }
        if (found[4] == '1'){
          MODE = 1;
          trip = 0;
          DrawMode();
        }
      }

      
//fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
      
                                                                              // Antenna setting
      found = strstr(workStringPtr,"HRAN");
      if (found != 0) {
        if (found[4] == ';'){
           UART_send(uart, (char*)"HRAN");
           UART_send_num(uart, ANTSEL[BAND]);
           UART_send_line(uart);
        }
        if (found[4] == '1'){
          ANTSEL[BAND] = 1;
          EEPROM.write(eeantsel+BAND, ANTSEL[BAND]);
          DrawAnt();
        }
        if (found[4] == '2'){
          ANTSEL[BAND] = 2;
          EEPROM.write(eeantsel+BAND, ANTSEL[BAND]);
          DrawAnt();
        }
      }

      
//fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
      
                                                                           // Temperature unit setting
      found = strstr(workStringPtr,"HRTS");
      if (found != 0) {
        if (found[4] == ';'){
           UART_send(uart, (char*)"HRTS");
           if (TMP_UNIT == 0) UART_send(uart, (char*)"F");
           else UART_send(uart, (char*)"C");
           UART_send_line(uart);
        }
        if (found[4] == 'F'){
          TMP_UNIT = 0;
          EEPROM.write(eecelsius, TMP_UNIT);
        }
        if (found[4] == 'C'){
          TMP_UNIT = 1;
          EEPROM.write(eecelsius, TMP_UNIT);
        }
      }

      
//fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
      
                                                                          // Set theme DAY or NIGHT
      found = strstr(workStringPtr,"HRDS");                               // New hardrock display set command (DS)
      if (found != 0) {
        if (found[4] == ';'){
           UART_send(uart, (char*)"HRDS");
           if (Theme == 0) UART_send(uart, (char*)"D");
           else UART_send(uart, (char*)"N");
           UART_send_line(uart);
        }
        if (found[4] == 'D'){
          Theme = 0;
          EEPROM.write(eetheme, Theme);
          snprintf(item_disp[mTheme], IDSZ, "     NORMAL     ");
          Set_Theme();
          DrawMeter();                                                    // Re-draw screens.
          DrawHome();
        }
        if (found[4] == 'N'){
          Theme = 1;
          EEPROM.write(eetheme, Theme);
          snprintf(item_disp[mTheme], IDSZ, "     NIGHT      ");
          Set_Theme();
          DrawMeter();                                                    // Re-draw screens.
          DrawHome();
        }
      }

      
//fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
      
                                                                          //  Read volts:
      found = strstr(workStringPtr,"HRVT");
      if (found != 0) {
        char vbuff[4];
        UART_send(uart, (char*)"HRVT");
        snprintf(vbuff, sizeof(vbuff), 
          "%2d", Read_Voltage()/40);
        UART_send(uart, vbuff);
        UART_send_line(uart);
      }      
      
      
//fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
      
                                                                          // Read ATU_P:
      found = strstr(workStringPtr,"HRAP");
      if (found != 0) {
        UART_send(uart, (char*)"HRAP");
        UART_send_num(uart, ATU_P);
        UART_send_line(uart);
      }
            
      
//fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
      
                                                                          // Bypass activate the ATU:
      found = strstr(workStringPtr,"HRTB");
      if (found != 0) {
        if (found[4] == ';'){
           UART_send(uart, (char*)"HRTB");
           UART_send_num(uart, ATU);
           UART_send_line(uart);
        }
        if (found[4] == '1'){
          ATU = 1;
          ATUB[BAND] = ATU;                                               // Update per-band array of ATU button state
          EEPROM.write(eeatub+BAND, ATUB[BAND]);                          // Update array in EEPROM too
          DrawATU();
        }
        if (found[4] == '0'){
          ATU = 0;
          ATUB[BAND] = ATU;                                               // Update per-band array of ATU button state
          EEPROM.write(eeatub+BAND, ATUB[BAND]);                          // Update array in EEPROM too
          DrawATU();
        }
      }
      
      
//fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
      
                                                                          // Press the TUNE button
      found = strstr(workStringPtr,"HRTU");
      if (found != 0 && ATU_P) {
        Tune_button();
      }      

                                                                          // Tune Result
      found = strstr(workStringPtr,"HRTR");
      if (found != 0) {
        UART_send(uart, (char*)"HRTR");
        UART_send_char(uart, ATU_STAT);
        UART_send_line(uart);
      }      

      
//fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
      
                                                                          // Is ATU Tuning?
      found = strstr(workStringPtr,"HRTT");
      if (found != 0) {
        UART_send(uart, (char*)"HRTT");
        UART_send_num(uart, TUNING);
        UART_send_line(uart);
      }      

      
//fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
      
                                                                         // Read temp:
      found = strstr(workStringPtr,"HRTP");
      if (found != 0) {
        char tbuff[4];
        int tread;
        if (TMP_UNIT == 0) tread = ((t_ave * 9) / 5) + 320;
        else tread = t_ave;
        tread /= 10;
        UART_send(uart, (char*)"HRTP");
        snprintf(tbuff, sizeof(tbuff), "%d%c",
          tread, TMP_UNIT ? 'C' : 'F');
        UART_send(uart, tbuff);
        UART_send_line(uart);
      }
      
//fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
      
                                                                        // read power
                                                                        // HRPWF; = Forward
                                                                        // HRPWR; = Reverse
                                                                        // HRPWD; = Drive
                                                                        // HRPWV; = VSWR 
      found = strstr(workStringPtr,"HRPW");
      if (found != 0) {
        byte psel, pwf = 0;
        char schr, tbuff[10];

        schr = found[4];
        if (schr == 'F') {psel = fwd_p; pwf = 1;}
        if (schr == 'R') {psel = rfl_p; pwf = 1;}
        if (schr == 'D') {psel = drv_p; pwf = 1;}
        if (schr == 'V') {psel = vswr; pwf = 1;}

        if (pwf == 1){
          unsigned int reading = Read_Power(psel);
          snprintf(tbuff, sizeof(tbuff), "HRPW%c%03d", schr, reading);
          UART_send(uart, tbuff);
          UART_send_line(uart);
        }
      }

      
//fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
      
                                                                              // Report status
      found = strstr(workStringPtr,"HRST");
      if (found != 0) {
        char tbuff[80];
        
        unsigned int stF = Read_Power(fwd_p);
        unsigned int stR = Read_Power(rfl_p);
        unsigned int stD = Read_Power(drv_p);
        unsigned int stS = Read_Power(vswr);
        unsigned int stV = Read_Voltage()/40;
        unsigned int stI = Read_Current()/20;
        unsigned int stT = t_ave/10;
        if (TMP_UNIT == 0) stT = ((stT * 9) / 5) + 32;
        snprintf(tbuff, sizeof(tbuff), 
          "HRST-%03d-%03d-%03d-%03d-%03d-%03d-%03d-%01d-%02d-%01d-%01d%01d%01d%01d%01d%01d%01d%01d-", 
        stF, stR, stD, stS, stV, stI, stT, MODE, BAND, ANTSEL[BAND], TX, F_alert, R_alert, D_alert, V_alert, I_alert, TUNING, ATU);
        UART_send(uart, tbuff);
        UART_send_line(uart);
      }

      
//fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
      
                                                                           // Respond and acknowlege:
      found = strstr(workStringPtr,"HRAA");
      if (found != 0) {
        UART_send(uart, (char*)"HRAA");
        UART_send_line(uart);
      }

      
//fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
      
                                                                   // Get new firmware
      found = strstr(workStringPtr,"HRFW");
      if (found != 0) {
        Serial.end();
        delay(50);
        Serial.begin(115200);
        while (!Serial.available());
        delay(50); 
        digitalWrite(8, LOW);
      }
      
//fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
      
                                                                  // Communicate with ATU
      found = strstr(workStringPtr,"HRTM");
      if (found != 0 && ATU_P) {
        char tm_buff[20];
        byte tmb_p = 0;
        size_t b_len;
        
        while (found[tmb_p + 4] != ';' && tmb_p < 18){
          tm_buff[tmb_p] = found[tmb_p + 4];
          tmb_p++; 
        }
        tm_buff[tmb_p] = 0;
        Serial3.setTimeout(75);
        Serial3.print('*');
        Serial3.println(tm_buff);
        b_len = Serial3.readBytesUntil(13, ATU_buff, 40);
        ATU_buff[b_len] = 0;
        
        if (b_len > 0){
          UART_send(uart, (char*)"HRTM");
          UART_send(uart, ATU_buff);
          UART_send_line(uart);
        }
      }
  }
}   








///////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void UART_send(char uart, char *message){
  if (uart == 1) Serial.print(message);
  if (uart == 2) Serial2.print(message);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void UART_send_num(char uart, int num){
  if (uart == 1) Serial.print(num);
  if (uart == 2) Serial2.print(num);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void UART_send_char(char uart, char num){
  if (uart == 1) Serial.print(num);
  if (uart == 2) Serial2.print(num);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void UART_send_line(char uart){
  if (uart == 1) Serial.println(";");
  if (uart == 2) Serial2.println(";");
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void UART_send_cr(char uart){
  if (uart == 1) Serial.println();
  if (uart == 2) Serial2.println();
}
