
#include <avr/pgmspace.h>
#include <Wire.h>
#include "Reverb3.h"

#include "SparkFun_External_EEPROM.h" // Click here to get the library: http://librarymanager/All#SparkFun_External_EEPROM
ExternalEEPROM myMem;

#include <AceButton.h>
using namespace ace_button;
const int red_led = 8;
const int green_led = 10;
const int BUTTON_PIN = 7;
AceButton button(BUTTON_PIN);
bool busy(false);

void handleEvent(AceButton*, uint8_t, uint8_t);

#define PRESETLEN 512

void readpreset(uint8_t preset) {

  uint32_t offset = preset*PRESETLEN;
  char buf[10];

  Serial.print("Reading preset: ");
  Serial.print(preset);
  Serial.print(" memory offset: ");
  Serial.println(offset);
  Serial.println("");
  
  Serial.print("const uint8_t preset");
  Serial.print(preset);
  Serial.println("[] PROGMEM = {");
  for ( uint32_t x=offset; x < offset+PRESETLEN; x = x+16) {
    for( uint32_t y=0; y<16; y++) {
        byte val = myMem.read(x+y);
        sprintf(buf, "0x%02X, ", val);        
        Serial.print(buf);
    }
    Serial.println("");
  }
  Serial.println("};");
}

void writepreset(uint8_t preset, const uint8_t *data, size_t len) {
  
  uint32_t offset = preset*PRESETLEN;
  
  Serial.print("Writing preset: ");
  Serial.print(preset);
  Serial.print(" memory offset: ");
  Serial.print(offset);
  Serial.print(" length: ");
  Serial.println(len);

  for( size_t i=0; i<len; i++) {
    myMem.write(offset+i, pgm_read_byte(data+i));   
  }
}

void setup() {
  Serial.begin(115200);

  pinMode (BUTTON_PIN, INPUT_PULLUP); // push button on pin 2
  pinMode(green_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  digitalWrite(green_led, LOW);
  digitalWrite(red_led, LOW);
  
  
  // Get the current config.
  ButtonConfig* config = button.getButtonConfig();
  config->setFeature(ButtonConfig::kFeatureLongPress);
  config->setFeature(ButtonConfig::kFeatureClick);
  config->setEventHandler(handleEvent);

  Wire.begin();
  Wire.setClock(400000); //Most EEPROMs can run 400kHz and higher

  if (myMem.begin() == false)
  {
    Serial.println("No memory detected. Freezing.");
    while (1){
      digitalWrite(red_led, HIGH);
      delay(250);
      digitalWrite(red_led, LOW);
      delay(250);
    }
  } else {
    digitalWrite(green_led, HIGH);
    delay(1000);
    digitalWrite(green_led, LOW);
  }
  

  Serial.println("Memory detected!");
  
  // 24LC32A
  myMem.setMemorySize(32768/8); //In bytes. 32kbits 4kbytes
  myMem.setPageSize(32); //In bytes. Has 32 byte page size.
  myMem.enablePollForWriteComplete(); //Supports I2C polling of write completion
  myMem.setPageWriteTime(5); //5 ms max write time

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("Click on the button to read the eeprom");
  Serial.println("Long press the button to write the eeprom");
}

bool done = false;

void loop() {
  
  button.check();
}

void writeMemory(){
    Serial.println("Starting write");
    writepreset(0, preset0, PRESETLEN);
    writepreset(1, preset1, PRESETLEN);
    writepreset(2, preset2, PRESETLEN);
    writepreset(3, preset3, PRESETLEN);
    writepreset(4, preset4, PRESETLEN);
    writepreset(5, preset5, PRESETLEN);
    writepreset(6, preset6, PRESETLEN);
    writepreset(7, preset7, PRESETLEN);
    Serial.println("Write Complete!");
}

void readMemory(){
    Serial.println("Start read");
      
    readpreset(1);
    readpreset(2);
    readpreset(3);
    readpreset(4);
    readpreset(5);
    readpreset(6);
    readpreset(7);
      
    //done = true;
    Serial.println("Read Complete!");
}
void handleEvent(AceButton* /*button*/, uint8_t eventType,
    uint8_t /*buttonState*/) {
    switch (eventType) {
      case AceButton::kEventClicked:
          readMemory();
          break;
      case AceButton::kEventLongPressed:
          writeMemory();
          //Serial.println("Done, reset the board to start again");
          break;
    }
}
