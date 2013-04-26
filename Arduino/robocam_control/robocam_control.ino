

#define TILT_DOWN 4
#define TILT_UP 5
#define PAN_RIGHT 6
#define PAN_LEFT 7

#define ZOOM_DOWN 8 
#define ZOOM_UP 9

#define FOCUS_RIGHT 10
#define FOCUS_LEFT 11

#define LED 13

#define NUM_INPUTS 8


// if something goes wrong, set this to true and plug the arduino into a serial monitor
// just dont forget that you need to take the RS232 shield off. Then remember to set this
// to false in production
#define DEBUG_MODE false

// if you set this to true, the pedestal will revert to the center
// when the power is cycled on the Robocam Control hardware.
#define DAILY_RESET false

int inputPins [] = {
  TILT_DOWN, TILT_UP, PAN_RIGHT, PAN_LEFT, ZOOM_DOWN, ZOOM_UP, FOCUS_LEFT, FOCUS_RIGHT};

int messageLength;

// CAMERA MESSAGES

// set host control
byte hostControl_bytes [] = {
  0xFF,0x30,0x30,0x00,0x90,0x30,0xEF};
// cascade off (no multicam control)
byte cascadeOff0_bytes [] = {
  0xFF, 0x30 , 0x30 , 0x00 , 0x8F , 0x30 , 0xEF};
byte cascadeOff1_bytes [] = {
  0xFF, 0x30 , 0x30 , 0x00 , 0x8F , 0x31 , 0xEF};
// Auto exposure
byte autoExposure_bytes [] = {
  0xFF , 0x30 , 0x30 , 0x00 , 0xA5 , 0x32 , 0xEF};
// white balance normal
byte whiteBalNormal_bytes [] = {
  0xFF , 0x30 , 0x30 , 0x00 , 0xA7 , 0x30 , 0xEF};
// pedestal init 1 (center camera)
byte pedestalInit1_bytes [] = {
  0xFF , 0x30 , 0x31 , 0x00 , 0x58 , 0x30 , 0xEF};
// pedestal init 2 (return to original position)
byte pedestalInit2_bytes [] = {
  0xFF , 0x30 , 0x31 , 0x00 , 0x58 , 0x31 , 0xEF};
// pan left
byte panLeft_bytes [] = {
  0xFF , 0x30 , 0x31 , 0x00 , 0x53 , 0x32 , 0xEF};
// pan right
byte panRight_bytes [] = {
  0xFF , 0x30 , 0x31 , 0x00 , 0x53 , 0x31 , 0xEF};
// pan/tilt stop
byte panTiltStop_bytes [] = {
  0xFF , 0x30 , 0x31 , 0x00 , 0x60 , 0x30, 0x30 , 0xEF};
// tilt up
byte tiltUp_bytes [] = {
  0xFF , 0x30 , 0x31 , 0x00 , 0x53 , 0x33 , 0xEF};
// tilt down
byte tiltDown_bytes [] = {
  0xFF , 0x30 , 0x31 , 0x00 , 0x53 , 0x34 , 0xEF};
// zoom in
byte zoomTele_bytes [] = {
  0xFF , 0x30 , 0x31 , 0x00 , 0xA2 , 0x32 , 0xEF};
// zoom out
byte zoomWide_bytes [] = {
  0xFF , 0x30 , 0x31 , 0x00 , 0xA2 , 0x31 , 0xEF};
// zoom stop
byte zoomStop_bytes [] = {
  0xFF , 0x30 , 0x31 , 0x00 , 0xA2 , 0x30 , 0xEF};
// focus near
byte focusNear_bytes [] = {
  0xFF , 0x30 , 0x31 , 0x00 , 0xA1 , 0x32 , 0xEF};
// focus far
byte focusFar_bytes [] = {
  0xFF , 0x30 , 0x31 , 0x00 , 0xA1 , 0x33 , 0xEF};
// focus stop
byte focusStop_bytes [] = {
  0xFF , 0x30 , 0x31 , 0x00 , 0xA1 , 0x31 , 0xEF};

// with these, 0 means nothing, 1 means left or down, 2 means right or up
byte focusState = 0;
byte zoomState = 0;
byte panState = 0;
byte tiltState = 0;

byte prevFocus = 0;
byte prevZoom = 0;
byte prevPan = 0;
byte prevTilt = 0;


byte incomingByte;


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:

  messageLength = sizeof(hostControl_bytes);

  for(int i=0;i< NUM_INPUTS; i++){
    pinMode(inputPins[i],INPUT_PULLUP);
  }
  pinMode(LED,OUTPUT);
  digitalWrite(LED,HIGH);

  Serial.begin(9600);


  waitForSerial();




  initializeCamera();
  digitalWrite(LED,LOW);
  // make the pushbutton's pin an input:
  //pinMode(pushButton, INPUT_PULLUP);
}

// the loop routine runs over and over again forever:
void loop() {
  // waitForSerial();
  // read the input pin:
  //int buttonState = digitalRead(pushButton);
  // print out the state of the button:
  // Serial.println(buttonState);
  //if(digitalRead(PAN_LEFT)==LOW){
  // Serial.println("pan left"); 
  //}

  monitorLeftStick();
  monitorRightStick();

  if(isAnyStickInUse()){
    // TO DO: light up the led 
  }
  delay(10);        // delay in between reads for stability

  // TO DO: turn off the led

}

boolean isAnyStickInUse(){
  int stateTotals = focusState+zoomState+panState+tiltState;
  if(stateTotals>0){
    return true;
  }  
  else {
    return false; 
  }
}



void printStatus(String action, byte dir, boolean xaxis){
  String msg = action;
  String oneMsg, twoMsg;
  if(xaxis){
    oneMsg = " left";
    twoMsg = " right"; 
  } 
  else {
    oneMsg = " down";
    twoMsg = " up";
  }
  dir == 0 ? msg += " stop" : dir == 1 ? msg += oneMsg : msg += twoMsg;
  if(DEBUG_MODE) Serial.println(msg);  
}

void doZoom(byte dir){
  // dir indicates direction
  // 0: error
  // 1: down
  // 2: up

  //TO DO: IMPLEMENT
  printStatus("zoom",dir, false);
    switch(zoomState){
  case 0:
    Serial.write(zoomStop_bytes,sizeof(zoomStop_bytes));
    break;
  case 1:
    Serial.write(zoomWide_bytes,sizeof(zoomWide_bytes));
    break;
  case 2:
    Serial.write(zoomTele_bytes,sizeof(zoomTele_bytes));
    break;
  }
}
void doFocus(byte dir){
  // dir indicates direction
  // 0: error
  // 1: left
  // 2:right
  //TO DO: IMPLEMENT
  printStatus("focus",dir,true);
  switch(focusState){
  case 0:
    Serial.write(focusStop_bytes,sizeof(focusStop_bytes));
    break;
  case 1:
    Serial.write(focusNear_bytes,sizeof(focusNear_bytes));
    break;
  case 2:
    Serial.write(focusFar_bytes,sizeof(focusFar_bytes));
    break;
  }

}
void doPan(byte dir){
  // dir indicates direction
  // 0: error
  // 1: left
  // 2:right
  //TO DO: IMPLEMENT
  printStatus("pan",dir,true);

  switch(panState){
  case 0:
    Serial.write(panTiltStop_bytes,sizeof(panTiltStop_bytes));
    break;
  case 1:
    Serial.write(panLeft_bytes,sizeof(panLeft_bytes));
    break;
  case 2:
    Serial.write(panRight_bytes,sizeof(panRight_bytes));
    break;
  }

}
void doTilt(byte dir){
  // dir indicates direction
  // 0: error
  // 1: down
  // 2: up
  //TO DO: IMPLEMENT
  printStatus("tilt",dir,false);
  switch(tiltState){
  case 0:
    Serial.write(panTiltStop_bytes,sizeof(panTiltStop_bytes));
    break;
  case 1:
    Serial.write(tiltDown_bytes,sizeof(tiltDown_bytes));
    break;
  case 2:
    Serial.write(tiltUp_bytes,sizeof(tiltUp_bytes));
    break;
  }
}

void monitorLeftStick(){
  // Zoom and Focus

  //  byte prevZoom = zoomState;
  //  byte prevFocus = focusState;

  // remember, reverse logic
  if(!digitalRead(ZOOM_DOWN)){
    zoomState = 1;
  } 
  else if(!digitalRead(ZOOM_UP)){
    zoomState = 2; 
  } 
  else {
    zoomState = 0; 
  }


  if(!digitalRead(FOCUS_LEFT)){
    focusState = 1;
  } 
  else if(!digitalRead(FOCUS_RIGHT)){
    focusState = 2; 
  } 
  else {
    focusState = 0; 
  }

  if(prevZoom!=zoomState){
    doZoom(zoomState); 
  }
  if(prevFocus!=focusState){
    doFocus(focusState); 
  }

  prevZoom = zoomState;
  prevFocus = focusState;


}
void monitorRightStick(){
  // Pan and tilt

  //  byte prevTilt = tiltState;
  //  byte prevPan = panState;

  // remember, reverse logic
  if(!digitalRead(TILT_DOWN)){
    tiltState = 1;
  } 
  else if(!digitalRead(TILT_UP)){
    tiltState = 2; 
  } 
  else {
    tiltState = 0; 
  }
  if(!digitalRead(PAN_LEFT)){
    panState = 1;
  } 
  else if(!digitalRead(PAN_RIGHT)){
    panState = 2; 
  } 
  else {
    panState = 0; 
  }

  if(prevTilt!=tiltState){
    doTilt(tiltState); 
  }
  if(prevPan!=panState){
    doPan(panState); 
  }

  prevPan = panState;
  prevTilt = tiltState;


}

void initializeCamera(){
  writeBytes(hostControl_bytes);
  delay(500);
  writeBytes(cascadeOff0_bytes);
  delay(500);
  writeBytes(cascadeOff1_bytes);
  delay(500);
  writeBytes(autoExposure_bytes);
  delay(500);
  writeBytes(whiteBalNormal_bytes);
  delay(500);
  if(DAILY_RESET){
    writeBytes(pedestalInit1_bytes);
  } 
  else {
    writeBytes(pedestalInit2_bytes);
  }
}
void writeBytes(byte * msg){
  Serial.write(msg,messageLength);
}
void waitForSerial(){
  while (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
    //Serial.print("I received: ");
    //Serial.println(incomingByte, DEC);
    // do nothing...

  } 
}








