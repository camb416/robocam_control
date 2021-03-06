/* ///////////////////////////////////////////
 *  ROBOCAM_CONTROL
 *  Feb 02-13-2013
 *  Cameron Browning
 *  Use WASD to Focus / Zoom
 *  Use Arrow Keys to Pan / Tilt
 *  A VC-C4 Camera
 *
 *////////////////////////////////////////////

import processing.serial.*;

Serial myPort;  // Create object from Serial class
int val;        // Data received from the serial port


int[] header;
int[] command;
int[] footer;

boolean isReady;

boolean newLine;


String header_str = "FF 30 30 00";
String hostControl_str = "90 30";
String footer_str = "EF";
String initialize_str = "58 30";

String panLeft = "53 32";
String panRight = "53 31";
String tiltUp = "53 33";
String tiltDown = "53 34";
String panTiltStop = "53 30";

String zoomIn = "A2 32";
String zoomOut = "A2 31";
String focusNear = "A1 32";
String focusFar = "A1 33";

String focusManual = "A1 31";
String zoomStop = "A2 30";

String lastHexString = "";


void setup() 
{

  newLine = true;

  size(200, 200);
  //println(Serial.list());
  String portName;
  int portNumber = -1;

  // if you're lucky:
  // portName = Serial.list()[0];

  // otherwise:
  portName = "/dev/tty.usbserial";

  String [] serialPorts = Serial.list();



  for (int i=0;i<serialPorts.length;i++) {
    //print(serialPorts[i]);
    if (serialPorts[i].equals(portName)) {
     // println("*"); 
      portNumber = i;
    } 
    else { 
     // println();
    }
  }
  

  
  if(portNumber == -1){
   println(portName + " not found, defaulting to: " + serialPorts[0]+".");
  portNumber = 0; 
  } else {
   println(portName + " found. "); 
  }


  portName = Serial.list()[portNumber];


  myPort = new Serial(this, portName, 9600);
  isReady = true;



  sendCommand(buildSerialString(hostControl_str));
  sendCommand(buildSerialString(initialize_str));


  /*
  // this works
   int [] controlstring = {0xFF, 0x30, 0x30, 0x00, 0x90, 0x30, 0xEF};
   for(int i=0;i<controlstring.length;i++){
   println(controlstring[i]); 
   myPort.write(controlstring[i]);
   }
   */
}

void draw() {


  if ( myPort.available() > 0) {  // If data is available,
    if (newLine) print("received:\t");
    newLine = false;
    int inByte = myPort.read();
    print (hex(inByte, 2));
    if (hex(inByte, 2).equals("EF")) {
      println(".");
      newLine = true;
    }
  } 
  else {
  }
}

void stop() {
  /* this is not calling :/ */
  println("exiting application, stopping the serial port.");
  myPort.stop();
  exit();
}


int [] buildSerialString(String commandString) {
  int [] returnArray;
  if (!commandString.equals(lastHexString)) {

    String wholeString = header_str + " " + commandString + " " + footer_str;




    String[] list = split(wholeString, " ");
    returnArray = new int[list.length];
    for (int i=0;i<list.length;i++) {

      returnArray[i] = int(unhex(list[i]));
    }


    lastHexString = commandString;
  } 
  else {
    returnArray = new int[0];
  }
  return returnArray;
}

void sendCommand(int [] serialString) {

  //  if(isReady){

  for (int i=0;i<serialString.length;i++) {
    myPort.write(serialString[i]);
  }
  isReady = false;
  //  }
}

void keyPressed() {

  if (key == CODED) {
    if (keyCode == UP) {
      sendCommand(buildSerialString(tiltUp));
    } 
    else if (keyCode == DOWN) {
      sendCommand(buildSerialString(tiltDown));
    } 
    else if (keyCode== LEFT) {
      sendCommand(buildSerialString(panLeft));
    } 
    else if (keyCode == RIGHT) {
      sendCommand(buildSerialString(panRight));
    }
  } 
  else {
    if (key=='a' || key == 'A') {
      sendCommand(buildSerialString(focusNear));
    } 
    else if (key == 'd' || key == 'D') {
      sendCommand(buildSerialString(focusFar));
    } 
    else if (key == 'w' || key == 'W') {
      sendCommand(buildSerialString(zoomIn));
    } 
    else if (key == 's' || key == 'S') {
      sendCommand(buildSerialString(zoomOut));
    } else {
     // any other key
    stop(); 
    }
    // do nothing
  }
}
void keyReleased() {

  if (key==CODED) {
    // arrow key released
    sendCommand(buildSerialString(panTiltStop));
  } 
  else {
    // it's a zoom or focus
    if (key=='w' || key == 'W' || key == 's' || key == 'S') {
      sendCommand(buildSerialString(zoomStop));
    } 
    else {
      sendCommand(buildSerialString(focusManual));
    }
  }
}

