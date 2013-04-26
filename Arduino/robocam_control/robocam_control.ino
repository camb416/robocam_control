

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

int inputPins [] = {
  TILT_DOWN, TILT_UP, PAN_RIGHT, PAN_LEFT, ZOOM_DOWN, ZOOM_UP, FOCUS_LEFT, FOCUS_RIGHT};



// with these, 0 means nothing, 1 means left or down, 2 means right or up
byte focusState = 0;
byte zoomState = 0;
byte panState = 0;
byte tiltState = 0;

byte prevFocus = 0;
byte prevZoom = 0;
byte prevPan = 0;
byte prevTilt = 0;

// digital pin 2 has a pushbutton attached to it. Give it a name:
int pushButton = 8;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  for(int i=0;i< NUM_INPUTS; i++){
    pinMode(inputPins[i],INPUT_PULLUP);
  }
  // make the pushbutton's pin an input:
  //pinMode(pushButton, INPUT_PULLUP);
}

// the loop routine runs over and over again forever:
void loop() {
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
  Serial.println(msg);  
}

void doZoom(byte dir){
  // dir indicates direction
  // 0: error
  // 1: down
  // 2: up

  //TO DO: IMPLEMENT
  printStatus("zoom",dir, false);
}
void doFocus(byte dir){
  // dir indicates direction
  // 0: error
  // 1: left
  // 2:right
  //TO DO: IMPLEMENT
  printStatus("focus",dir,true);
}
void doPan(byte dir){
  // dir indicates direction
  // 0: error
  // 1: left
  // 2:right
  //TO DO: IMPLEMENT
  printStatus("pan",dir,true);
}
void doTilt(byte dir){
  // dir indicates direction
  // 0: error
  // 1: down
  // 2: up
  //TO DO: IMPLEMENT
  printStatus("tilt",dir,false);
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




