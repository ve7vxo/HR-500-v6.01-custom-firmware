//===============================================================================================================
// Calculate forward or reflected or drive power, or calculate VSWR depending on passed pType byte
// Based on values updated in ISR every 1 millisecond.
// Return pResult

unsigned int Read_Power(byte pType){
  long pCalc, tCalc, bCalc;
  long pResult;
  
  if (pType == fwd_p){                                                          // for forward power
    if (!TX) return 0;
    if (analogRead(INPUT_RF) < 3) return 0;
    pCalc = long(f_tot) + long(30);
    pResult = (pCalc * pCalc) / long(809);
  }
  
  if (pType == rfl_p){                                                          // for reflected power
    if (!TX) return 0;
    if (analogRead(INPUT_RF) < 3) return 0;
    pCalc = long(r_tot) + long(140);
    pResult = (pCalc * pCalc) / long(10716);
  }
  
  if (pType == drv_p){                                                          // for drive power
    if (d_tot == 0) return 0;
    pCalc = long(d_tot) + long(30);
    pResult = (pCalc * pCalc) / long(10860);
  }
  
  if (pType == vswr){                                                           // for vswr
    
    if (!TX) return 0;
    if (f_tot < 100) return 0;
    if (TX && r_tot < 5) return 10;                                             // For very low vswr display 1.0
    tCalc = long(100) * long(f_tot) + long(30) * (r_tot) + long(2800);
    bCalc = long(10) * long(f_tot) - long(3) * long(r_tot) - long(280);
    pResult = swr_val.filter(tCalc / bCalc);                                    // Exponentially smooth swr value
    if (pResult < 10) pResult = 10;
    if (pResult > 99) pResult = 99;
  }
  return pResult;
}



//===============================================================================================================
// Returns the voltage in 25mV steps

unsigned int Read_Voltage(void){                                             
byte ADCvinMSB, ADCvinLSB;

  Wire.beginTransmission(LTCADDR);                                          // first get Input Voltage - 80V max
  Wire.write(0x1e);                                                         // Voltage in register address
  Wire.endTransmission(false);
  Wire.requestFrom(LTCADDR, 2, true);                                       // Read two bytes
  delay(1);
  ADCvinMSB = Wire.read();                                                  // Read MSB
  ADCvinLSB = Wire.read();                                                  // Read LSB
  return((unsigned int)(ADCvinMSB) << 4) + ((ADCvinLSB >> 4) & 0x0F);       // formats into 12bit integer  
}



//===============================================================================================================
// Returns the current in 5mA steps

unsigned int Read_Current(void){                                            
byte curSenseMSB, curSenseLSB;
  Wire.beginTransmission(LTCADDR);                                          // Get sense current
  Wire.write(0x14);                                                         // Delta sense register address
  Wire.endTransmission(false);
  Wire.requestFrom(LTCADDR, 2, true);                                       // Read two bytes
  delay(1);
  curSenseMSB = Wire.read();                                                // Read MSB
  curSenseLSB = Wire.read();                                                // Read LSB

  return ((unsigned int)(curSenseMSB) << 4) + ((curSenseLSB >> 4) & 0x0F);  // 12 bit format

}




//===============================================================================================================
// Clear any existing faults in LTC2945 power monitor chip

void trip_clear(void){
  Wire.beginTransmission(LTCADDR);                                         
  Wire.write(0x04);                                                         // Fault CoR register address
  Wire.endTransmission(false);
  Wire.requestFrom(LTCADDR, 2, true);                                       // Request two bytes (clears any existing fault)
  delay(1);
  Wire.read();
  delay(10);
  Wire.beginTransmission(LTCADDR);                                          // set alert register
  Wire.write(0x01);
  Wire.write(0x02);
  Wire.endTransmission();
  Wire.requestFrom(LTCADDR, 2, true);
  delay(1);
}



//===============================================================================================================
// Establish a fault condition which causes LTC2945 to latch out DC power to the PA.

void trip_set(void){
  Wire.beginTransmission(LTCADDR);                                        
  Wire.write(0x03);                                                         // Select fault register
  Wire.write(0x02);                                                         // Write overvalue fault.
  Wire.endTransmission();
  Wire.requestFrom(LTCADDR, 2, true);
  delay(1);
  BIAS_OFF
  Send_RLY(SR_DATA);
  RF_BYPASS
  trip = 1;
  TX = 0;
  DrawMode();
  Tft.LCD_SEL = 0;
}
