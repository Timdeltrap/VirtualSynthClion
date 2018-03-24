//VirtSynth firmware version 0.1
//Tim Deltrap © 2018 under GNU GPL 3.0
#include "Arduino.h"
//Initialize some constants
const int keyOutputCount = 7;                                                                                           //the width of the key matrices.
const int keyInputCount = 8;                                                                                            //The height of the key matrices.
const int buttonOutputCount = 6;                                                                                        //the width of the button matrices.
const int buttonInputCount = 8;                                                                                         //The height of the button matrices.
const int baudRate = 115200;                                                                                            //Must be the same as serial reciever running on PC

//Initialize some variables
int button;
int buttonType;
//int buttonInputNum;
//int buttonOutputNum;
byte buttonState;
int key;
//int keyInputNum;
//int keyOutputNum;
byte keyState;
int note;                                                                                                               //Note sended to computer
int intensity;                                                                                                          //Note velocity
byte analogStatusByte;
byte analogDataByteOne;
byte analogDataByteTwo;

//input and output pins arrays
int keyOutputPins[] = {24, 28, 32, 36, 40, 44, 48};
int keyInputPins[] = {9, 8, 7, 6, 5, 4, 3, 2};
int buttonOutputs[] = {23, 25, 27, 29, 31, 33};
int buttonInputs[] = {39, 41, 43, 45, 47, 49, 51, 53};
int analogPins[11] = {A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11};
int ledPins[4] = {10,11,12,13};
int toggleButtons[4] = {};

//analog controls state arrays
int analogValue[11];
int previousAnalogValue[11];


//defining mapping of keys in the matrix
int keyMatrix[keyOutputCount][keyInputCount] = {{72,  0,  0,  0,  0,  0,  0,  0},
                                                {64, 65, 66, 67, 68, 69, 70, 71},
                                                {56, 57, 58, 59, 60, 61, 62, 63},
                                                {48, 49, 50, 51, 52, 53, 54, 55},
                                                {40, 41, 42, 43, 44, 45, 46, 47},
                                                {32, 33, 34, 35, 36, 37, 38, 39},
                                                {24, 25, 26, 27, 28, 29, 30, 31}};

//initialization of key state matrix
byte keyStateMatrix[keyOutputCount][keyInputCount];

//mapping of keys in the matrix
byte buttonMatrix[buttonOutputCount][buttonInputCount] = {{ 1,  2,  3,  4,  5,  6,  7,  8},
                                                          { 9, 10, 11, 12, 13, 14,  0,  0},
                                                          { 0,  0,  0, 15, 16, 17, 18, 19},
                                                          {20, 21, 22, 23, 24, 25, 26, 27},
                                                          {28, 29, 30, 31, 32, 33, 34, 35},
                                                          {36, 37, 38, 39, 40,  0,  0,  0}};

//initialization of button state matrix
byte buttonStateMatrix[buttonOutputCount][buttonInputCount];
//Declare all functions.
void getKey();
void scanKeyOutput(int keyOutputNum);
void scanKeyInput(int keyInputNum, int keyOutputNum);
void whenKeyPressed(int keyInputNum, int keyOutputNum);
void whenKeyReleased(int keyInputNum, int keyOutputNum);
void sendMidi(int channel, int cmd, int value);
void getButton();
void scanButtonOutput(int buttonOutputNum);
void scanButtonInput(int buttonInputNum, int buttonOutputNum);
void toggleButton(int button, int buttonInputNum, int buttonOutputNum);
void triggerButton(int button, int buttonInputNum, int buttonOutputNum);
void getAnalog();



void setup() {
    Serial.begin(baudRate);
    for (int keyInputPin = 0; keyInputPin < keyInputCount; keyInputPin++)
    {                                                                                                                   //Setup all column pins as INPUT_PULLUP.
        pinMode(keyInputPins[keyInputPin], INPUT_PULLUP);                                                               //Note that INPUT_PULLUP is being used for inmproved accuracy.
    }                                                                                                                   //This means that all logic is reversed.
    for (int keyOutputPin = 0; keyOutputPin < keyOutputCount; keyOutputPin++)
    {
        pinMode(keyOutputPins[keyOutputPin], OUTPUT);                                                                   //Setup all row pins as OUTPUT.
    }
    for (int buttonInputPin = 0; buttonInputPin < buttonInputCount; buttonInputPin++)
    {                                                                                                                   //Setup all column pins as INPUT_PULLUP.
        pinMode(buttonInputs[buttonInputPin], INPUT_PULLUP);                                                            //Note that INPUT_PULLUP is being used for improved accuracy. This means that all logic is reversed.
    }                                                                                                                   //This means that all logic is reversed.
    for (int buttonOutputPin = 0; buttonOutputPin < buttonOutputCount; buttonOutputPin++)
    {
        pinMode(buttonOutputs[buttonOutputPin], OUTPUT);                                                                //Setup all row pins as OUTPUT.
    }
    for (int ledPin = 0; ledPin < 3; ledPin++)
    {
        pinMode(ledPins[ledPin], OUTPUT);                                                                               //Setup all led pins as OUTPUT.
    }
}

void loop()
{
    getKey();                                                                                                           //Get keyboard inputs.
    getButton();                                                                                                        //Get buttons inputs, currently no function assigned so ignore.
    getAnalog();                                                                                                        //Get analog inputs, currently only for debugging.
}

void getKey()
{
    for(int i = 0; i <= 6; i++)
    {
        scanKeyOutput(i);                                                                                               //Activate keyboard output pins one by one.
    }
}

void scanKeyOutput(int keyOutputNum)
{
    digitalWrite(keyOutputPins[keyOutputNum], LOW);                                                                     //Set output on the used row to LOW. Which means in reversed logic that the output is "on".
    delay(2);                                                                                                           //Wait for 2ms to not to interrupt with the same function but with other parameters.
    for(int j = 0; j<=7; j++)
    {
        scanKeyInput(j, keyOutputNum);                                                                                  //Read the state of a key on the keyboard.
    }
    digitalWrite(keyOutputPins[keyOutputNum], HIGH);                                                                    //Reset the output to HIGH to put the output "off".
    delay(2);                                                                                                           //Another 2ms delay with the same purpose as the previous one.
}

void scanKeyInput(int keyInputNum, int keyOutputNum)
{
    keyState = digitalRead(keyInputPins[keyInputNum]);                                                                  //Read the state of a key on the keyboard.
    if((keyState  == LOW) && (keyStateMatrix[keyOutputNum][keyInputNum] == 0))                                                                       //If the key was pressed and state in state matrix is 0, has to do with ghosting.
    {
        keyStateMatrix[keyOutputNum][keyInputNum] = 1;                                                                  //Make state of current button in button state matrix 1.
        whenKeyPressed(keyInputNum, keyOutputNum);                                                                      //Specify key that was pressed.
    }
    if((keyState  == HIGH) && (keyStateMatrix[keyOutputNum][keyInputNum] == 1))                                                                      //If the key was released and state in state matrix is 1, has to do with ghosting.
    {
        keyStateMatrix[keyOutputNum][keyInputNum] = 0;                                                                  //Make state of current button in button state matrix 0.
        whenKeyReleased(keyInputNum, keyOutputNum);                                                                     //Specify key that was released.
    }
}

void whenKeyPressed(int keyInputNum, int keyOutputNum)
{
    key = keyMatrix[keyOutputNum][keyInputNum];                                                                         //Read key that was pressed.
    note = key;                                                                                                         //Modify key with transpose and octave.
    int val  = analogRead(A0);                                                                                          //Read note velocity.
    intensity = map(val, 0, 1023, 127, 0);                                                                              //Map analog value to Midi code.
    sendMidi(0x90, note, intensity);                                                                                    //Send midi On signal to PC.
}

void whenKeyReleased(int keyInputNum, int keyOutputNum)
{
    key = keyMatrix[keyOutputNum][keyInputNum];                                                                         //Read key that was released.
    note = key;                                                                                                         //Modify key with transpose and octave.
    sendMidi(0x90, note, 0x00);                                                                                            //Send midi Off signal to PC.
}

void sendMidi(byte statusByte, byte dataByteOne, byte dataByteTwo)
{
    Serial.write(statusByte);                                                                                              //Send midi channel.
    Serial.write(dataByteOne);                                                                                                  //Send command code | note / ControlCode.
    Serial.write(dataByteTwo);                                                                                                //Send value code | velocity / value.
}


void getButton()
{
    for(int i = 0; i <= 6; i++){
        scanButtonOutput(i);                                                                                            //Activate button output pins one by one.
    }
}

void scanButtonOutput(int buttonOutputNum)
{
    digitalWrite(keyOutputPins[buttonOutputNum], LOW);                                                                  //Set output on the current output to LOW. Which means in reversed logic that the output is "on".
    delay(2);                                                                                                           //Wait for 2ms to not to interrupt.
    for(int j = 0; j<=7; j++)
    {
        scanButtonInput(j, buttonOutputNum);                                                                            //Read the state of a button on the keyboard.
    }
    digitalWrite(buttonOutputs[buttonOutputNum], HIGH);                                                                  //Reset the output to HIGH to put the output "off".
    delay(2);                                                                                                           //Another 2ms delay with the same purpose as the previous one.
}

void scanButtonInput(int buttonInputNum, int buttonOutputNum)
{
    buttonState = digitalRead(buttonInputs[buttonInputNum]);                                                            //Read the state of a button on the keyboard.
    button = buttonMatrix[buttonOutputNum][buttonInputNum];                                                             //Read state of button in button state matrix.
    for(int i; i <= 3; i++){
        if(button == toggleButtons[i]){
            buttonType = 2;
        }
        else{
            buttonType = 1;
        }
    }
    if(buttonType == 1){
        triggerButton(button, buttonInputNum, buttonOutputNum);
    }
    else if(buttonType == 2){
        toggleButton(button, buttonInputNum, buttonOutputNum);
    }
}

void toggleButton(int button, int buttonInputNum, int buttonOutputNum){
    if((buttonState == LOW) && (buttonStateMatrix[buttonOutputNum][buttonInputNum] == 0))
    {
        buttonStateMatrix[buttonOutputNum][buttonInputNum] = 1;
        sendMidi();
    }
    else if((buttonState == LOW) && (buttonStateMatrix[buttonOutputNum][buttonInputNum] == 2))
    {
        buttonStateMatrix[buttonOutputNum][] = 3;
        sendMidi();
    }
    else if((buttonState == HIGH) && (buttonStateMatrix[buttonOutputNum][buttonInputNum] == 1))
    {
        buttonStateMatrix[buttonOutputNum][buttonInputNum] = 2;
    }
    else if((buttonState == HIGH) && (buttonStateMatrix[buttonOutputNum][buttonInputNum] == 3))
    {
        buttonStateMatrix[buttonOutputNum][buttonInputNum] = 0;
    }
}

void triggerButton(int button, int buttonInputNum, int buttonOutputNum){
    if((buttonState == LOW) && (buttonStateMatrix[buttonOutputNum][buttonInputNum] == 0))
    {
        buttonStateMatrix[buttonOutputNum][buttonInputNum] = 1;
        sendMidi();
    }
    else if((buttonState == HIGH) && (buttonStateMatrix[buttonOutputNum][buttonInputNum] == 1))
    {
        buttonStateMatrix[buttonOutputNum][buttonInputNum] = 0;
        sendMidi();
    }
}


void getAnalog()
{
    for(int i = 0; i <= 10; i++)
    {
        analogValue[i] = analogRead(analogPins[i]);                                                                     //Read values of analog pins one by one.
        if((analogValue[i] <= (previousAnalogValue[i] - 4)) || (analogValue[i] >= (previousAnalogValue[i] + 4)))        //If the value on an analog pin changed; by more than 4 either positive or negative to ignore noise on the pins.
        {


            switch(analogValue[i]) {
                case 0 :
                case 10 :
                    analogStatusByte = 0;
                    analogDataByteOne = map(analogValue[i], 0, 1023, 0, 127);
                    analogDataByteTwo = 0;
                    break;
                case 1 :
                case 2 :
                case 3 :
                case 4 :
                case 5 :
                case 6 :
                case 7 :
                case 8 :
                case 9 :
                    analogStatusByte = 0;
                    analogDataByteOne = map(analogValue[i], 0, 1023, 0, 127);
                    analogDataByteTwo = 0;
                    break;
            }
            sendMidi(analogStatusByte, analogDataByteOne, analogDataByteTwo);
            previousAnalogValue[i] = analogValue[i];
        }
    }
}