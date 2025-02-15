int ATU_exch(void){                                                 // Exchange serial data between control and ATU boards
  size_t b_len;
  
  Serial3.setTimeout(50);                                           // Don't get hung waiting for ATU serial responses
  Serial3.println(ATU_cmd);
  b_len = Serial3.readBytesUntil(0x13, ATU_buff, 28);
  ATU_buff[b_len] = 0;
  return b_len; 
}

void Tune_button(void){                                             // Tune has been requested.
  while (ts2.touched());
  Tft.LCD_SEL = 1;
  Tft.lcd_fill_rect(121, 142, 74, 21, BG_col);                      // Erase stale text form ATU message windows
  Tft.lcd_fill_rect(121, 199, 74, 21, BG_col);

  
  if (!TUNING){                                                     // Initiate tuning if not in progress already
    if (ATU == 0){
      ATU = 1;
      DrawATU();
    }
    Tft.drawString((uint8_t*)"STOP", 122, 199,  3, FG_col);         // Display new ATU text while tuning
    Tft.drawString((uint8_t*)"TUNING", 122, 142,  2, WARN_LED);
    TUNING = 1;
    if (TX){                                                        // If the amp is on, turn it off
      PTT = 0;
      BIAS_OFF
      TX = 0;
      byte SRSend_RLY = SR_DATA;
      RELAY_CS_LOW
      SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
      SPI.transfer(SRSend_RLY);
      SPI.endTransaction();
      RELAY_CS_HIGH    
      RF_BYPASS
    }
    ATU_TUNE_LOW                                                    // Activate the tune line
    delay(5);                                                       // Tells tuner board to go to work
  }
  else Tune_End();
}




void Tune_End(void){                                                // Handle termination of matching attempt.
  Tft.LCD_SEL = 1;
  Tft.lcd_fill_rect(121, 142, 74, 21, BG_col);                      // Erase stale ATU text
  Tft.lcd_fill_rect(121, 199, 74, 21, BG_col);
  Tft.drawString((uint8_t*)"TUNE", 122, 199,  3, ACT_TXT);          // display TUNE button
  TUNING = 0;
  ATU_TUNE_HIGH                                                     // Signal ATU board to stop
  delay(10);
  ATU_buff[0] = 0;
  Serial3.setTimeout(50);
  Serial3.println("*S");                                            // Request status update from ATU board
  Serial3.readBytesUntil(0x13, ATU_buff, 5);                                      
  strcpy(RL_TXT," -.-");                                            // Update ATU  text and button 
  ATU_STAT = ATU_buff[0];                                           // state depending on status
                                                                    // Reply and save to EEPROM afterward
  switch (ATU_STAT){                                                
    case 'F': Tft.drawString((uint8_t*)"FAILED", 122, 142,  2, ALRM_LED); ATUB[BAND] = 0; break;
    case 'E': Tft.drawString((uint8_t*)"HI SWR", 122, 142,  2, ALRM_LED); ATUB[BAND] = 0; break;
    case 'H': Tft.drawString((uint8_t*)"HI PWR", 122, 142,  2, WARN_LED); ATUB[BAND] = 0; break;
    case 'L': Tft.drawString((uint8_t*)"LO PWR", 122, 142,  2, WARN_LED); ATUB[BAND] = 0; break;
    case 'A': Tft.drawString((uint8_t*)"CANCEL", 122, 142,  2, GOOD_LED); ATUB[BAND] = 0; break;
    case 'T':
    case 'S': 
      Tft.drawString((uint8_t*)"TUNED", 128, 142,  2, GOOD_LED);
      ATUB[BAND] = 1;                                               // Update per-band array of ATU button state
      Serial3.setTimeout(50);
      Serial3.println("*F");
      Serial3.readBytesUntil(0x13, ATU_buff, 8);
      unsigned char RL_CH = (ATU_buff[0] - 48) * 100 + (ATU_buff[1] - 48) * 10 + (ATU_buff[2] - 48);
      int SWR = swr[RL_CH]/10;
      snprintf(RL_TXT, sizeof(RL_TXT), "%2d.%d", SWR/10, SWR % 10);
      Tft.LCD_SEL = 0;
      Tft.lcd_fill_rect(70, 203, 36, 16, BG_col);
      Tft.drawString((uint8_t*)RL_TXT, 70, 203, 2, vswr_col);
      strcpy(ORL_TXT, RL_TXT);
      break;
  }
  EEPROM.write(eeatub+BAND, ATUB[BAND]);                            // Update array in EEPROM 
  Switch_to_RX();                                                   // Return amp to RX state
  
}
