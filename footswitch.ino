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




LooperChannel loopers[LOOPERS_AMOUNT] = {
  LooperChannel(14,   1,  18, 48, 20, 49),
  LooperChannel(16,   0,  19, 50, 21, 51),
  LooperChannel(7,    4,  20, 52, 22, 53)
  //LooperChannel(int _recPin, int _clearPin, int _ledPin, int _midiRecCC, int midiPauseNote, int _midiClearCC)
};



void MuteAll(){
  for (int i = 0; i < LOOPERS_AMOUNT; i++) {
    controlChange(0, loopers[i].midiPauseNote, 0); // Apagar canal
  }
}

void PlayAll(){
  for (int i = 0; i < LOOPERS_AMOUNT; i++) {
    controlChange(0, loopers[i].midiPauseNote, 127); // Activar Canal
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println("hola");
  for (int i = 0; i < LOOPERS_AMOUNT; i++) {
    loopers[i].setup();
  }
}

void loop()
{

  for (int i = 0; i < LOOPERS_AMOUNT; i++) {
    Serial.println(" ");
    Serial.print(i);
    Serial.print(" ");
    Serial.print(loopers[i].recPin);

    // Rec Switch Logic
    loopers[i].currRecState = digitalRead(loopers[i].recPin);
    
    if( loopers[i].currRecState != loopers[i].prevRecState){
     
      if( loopers[i].currRecState == HIGH){
        Serial.println("REC BT PRESS");
        //loopers[i].RecButtonPressed();

        Serial.println(loopers[i].status());
        //send midi cc on
        if( loopers[i].status() == 0){
          // clear -> rec
          noteOn(0, loopers[i].midiRecNote, 127);
          loopers[i].StartRecording();
          
        }else if( loopers[i].status() == 1){
          // rec -> play
          noteOn(0, loopers[i].midiRecNote, 127);
          controlChange(0, loopers[i].midiPauseNote, 127); // Activar Canal
          loopers[i].ResumePlayback();
        
        }else if( loopers[i].status() == 2){
          // Playing -> pause
          controlChange(0, loopers[i].midiPauseNote, 0); // Apagar canal
          loopers[i].PausePlayback();
          
        }else{
          // pause -> play
          controlChange(0, loopers[i].midiPauseNote, 127); // Activar Canal
          loopers[i].ResumePlayback();
        }
        
      }else{
        
        Serial.println("REC BT Released");
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
    
    if( loopers[i].currClearState != loopers[i].prevClearState){
     
      if( loopers[i].currClearState == HIGH){
        Serial.println("Clear BT PRESS");
        loopers[i].ClearButtonPressed();
        //send midi cc on
        noteOn(0, loopers[i].midiClearNote, 64);
      }else{
        
        Serial.println("Clear BT Released");
        loopers[i].RecButtonReleased();
        noteOff(0, loopers[i].midiClearNote, 127);
      }
    } 

    loopers[i].prevClearState = loopers[i].currClearState;



    loopers[i].loop();
    // Delay a little bit to avoid bouncing
    delay(50);

  }

  //  analogWrite(5, 1);
  //  if( digitalRead(3) == HIGH){
  //Serial.println("3 high");
  //
  //  }else{
  //    Serial.println("3 low");
  //  }
  //
  //    if( digitalRead(4) == HIGH){
  //Serial.println("4 high");
  //
  //  }else{
  //    Serial.println("4 low");
  //  }


  MidiUSB.flush();
}
