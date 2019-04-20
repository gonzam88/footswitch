/*
  Footswitch USB - MIDI Controller
  --------------------------------
  gonzalo moiguer 2019
  www.gonzamoiguer.com.ar
*/


#include <MIDIUSB.h>
#include "looperChannel.h" // to make the code more readable every class should be stored in separate files

#define LOOPERS_AMOUNT 3

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}



int screenTimerCounter = 0;
int screenMode = 0;
// 0 = setup show
// 1 = midi output
// 2 = instrument select

const int setupShowDuration = 2000;
int setupShowCurrLed = 0;

int drumsChannel = 0;
const int maxDrumsChannels = 4;
int drumsChannelDisplayCount = 0;
const int CHANGE_DRUM_PIN = 2;
bool changeDrumPrevState = true;
const int DRUMS_DISPLAY_DURATION = 3000;

const int DEMUX_A = 21;
const int DEMUX_B = 19;
const int DEMUX_C = 20;
const int DEMUX_OUT = 5;


//
//const int num_array[10][7] = {  { 1, 1, 1, 1, 1, 1, 0 }, // 0
//  { 0, 1, 1, 0, 0, 0, 0 }, // 1
//  { 1, 1, 0, 1, 1, 0, 1 }, // 2
//  { 1, 1, 1, 1, 0, 0, 1 }, // 3
//  { 0, 1, 1, 0, 0, 1, 1 }, // 4
//  { 1, 0, 1, 1, 0, 1, 1 }, // 5
//  { 1, 0, 1, 1, 1, 1, 1 }, // 6
//  { 1, 1, 1, 0, 0, 0, 0 }, // 7
//  { 1, 1, 1, 1, 1, 1, 1 }, // 8
//  { 1, 1, 1, 0, 0, 1, 1 }
//};   // 9
//
//const int tempo_array[6][7] =  {  { 1, 0, 0, 0, 0, 0, 0 },
//  { 0, 1, 0, 0, 0, 0, 0 },
//  { 0, 0, 1, 0, 0, 0, 0 },
//  { 0, 0, 0, 1, 0, 0, 0 },
//  { 0, 0, 0, 0, 1, 0, 0 },
//  { 0, 0, 0, 0, 0, 1, 0 }
//};

const bool secuenciaDemux[7][3] =  {  
  { 0, 0, 0},
  { 1, 0, 0},
  { 0, 0, 1},
  { 1, 1, 1},
  { 1, 1, 0},
  { 1, 0, 1},
  { 0, 1, 1}
};

int currTempo = 0;
//
//void Num_Write(int number)
//{
//  for (int j = 0; j < 8; j++) {
//    int pin = screenPos[j];
//    digitalWrite(pin, num_array[number][j]);
//  }
//}
//
//void Tempo_Write(int number)
//{
//  for (int j = 0; j < 8; j++) {
//    int pin = screenPos[j];
//    digitalWrite(pin, tempo_array[number][j]);
//  }
//}

void DisplayDemuxIndex(int i){
  int pin_a_value = secuenciaDemux[i][0];
  digitalWrite(DEMUX_A, pin_a_value);
  int pin_b_value = secuenciaDemux[i][1];
  digitalWrite(DEMUX_B, pin_b_value);
  int pin_c_value = secuenciaDemux[i][2];
  digitalWrite(DEMUX_C, pin_c_value);
  digitalWrite(DEMUX_OUT, HIGH);
}

int TOGGLE_ALL_PIN = 15;

LooperChannel loopers[LOOPERS_AMOUNT] = {
  LooperChannel(14,   1,  3, 48, 20, 49),
  LooperChannel(16,   0,  10, 50, 21, 51),
  LooperChannel(7,    4,  9, 52, 22, 53)
  //  recPin, clearPin, ledPin, midiRecNote, midiPauseNote, midiClearNote
};



void MuteAll() {
  for (int i = 0; i < LOOPERS_AMOUNT; i++) {
    controlChange(0, loopers[i].midiPauseNote, 0); // Apagar canal
  }
}

void PlayAll() {
  for (int i = 0; i < LOOPERS_AMOUNT; i++) {
    controlChange(0, loopers[i].midiPauseNote, 127); // Activar Canal
  }
}


void setup()
{
//  Serial.beginu(9600);
//  Serial.println("hola");
  for (int i = 0; i < LOOPERS_AMOUNT; i++) {
    loopers[i].setup();
  }

  // Demuxer PINS
  pinMode(DEMUX_A,  OUTPUT);
  pinMode(DEMUX_B,  OUTPUT);
  pinMode(DEMUX_C,  OUTPUT);
  pinMode(DEMUX_OUT,  OUTPUT);
  // Screen LEDS
//  pinMode(3,   OUTPUT);
//  pinMode(5,   OUTPUT);
//  pinMode(6,   OUTPUT);
//  pinMode(8,   OUTPUT);
//  pinMode(9,   OUTPUT);
//  pinMode(10,  OUTPUT);
//  pinMode(21,  OUTPUT);

  //
  pinMode(CHANGE_DRUM_PIN,  INPUT_PULLUP);
  pinMode(TOGGLE_ALL_PIN,  INPUT_PULLUP);

}

unsigned long previousMillis = 0;

void loop()
{

  // *************************
  // Read MIDI IN
  // *************************
  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    // read the velocity value
    int note = rx.byte2;
    int velocity = rx.byte3;
    if (rx.header != 0) {
//      Serial.print("\t Received: ");
//      Serial.print(rx.header, DEC);
//      Serial.print("-");
//      Serial.print(rx.byte1, DEC);
//      Serial.print("-");
//      Serial.print(rx.byte2, DEC);
//      Serial.print("-");
//      Serial.println(velocity, DEC);
      // *************************
      // MIDI IN to Beat Display
      if (velocity > 64) {
        if (note == 72) {
          // Si la nota es C4. Seteo el tempo en el primer beat.
          // Asi me aseguro de estar siempre en sincro
          currTempo = 0;
        } else {
          currTempo++;
          if (currTempo > 5) currTempo = 0;
        }
      }
    }
  } while (rx.header != 0);


  // *************************
  // MIDI IN to Beat Display
  //
  //    midiEventPacket_t rx;
  //    rx = MidiUSB.read();
  //    if(rx.byte3 > 64){
  //      Serial.println("boop");
  ////      Serial.println(rx.byte2);
  ////      if( rx.byte2 == 72){
  ////        // Uso la nota C4 como set obligatorio del beat 1
  ////        // Asi me garantizo siempre estar en sincro
  ////        currTempo = 0;
  ////      }else{
  //        currTempo++;
  //        if(currTempo > 5) currTempo = 0;
  ////      }
  //    }
  // *************************



  // *************************
  // LED Display Logic
  // *************************
  unsigned long currentMillis = millis();
  unsigned int deltaTime = currentMillis - previousMillis;
  previousMillis = currentMillis;
  screenTimerCounter += deltaTime;

  bool changeDrumPinState = (digitalRead(CHANGE_DRUM_PIN) == HIGH);
  if ( !changeDrumPinState  && changeDrumPinState != changeDrumPrevState ) {
    drumsChannel++;
    if (drumsChannel + 1 > maxDrumsChannels) {
      drumsChannel = 0;
    }
    if (screenMode != 2) {
      screenMode = 2;
      screenTimerCounter = 0;
    }
  }
  changeDrumPrevState = changeDrumPinState;

  if (screenMode == 0) {
    // init show
//    setupShowLedDuration += deltaTime;

    //if (screenTimerCounter >= setupShowLedDuration) {
      //setupShowLedDuration = 0;
//      digitalWrite(screenPos[setupShowCurrLed], LOW);
    DisplayDemuxIndex(setupShowCurrLed);
      setupShowCurrLed++;
      if (setupShowCurrLed > 5)setupShowCurrLed = 0;
//    }
    //digitalWrite(screenPos[setupShowCurrLed], HIGH);

    if (screenTimerCounter >= setupShowDuration) {
      
      screenMode = 1;
    }

  } else if (screenMode == 1) {
    DisplayDemuxIndex(currTempo);

  } else if (screenMode == 2) {
    DisplayDemuxIndex(drumsChannel);
    if (screenTimerCounter >= DRUMS_DISPLAY_DURATION) {
      screenMode = 1;
    }
  }
  // *************************


  // *************************
  // Loopers Logic
  // *************************
  for (int i = 0; i < LOOPERS_AMOUNT; i++) {
    // Rec Switch Logic
    loopers[i].currRecState = digitalRead(loopers[i].recPin);

    if ( loopers[i].currRecState != loopers[i].prevRecState) {

      if ( loopers[i].currRecState == LOW) {
//        Serial.println("REC BT PRESS");
        //loopers[i].RecButtonPressed();

//        Serial.println(loopers[i].status());
        //send midi cc on
        if ( loopers[i].status() == 0) {
          // clear -> rec
          noteOn(0, loopers[i].midiRecNote, 127);
          loopers[i].StartRecording();

        } else if ( loopers[i].status() == 1) {
          // rec -> play
          noteOn(0, loopers[i].midiRecNote, 127);
          controlChange(0, loopers[i].midiPauseNote, 127); // Activar Canal
          loopers[i].ResumePlayback();

        } else if ( loopers[i].status() == 2) {
          // Playing -> pause
          controlChange(0, loopers[i].midiPauseNote, 0); // Apagar canal
          loopers[i].PausePlayback();

        } else {
          // pause -> play
          controlChange(0, loopers[i].midiPauseNote, 127); // Activar Canal
          loopers[i].ResumePlayback();
        }

      } else {

//        Serial.println("REC BT Released");
        loopers[i].RecButtonReleased();
        //        if( loopers[i].status() == 1){
        //          noteOff(0, loopers[i].midiRecNote, 127);
        //        }

      }
    }

    loopers[i].prevRecState = loopers[i].currRecState;


    // *************************
    // Clear Switch Logic
    loopers[i].currClearState = digitalRead(loopers[i].clearPin);

    if ( loopers[i].currClearState != loopers[i].prevClearState) {

      if ( loopers[i].currClearState == LOW) {
//        Serial.println("Clear BT PRESS");
        loopers[i].ClearButtonPressed();
        //send midi cc on
        noteOn(0, loopers[i].midiClearNote, 64);
      } else {

//        Serial.println("Clear BT Released");
        loopers[i].RecButtonReleased();
        noteOff(0, loopers[i].midiClearNote, 127);
      }
    }

    loopers[i].prevClearState = loopers[i].currClearState;



    loopers[i].loop();
    // Delay a little bit to avoid bouncing
    delay(10);

  }
  // *************************
  MidiUSB.flush();
}
