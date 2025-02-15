// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Draws home screen for right side display adding buttons for ATU
// if present

void DrawHome(void){
  Tft.LCD_SEL = 1;
  DrawButton(5, 65, 118, 50);             // MODE button
  DrawButton(197, 65, 58, 50);            // < arrow
  DrawButton(257, 65, 58, 50);            // > arrow
  DrawButton(5, 185, 88, 50);             // ANT select
  DrawButton(227, 185, 88, 50);           // ATU button
  if (!ATU_P) {
    DrawButton(130, 135, 60, 30);         // Menu button (no ATU)
  }
  else {
    DrawButton(130, 85, 60, 30);          // ATU and menu buttons
    DrawButton(116, 185, 88, 50);
    DrawPanel(116, 128, 88, 50);
  }
  DrawPanel(5, 8, 118, 50);               // Mode display text window
  DrawPanel(197, 8, 118, 50);             // Band display text window
  DrawPanel(5, 128, 88, 50);              // Antenna display text window
  DrawPanel(227, 128, 88, 50);            // ATU display text window
  DrawPanel(140, 8, 40, 20);              // TX/RX staus LED
  DrawTxPanel(GOOD_LED);
  DrawRxButtons(FG_col);
  DrawMode();
  if (BAND != 0) DrawBand(BAND, ACT_TXT);
  else DrawBand(BAND, RED);
  DrawAnt();
  DrawATU();
}



// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Draws buttons and text for right display and updates via menu pointer

void DrawMenu(void){
  Tft.LCD_SEL = 1;
  DrawButton(130, 205, 60, 30);
  Tft.drawString((uint8_t*)"EXIT", 135, 213,  2, FG_col);
  DrawButton(14, 8, 40, 44);
  Tft.drawString((uint8_t*)"<", 24, 18,  4, FG_col);
  DrawButton(266, 8, 40, 44);
  Tft.drawString((uint8_t*)">", 274, 18,  4, FG_col);
  DrawPanel(60, 8, 200, 44);
  DrawPanel(60, 68, 200, 44);
  Tft.drawString((uint8_t*)menu_items[menu_choice], 65, 20, 2, ACT_TXT);
  Tft.drawString((uint8_t*)item_disp[menu_choice], 65, 80, 2, FG_col);
  DrawButton(120, 125, 80, 30);
  Tft.drawString((uint8_t*)"SELECT", 124, 132, 2, FG_col);
  menuSEL = 0;
  Tft.drawString((uint8_t*)"ATU:", 206, 190, 2, FG_col);
  Tft.drawString((uint8_t*)ATU_ver, 254, 190, 2, FG_col);
  Tft.drawString((uint8_t*)"FW:", 206, 213, 2, FG_col);
  Tft.drawString((uint8_t*)VERSION, 244, 213, 2, FG_col);
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Draws leftt hand screen button text
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Draws status indicator and text in buttons for FWD,RFL,DRV,VDD,IDD
// Also draws text for DCP, Eff, SWR and temperature indicators.

void DrawMeter(void){
  Tft.LCD_SEL = 0;

  DrawPanel(7, 8, 304, 64);                                       // Draws bargraph meter area


  DrawPanel(18, 80, 29, 14);                                      // Draw status LED frames
  DrawPanel(82, 80, 29, 14);
  DrawPanel(146, 80, 29, 14);
  DrawPanel(210, 80, 29, 14);
  DrawPanel(274, 80, 29, 14);
  
  
  DrawButton(10, 100, 44, 30);                                    // Draw buttons and internal text
  Tft.drawString((uint8_t*)"FWD", 13, 109, 2, FG_col);
  DrawButton(74, 100, 44, 30);
  Tft.drawString((uint8_t*)"RFL", 77, 109, 2, FG_col);
  DrawButton(138, 100, 44, 30);
  Tft.drawString((uint8_t*)"DRV", 141, 109, 2, FG_col);
  DrawButton(202, 100, 44, 30);
  Tft.drawString((uint8_t*)"VDD", 205, 109, 2, FG_col);
  DrawButton(266, 100, 44, 30);
  Tft.drawString((uint8_t*)"IDD", 269, 109, 2, FG_col);
  
  DrawButtonDn(MeterSel);                                         // Draw selected meter button
  
  Tft.drawString((uint8_t*)"DCP:", 10, 155, 2, FG_col);
  DrawPanel(63, 146, 80, 30);
  Tft.drawString("W", 118, 153, 2, ACT_TXT);

  Tft.drawString((uint8_t*)"Eff:", 165, 155, 2, FG_col);
  DrawPanel(230, 146, 80, 30);
  Tft.drawString("%", 280, 153, 2, ACT_TXT);

  Tft.drawString((uint8_t*)"SWR:", 10, 205, 2, FG_col);
  DrawPanel(63, 196, 80, 30);
  Tft.drawString((uint8_t*)"TEMP:", 165, 205, 2, FG_col);
  DrawPanel(230, 196, 80, 30);
}



// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Draws right hand screen button text

void DrawRxButtons(uint16_t bcolor){
  if (SCREEN != 0) return;
  Tft.LCD_SEL = 1;
  
  if(MODE == 1 && !trip)Tft.drawString((uint8_t*)"ON", 36, 21,  3, ACT_TXT);

  Tft.drawString((uint8_t*)"MODE", 26, 79,  3, bcolor);
  Tft.drawString((uint8_t*)"<", 219, 79,  3, bcolor);
  Tft.drawString((uint8_t*)">", 279, 79,  3, bcolor);
  Tft.drawString((uint8_t*)"ANT", 21, 199,  3, bcolor);
  if (!ATU_P) {
    Tft.drawString((uint8_t*)"MENU", 135, 143,  2, bcolor);                       // With no ATU draw menu button higher
  }
  else {
    Tft.drawString((uint8_t*)"MENU", 135, 93,  2, bcolor);                        // With ATU draw menu button  
    Tft.drawString((uint8_t*)"TUNE", 122, 199,  3, bcolor);                       // and TUNE button below it
  }
  if (!ATU_P) {
    bcolor = FG_col;   
    Tft.drawString((uint8_t*)"ATU", 244, 199,  3, bcolor);                        // ATU text colour changes if present
  }
  else {
    bcolor = ACT_TXT;   
    Tft.drawString((uint8_t*)"ATU", 244, 199,  3, bcolor);    
  }  
}



// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Draws rectangle for button and fills background colour

void DrawButton(int x, int y, int w, int h){
  if (h < 8 ||w < 8) return;
  Tft.lcd_draw_v_line(x, y, h, LT_BTN);
  Tft.lcd_draw_v_line(x+1, y+1, h-2, LT_BTN);
  Tft.lcd_draw_v_line(x+w, y, h, DK_BTN);
  Tft.lcd_draw_v_line(x+w-1, y+1, h-2, DK_BTN);
  Tft.lcd_draw_h_line(x, y, w, LT_BTN);
  Tft.lcd_draw_h_line(x+1, y+1, w-2, LT_BTN);
  Tft.lcd_draw_h_line(x, y+h, w, DK_BTN);
  Tft.lcd_draw_h_line(x+1, y+h-1, w-2, DK_BTN);
  Tft.lcd_fill_rect(x+2, y+2, w-4, h-4,  BG_col);  // All button background colour
}



// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Draw button down with dark highight on right and lower sides
// Change selected button text to active and others to passive
// Draw meter and scale marks with units corresponding to selected meter

void DrawButtonDn(int button){
  int x = (button - 1) * 64 + 10;
  int y = 11;
  int w = 44;
  int h = 30;
  int l_start = 32, l_height = 11, l_color = MTR_MRK;
  
  Tft.LCD_SEL = 0;
  Tft.lcd_draw_v_line(x, y, h, DK_BTN);                                     
  Tft.lcd_draw_v_line(x+1, y+1, h-2, DK_BTN);
  Tft.lcd_draw_v_line(x+w, y, h, LT_BTN);
  Tft.lcd_draw_v_line(x+w-1, y+1, h-2, LT_BTN);
  Tft.lcd_draw_h_line(x, y, w, DK_BTN);
  Tft.lcd_draw_h_line(x+1, y+1, w-2, DK_BTN);
  Tft.lcd_draw_h_line(x, y+h, w, LT_BTN);
  Tft.lcd_draw_h_line(x+1, y+h-1, w-2, LT_BTN);
  Tft.lcd_fill_rect(9, 10, 301, 34, MTR_SCL);                                 // Background colour of meter scale

  
  if (button == 1 || button == 2 || button == 3){                             // button for FWD, REV or DRV power
    for (int i = 19; i < 300; i += 56){
      Tft.lcd_draw_v_line(i, l_start-7, l_height+7, l_color);                 // Large scale mark
    }
    for (int i = 47; i < 300; i += 56){
      Tft.lcd_draw_v_line(i, l_start, l_height, l_color);                     // Small scale mark
    }
    if (button == 1){                                                         // Forward power scale display
      Tft.drawString((uint8_t*)"RFL", 77, 109, 2, PSV_TXT);         
      Tft.drawString((uint8_t*)"DRV", 141, 109, 2, PSV_TXT);
      Tft.drawString((uint8_t*)"VDD", 205, 109, 2, PSV_TXT);
      Tft.drawString((uint8_t*)"IDD", 269, 109, 2, PSV_TXT);
      Tft.drawString((uint8_t*)"FWD", 13, 109, 2, ACT_TXT);
      Tft.lcd_display_string(16, 9, (uint8_t *)"0", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(30, 11, (uint8_t *)"WATTS", FONT_1206, MTR_TXT);
      Tft.lcd_display_string(64, 9, (uint8_t *)"100", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(120, 9, (uint8_t *)"200", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(176, 9, (uint8_t *)"300", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(232, 9, (uint8_t *)"400", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(286, 9, (uint8_t *)"500", FONT_1608, MTR_MRK);
    }
    if (button == 2){                                                        // Reflected power scale display
      Tft.drawString((uint8_t*)"FWD", 13, 109, 2, PSV_TXT);
      Tft.drawString((uint8_t*)"DRV", 141, 109, 2, PSV_TXT);
      Tft.drawString((uint8_t*)"VDD", 205, 109, 2, PSV_TXT);
      Tft.drawString((uint8_t*)"IDD", 269, 109, 2, PSV_TXT);
      Tft.drawString((uint8_t*)"RFL", 77, 109, 2, ACT_TXT);
      Tft.lcd_display_string(16, 9, (uint8_t *)"0", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(30, 11, (uint8_t *)"WATTS", FONT_1206, MTR_TXT);
      Tft.lcd_display_string(67, 9, (uint8_t *)"10", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(123, 9, (uint8_t *)"20", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(179, 9, (uint8_t *)"30", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(235, 9, (uint8_t *)"40", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(291, 9, (uint8_t *)"50", FONT_1608, MTR_MRK);
    }
    if (button == 3){                                                         // Drive power scale display
      Tft.drawString((uint8_t*)"FWD", 13, 109, 2, PSV_TXT);
      Tft.drawString((uint8_t*)"RFL", 77, 109, 2, PSV_TXT);
      Tft.drawString((uint8_t*)"VDD", 205, 109, 2, PSV_TXT);
      Tft.drawString((uint8_t*)"IDD", 269, 109, 2, PSV_TXT);
      Tft.drawString((uint8_t*)"DRV", 141, 109, 2, ACT_TXT);
      Tft.lcd_display_string(16, 9, (uint8_t *)"0", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(30, 11, (uint8_t *)"WATTS", FONT_1206, MTR_TXT);
      Tft.lcd_display_string(72, 9, (uint8_t *)"2", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(128, 9, (uint8_t *)"4", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(184, 9, (uint8_t *)"6", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(240, 9, (uint8_t *)"8", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(291, 9, (uint8_t *)"10", FONT_1608, MTR_MRK);
    }
  }
  l_start = 27; l_height = 15;
  if (button == 4 || button == 5){                                          
    if (button == 4){                                                       // Voltage scale display
      Tft.drawString((uint8_t*)"FWD", 13, 109, 2, PSV_TXT);
      Tft.drawString((uint8_t*)"RFL", 77, 109, 2, PSV_TXT);
      Tft.drawString((uint8_t*)"DRV", 141, 109, 2, PSV_TXT);
      Tft.drawString((uint8_t*)"IDD", 269, 109, 2, PSV_TXT);
      Tft.drawString((uint8_t*)"VDD", 205, 109, 2, ACT_TXT);
      for (int i = 19; i < 300; i += 28){
        Tft.lcd_draw_v_line(i, l_start, l_height, l_color);
      }
      l_start = 31; l_height = 12;
      Tft.lcd_display_string(16, 9, (uint8_t *)"0", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(30, 11, (uint8_t *)"VOLTS", FONT_1206, MTR_TXT);
      Tft.lcd_display_string(67, 9, (uint8_t *)"12", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(123, 9, (uint8_t *)"24", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(179, 9, (uint8_t *)"36", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(235, 9, (uint8_t *)"48", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(291, 9, (uint8_t *)"60", FONT_1608, MTR_MRK);
    }
    if (button == 5){                                                       // Amperage scale display
      Tft.drawString((uint8_t*)"FWD", 13, 109, 2, PSV_TXT);
      Tft.drawString((uint8_t*)"RFL", 77, 109, 2, PSV_TXT);
      Tft.drawString((uint8_t*)"DRV", 141, 109, 2, PSV_TXT);
      Tft.drawString((uint8_t*)"VDD", 205, 109, 2, PSV_TXT);
      Tft.drawString((uint8_t*)"IDD", 269, 109, 2, ACT_TXT);
      for (int i = 19; i < 300; i += 14){
        Tft.lcd_draw_v_line(i, l_start, l_height, l_color);
      }
      Tft.lcd_display_string(16, 9, (uint8_t *)"0", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(40, 11, (uint8_t *)"AMPS", FONT_1206, MTR_TXT);
      Tft.lcd_display_string(86, 9, (uint8_t *)"5", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(151, 9, (uint8_t *)"10", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(221, 9, (uint8_t *)"15", FONT_1608, MTR_MRK);
      Tft.lcd_display_string(291, 9, (uint8_t *)"20", FONT_1608, MTR_MRK);
    }
  }
}



// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Draw button up with dark highight on left and upper sides

void DrawButtonUp(int button){
  int x = (button - 1) * 64 + 10;
  int y = 100;
  int w = 44;
  int h = 30;
  
  Tft.LCD_SEL = 0;
  Tft.lcd_draw_v_line(x, y, h, LT_BTN);
  Tft.lcd_draw_v_line(x+1, y+1, h-2, LT_BTN);
  Tft.lcd_draw_v_line(x+w, y, h, DK_BTN);
  Tft.lcd_draw_v_line(x+w-1, y+1, h-2, DK_BTN);
  Tft.lcd_draw_h_line(x, y, w, LT_BTN);
  Tft.lcd_draw_h_line(x+1, y+1, w-2, LT_BTN);
  Tft.lcd_draw_h_line(x, y+h, w, DK_BTN);
  Tft.lcd_draw_h_line(x+1, y+h-1, w-2, DK_BTN);
}



// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Draw panel with highlighted edges 2 pixels thick
// and fill panel with background colour

void DrawPanel(int x, int y, int w, int h){
  if (w < 8 || h < 8) return;
  Tft.lcd_draw_v_line(x, y, h, DK_BTN);
  Tft.lcd_draw_v_line(x+1, y+1, h-2, DK_BTN);
  Tft.lcd_draw_v_line(x+w, y, h, LT_BTN);
  Tft.lcd_draw_v_line(x+w-1, y+1, h-2, LT_BTN);
  Tft.lcd_draw_h_line(x, y, w, DK_BTN);
  Tft.lcd_draw_h_line(x+1, y+1, w-2, DK_BTN);
  Tft.lcd_draw_h_line(x, y+h, w, LT_BTN);
  Tft.lcd_draw_h_line(x+1, y+h-1, w-2, LT_BTN);
  Tft.lcd_fill_rect(x+2, y+2, w-3, h-3, BG_col);                             // Background of panel
}



// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Fills TX indicator with red or green depending on PTT level

void DrawTxPanel(uint16_t pcolor){  
  if (SCREEN != 0) return;
  Tft.LCD_SEL = 1;
  Tft.lcd_fill_rect(142, 10, 36, 16, pcolor);                               // Fill TX/RX led with TX/RX state colour
}



// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Changes mode display text with mode changes

void DrawMode(void){
  if (SCREEN != 0) return;
  Tft.LCD_SEL = 1;
  if (trip || fault) {                                                    // Erase "ON" and flash "TRIP" message if tripped
    fault = 1;                                                            // Trip condition is latched
    Tft.lcd_fill_rect(142, 10, 36, 16, DGRAY);                            // Fill TX/RX led with OFF state colour  
    if (trip) Tft.drawString((uint8_t*)"ON", 36, 21,  3, BG_col);         
    
    if (fault) {
      Tft.drawString((uint8_t*)"TRIP", 36, 21,  3, BG_col);
      delay(200);
      Tft.drawString((uint8_t*)"TRIP", 36, 21,  3, ALRM_LED);
      delay(200);
  }
  }
  else if (MODE == 0 && trip == 0) {                                      // Amplifier off
    Tft.lcd_fill_rect(26, 21, 90, 21, BG_col);
    Tft.drawString((uint8_t*)"OFF", 36, 21,  3, DGRAY);
    Tft.lcd_fill_rect(142, 10, 36, 16, DGRAY);                            // Fill TX/RX led with OFF state colour
  }
  else if (MODE == 1 && trip == 0) {                                      // Amplifier is ON display it
    Tft.lcd_fill_rect(26, 21, 90, 21, BG_col);
    Tft.drawString((uint8_t*)"ON", 36, 21,  3, YELLOW);
    Tft.lcd_fill_rect(142, 10, 36, 16, GOOD_LED);                         // Fill TX/RX led with ON/RX state colour
  }
}



// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Changes band display text with band changes

void DrawBand(byte Band, uint16_t acolor){
  if (SCREEN != 0) return;
  Tft.LCD_SEL = 1;
  if (ANTSEL[BAND] == 1) acolor = A1_TXT;
  if (ANTSEL[BAND] == 2) acolor = A2_TXT;
  Tft.lcd_fill_rect(228, 21, 80, 21, BG_col);
  if (Band == 0) Tft.drawString((uint8_t*)"UNK", 228, 21,  3, DK_BTN);
  else if (Band == 1) Tft.drawString((uint8_t*)"10M", 228, 21,  3, acolor);
  else if (Band == 2) Tft.drawString((uint8_t*)"12M", 228, 21,  3, acolor);
  else if (Band == 3) Tft.drawString((uint8_t*)"15M", 228, 21,  3, acolor);
  else if (Band == 4) Tft.drawString((uint8_t*)"17M", 228, 21,  3, acolor);
  else if (Band == 5) Tft.drawString((uint8_t*)"20M", 228, 21,  3, acolor);
  else if (Band == 6) Tft.drawString((uint8_t*)"30M", 228, 21,  3, acolor);
  else if (Band == 7) Tft.drawString((uint8_t*)"40M", 228, 21,  3, acolor);
  else if (Band == 8) Tft.drawString((uint8_t*)"60M", 228, 21,  3, acolor);
  else if (Band == 9) Tft.drawString((uint8_t*)"80M", 228, 21,  3, acolor);
  else if (Band == 10) Tft.drawString((uint8_t*)"160M", 228, 21,  3, acolor);
  else return;
}



// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Changes antenna display text and colour as selected

void DrawAnt(void){
  if (SCREEN != 0) return;
  Tft.LCD_SEL = 1;
  Tft.lcd_fill_rect(43, 142, 16, 21, BG_col);                                // Erase stale antenna text
  if (ANTSEL[BAND] == 1) {
    acolor = A1_TXT;
    Tft.drawString((uint8_t*)"1", 41, 142,  3, acolor);
    Serial3.println("*N1");
    SEL_ANT1;
  }
  if (ANTSEL[BAND] == 2) {
    acolor = A2_TXT;
    Tft.drawString((uint8_t*)"2", 41, 142,  3, acolor);
    Serial3.println("*N2");
    SEL_ANT2;
  }
  DrawBand(BAND, acolor);                                                   // Display band text in antenna color
  
  if (ATU_P){
    Tft.LCD_SEL = 1;
    Tft.lcd_fill_rect(121, 142, 74, 21, BG_col);                            // Erase stale ATU text window when present
  }

}



// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Changes ATU status display text if ATU present

void DrawATU(void){
  if (SCREEN != 0) return;
  Tft.LCD_SEL = 1;
  Tft.lcd_fill_rect(244, 142, 54, 21, BG_col);                              // Erase stale ATU state text
  if (!ATU_P) Tft.drawString((uint8_t*)"N/A", 244, 142,  3, DGRAY);         // Grey out if not present
  else{
    if (ATUB[BAND] == 0) {
      Tft.drawString((uint8_t*)"BYP", 244, 142,  3, GOOD_LED);              // Display BYP 
      Serial3.println("*Y1");
    }
    if (ATUB[BAND] == 1) {
      Tft.drawString((uint8_t*)"ON", 244, 142,  3, WARN_LED);               // Display ON
      Serial3.println("*Y0");
    }
  }
}
