extern byte TMP_UNIT;
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// Draw text for selected menu item
void menuFunction(byte item, byte ch_dir){  
  Tft.LCD_SEL = 1;
  Tft.drawString((uint8_t*)item_disp[item], 65, 80, 2, BG_col);  // Erase previous item

  switch (item){
    
   case mACCbaud:
      if(XCVR == xft817) break;
      if (ch_dir  == 1) ACC_Baud++;
      else ACC_Baud--;
      if (ACC_Baud == 6) ACC_Baud = 0;
      if (ACC_Baud == 255) ACC_Baud = 5;
      EEPROM.write(eeaccbaud, ACC_Baud);
      Set_Ser2(ACC_Baud);
      break;
    
    case mTheme:
      Theme = ch_dir;
      EEPROM.write(eetheme, Theme);
      if (Theme){
        snprintf(item_disp[mTheme], IDSZ, "     NIGHT      ");
      }
      else {
        snprintf(item_disp[mTheme], IDSZ, "     NORMAL     ");
        }
      break;
      
     case mCELSIUS:
      TMP_UNIT = ch_dir;
      EEPROM.write(eecelsius, TMP_UNIT);
      snprintf(item_disp[mCELSIUS], IDSZ,
        "       &%c       ", TMP_UNIT ? 'C' : 'F');
      break;
      
    case mUSBbaud:
      if (ch_dir  == 1) USB_Baud++;
      else USB_Baud--;
      if (USB_Baud == 6) USB_Baud = 0;
      if (USB_Baud == 255) USB_Baud = 5;
      EEPROM.write(eeusbbaud, USB_Baud);
      Set_Ser(USB_Baud);
      break;

    case mXCVR:
      if (ch_dir  == 1) XCVR++;
      else XCVR--;
      if (XCVR == xcvr_max + 1) XCVR = 0;
      if (XCVR == 255) XCVR = xcvr_max;
      strcpy(item_disp[mXCVR], xcvr_disp[XCVR] );
      EEPROM.write(eexcvr, XCVR);
      SET_XCVR(XCVR);
      break;

    case mATTN:
      if (!ATTN_P){
        item_disp[mATTN] = (char*)" NO ATTENUATOR  ";
      }
      else{
        if (ATTN_ST != 0){ 
          ATTN_ST = 0;
          ATTN_ON_LOW;
          item_disp[mATTN] = (char*)" ATTENUATOR OUT ";
        }  
        else {
          ATTN_ST = 1;
          ATTN_ON_HIGH;
          item_disp[mATTN] = (char*)" ATTENUATOR IN  ";
        }
        EEPROM.write(eeattn, ATTN_ST);
        OD_alert = 5;
      }
      break;

    case mMCAL:
      if (ch_dir == 1) M_CORR++;
      else M_CORR--;
      if (M_CORR > 125) M_CORR = long(125);
      if (M_CORR < 75) M_CORR = long(75);
      byte MCAL = M_CORR;
      EEPROM.write(eemcal, MCAL);
      snprintf(item_disp[mMCAL], IDSZ,
        "      %3u       ", M_CORR);
      break;
    }
  delay(100);
  Tft.drawString((uint8_t*)item_disp[item], 65, 80, 2, ACT_TXT);        // Draw current menu item text in active text colour
}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void menuSelect(void){
  Tft.LCD_SEL = 1;
  if (menuSEL == 0){
    menuSEL = 1;
    Tft.drawString((uint8_t*)menu_items[menu_choice], 65, 20, 2, CHG_TXT);      // Menu item turns red when selected for edit
    Tft.lcd_fill_rect(14, 8, 41, 45, BG_col);                                // Menu decr/incr L/R arrows < > erased 
    Tft.lcd_fill_rect(266, 8, 41, 45, BG_col);
    if (menu_choice != mSETbias && menu_choice != mATWfwl){
      Tft.drawString((uint8_t*)item_disp[menu_choice], 65, 80, 2, ACT_TXT);   // Draw current menu item text in white
      DrawButton(14, 68, 40, 44);
      Tft.drawString((uint8_t*)"<", 24, 78,  4, FG_col);                     // Item decr/incr L/R arrows < > appear in LGRAY color
      DrawButton(266, 68, 40, 44);
      Tft.drawString((uint8_t*)">", 274, 78,  4, FG_col);
    }
    Tft.drawString((uint8_t*)"SELECT", 124, 132, 2, BG_col);                 // Black out stale SELECT text and display OK
    Tft.drawString((uint8_t*)"OK", 150, 132, 2, FG_col);
    if (menu_choice == mSETbias){
      oMODE = MODE;
      MODE = 1;
      MAX_CUR = 3;
      Bias_Meter = 1;
      Send_RLY(SR_DATA + 0x02);
      BIAS_ON
    }
  }
  else{
    Tft.drawString((uint8_t*)"OK", 150, 132, 2, BG_col);                      // Erase stale OK text
    menuSEL = 0;
    Tft.drawString((uint8_t*)menu_items[menu_choice], 65, 20, 2, ACT_TXT);
    Tft.drawString((uint8_t*)item_disp[menu_choice], 65, 80, 2, FG_col);
    Tft.lcd_fill_rect(14, 68, 41, 45, BG_col);                               // Item decr/incr L/R arrows < > erased
    Tft.lcd_fill_rect(266, 68, 41, 45, BG_col);
    DrawButton(14, 8, 40, 44);
    Tft.drawString((uint8_t*)"<", 24, 18,  4, FG_col);                       // Redraw Menu < > arrows
    DrawButton(266, 8, 40, 44);
    Tft.drawString((uint8_t*)">", 274, 18,  4, FG_col);
    if (menu_choice == mSETbias){
      BIAS_OFF
      Send_RLY(SR_DATA);
      MODE = oMODE;
      MAX_CUR = 20;
      Bias_Meter = 0;
      Tft.lcd_fill_rect(62, 70, 196, 40, BG_col);  
    }
    if (menu_choice == mATWfwl){
      Tft.drawString((uint8_t*)"  Updating ATU  ", 65, 80, 2, ACT_TXT);
      Serial.begin(19200);
      Serial3.println("*u");
      while(1){
        if (Serial.available()) Serial3.write(Serial.read());
        if (Serial3.available()) Serial.write(Serial3.read());
      }
    }
    Tft.drawString((uint8_t*)"SELECT", 124, 132, 2, FG_col);
  }
  
}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void Set_Ser2(byte BR){
  Serial2.end();
  switch (BR){
    case 0: Serial2.begin(4800);item_disp[mACCbaud] = (char*)"   4800 Baud    "; break;
    case 1: Serial2.begin(9600);item_disp[mACCbaud] = (char*)"   9600 Baud    "; break;
    case 2: Serial2.begin(19200);item_disp[mACCbaud] = (char*)"   19200 Baud   "; break;
    case 3: Serial2.begin(38400);item_disp[mACCbaud] = (char*)"   38400 Baud   "; break;
    case 4: Serial2.begin(57600);item_disp[mACCbaud] = (char*)"   57600 Baud   "; break;
    case 5: Serial2.begin(115200);item_disp[mACCbaud] = (char*)"  115200 Baud   "; break;
  }
}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void Set_Ser(byte BR){
  Serial.end();
  switch (BR){
    case 0: Serial.begin(4800);item_disp[mUSBbaud] = (char*)"   4800 Baud    "; break;
    case 1: Serial.begin(9600);item_disp[mUSBbaud] = (char*)"   9600 Baud    "; break;
    case 2: Serial.begin(19200);item_disp[mUSBbaud] = (char*)"   19200 Baud   "; break;
    case 3: Serial.begin(38400);item_disp[mUSBbaud] = (char*)"   38400 Baud   "; break;
    case 4: Serial.begin(57600);item_disp[mUSBbaud] = (char*)"   57600 Baud   "; break;
    case 5: Serial.begin(115200);item_disp[mUSBbaud] = (char*)"  115200 Baud   "; break;
  }
}
