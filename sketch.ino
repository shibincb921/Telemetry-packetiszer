#define POT_PIN 4
#define BTN1 11
#define BTN2 12
#define BTN3 13
#define BTN4 14
int lastPotValue = -1;
uint8_t data[10];
uint8_t lastButtonValue = 0x00; 
const int POT_THRESHOLD = 20;
uint16_t mapped_voltage;

//function prototypes
int readPot(void);
uint8_t readButtonsAsBinary(void);

void setup() {
  // put your setup code here, to run once:
  pinMode(POT_PIN, INPUT);
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN4, INPUT_PULLUP);
  Serial.begin(115200);            //for debugging
  Serial1.begin(115200);           //for data transmission
  analogReadResolution(12);        
  analogSetAttenuation(ADC_11db);   
  Serial.println("Hello, ESP32-S3!");
  data[0]=0xFD; 
  data[1]=0xFC;
  data[2]=0xFB;
  data[3]=0xFA;
  data[4]=0x02;
  data[8]=0x04;
  data[9]=0x03;
  data[10]=0x02;
  data[11]=0x01;
}

void loop() {
  // put your main code here, to run repeatedly:
  bool sendData = false;
  int potValue = readPot();
  if (abs(potValue - lastPotValue) > POT_THRESHOLD) {
    lastPotValue = potValue;
    sendData = true;
  }
  uint8_t buttonValue = readButtonsAsBinary();
  if (buttonValue != lastButtonValue) {
    lastButtonValue = buttonValue;
    sendData = true;
  }
  if (sendData) {
    Serial.print("POT:");
    Serial.print(lastPotValue);
    mapped_voltage = (uint16_t)((lastPotValue / 4095.0) * 5.0*100.0); //scale of 0.01
    Serial.print("  BTN:");
    Serial.print(lastButtonValue, BIN);
    Serial.print(" MAP:");
    Serial.print(mapped_voltage);
    Serial.print("\n");
    data[5]=(mapped_voltage>>4)&0xFF; //mapped data is of 12 bits msb in data[5] 
    data[6]=((mapped_voltage<<4)&0xF0)|(lastButtonValue&0x0F);//mapped data lsb with button data in data[6]
    for(int i=0; i<=6;i++){
      data[7]^=data[i];               //xor checksum for first 7 bytes of data;
    }
    Serial1.write(data,sizeof(data));
  }
}
int readPot(void) {
  int sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(POT_PIN);
    delayMicroseconds(200);
  }
  return sum / 10;
}
uint8_t readButtonsAsBinary(void) {
  uint8_t value = 0;

  value |= (digitalRead(BTN1) == LOW) << 0;
  value |= (digitalRead(BTN2) == LOW) << 1;
  value |= (digitalRead(BTN3) == LOW) << 2;
  value |= (digitalRead(BTN4) == LOW) << 3;
  return value;
}