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
  int screenPos[7] = {6, 3, 5, 10, 8, 21, 9};
  const int setupShowInterval = 0;
  int setupShowLedDuration = 0;
  const int setupShowDuration = 4250;
  int setupShowCurrLed = 0;
  
  int drumsChannel = 0;
  const int maxDrumsChannels = 4;
  int drumsChannelDisplayCount = 0;
  const int CHANGE_DRUM_PIN = 2;
  bool changeDrumPrevState = true;
  const int DRUMS_DISPLAY_DURATION = 3000;
  
  const int num_array[10][7] = {  { 1,1,1,1,1,1,0 },    // 0
                                  { 0,1,1,0,0,0,0 },    // 1
                                  { 1,1,0,1,1,0,1 },    // 2
                                  { 1,1,1,1,0,0,1 },    // 3
                                  { 0,1,1,0,0,1,1 },    // 4
                                  { 1,0,1,1,0,1,1 },    // 5
                                  { 1,0,1,1,1,1,1 },    // 6
                                  { 1,1,1,0,0,0,0 },    // 7
                                  { 1,1,1,1,1,1,1 },    // 8
                                  { 1,1,1,0,0,1,1 }};   // 9

  const int tempo_array[6][7] =  {  { 1,0,0,0,0,0,0 },
                                    { 0,1,0,0,0,0,0 },
                                    { 0,0,1,0,0,0,0 },
                                    { 0,0,0,1,0,0,0 },
                                    { 0,0,0,0,1,0,0 },
                                    { 0,0,0,0,0,1,0 }};
  int currTempo = 0;
  
  void Num_Write(int number) 
  {
    //digitalWrite(9, HIGH);
    for (int j=0; j < 8; j++) {
      int pin = screenPos[j];
      digitalWrite(pin, num_array[number][j]);
    }
  }

   void Tempo_Write(int number) 
  {
    //digitalWrite(9, HIGH);
    for (int j=0; j < 8; j++) {
      int pin = screenPos[j];
      digitalWrite(pin, tempo_array[number][j]);
    }
  }
  
  int TOGGLE_ALL_PIN = 15;
  
  LooperChannel loopers[LOOPERS_AMOUNT] = {
    LooperChannel(14,   1,  18, 48, 20, 49),
    LooperChannel(16,   0,  19, 50, 21, 51),
    LooperChannel(7,    4,  20, 52, 22, 53)
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
  
  
    // Screen LEDS
    pinMode(3,   OUTPUT);
    pinMode(5,   OUTPUT);
    pinMode(6,   OUTPUT);
    pinMode(8,   OUTPUT);
    pinMode(9,   OUTPUT);
    pinMode(10,  OUTPUT);
    pinMode(21,  OUTPUT);
    
    // 
    pinMode(CHANGE_DRUM_PIN,  INPUT_PULLUP);
    pinMode(TOGGLE_ALL_PIN,  INPUT_PULLUP);
    
  }
  
  unsigned long previousMillis = 0;
  int t_old = 0;
  
  
  void loop()
  {
  
//    midiEventPacket_t rx;
//    rx = MidiUSB.read();
//    do {
//        rx = MidiUSB.read();
//        // read the velocity value
//        int Velocity = rx.byte3;
//        int t_now = millis();
//        int t_diff = t_now - t_old;
//        
//        if (rx.header != 0) {
//          Serial.print("t_diff = ");
//          Serial.print(t_diff);
//          
//          Serial.print("\t Received: ");
//          Serial.print(rx.header, DEC);
//          Serial.print("-");
//          Serial.print(rx.byte1, DEC);
//          Serial.print("-");
//          Serial.print(rx.byte2, DEC);
//          Serial.print("-");
//          Serial.println(Velocity, DEC);
//    
//          if (Velocity == 127){
//            digitalWrite(LED_BUILTIN,HIGH);
//            } else{
//            digitalWrite(LED_BUILTIN,LOW);
//            Serial.println(" ");
//            }
//    
//            t_old = t_now;
//          }
//      } while (rx.header != 0);


    // *************************
    // MIDI IN to Beat Display
    //
    midiEventPacket_t rx;
    rx = MidiUSB.read();
    if(rx.byte3 > 64){
      Serial.println("boop");
//      Serial.println(rx.byte2);
//      if( rx.byte2 == 72){
//        // Uso la nota C4 como set obligatorio del beat 1
//        // Asi me garantizo siempre estar en sincro
//        currTempo = 0;
//      }else{
        currTempo++;
        if(currTempo > 5) currTempo = 0;  
//      }
    }
    // *************************
  
  
    
    // *************************
    // LED Display Logic
    // *************************
    unsigned long currentMillis = millis();
    unsigned int deltaTime = currentMillis - previousMillis;
    previousMillis = currentMillis;
    screenTimerCounter += deltaTime;
  
    bool changeDrumPinState = (digitalRead(CHANGE_DRUM_PIN)==HIGH);
    if( !changeDrumPinState  && changeDrumPinState != changeDrumPrevState ){
      drumsChannel++;
      if(drumsChannel+1 > maxDrumsChannels){
        drumsChannel = 0;
      }
      if(screenMode != 2){
        screenMode = 2;  
        screenTimerCounter = 0;
      }
    }
    changeDrumPrevState = changeDrumPinState;
    
    if(screenMode == 0){
      // init show
      setupShowLedDuration += deltaTime;
      
      if(screenTimerCounter >= setupShowLedDuration){
        setupShowLedDuration = 0;
        digitalWrite(screenPos[setupShowCurrLed], LOW);
        setupShowCurrLed++;
        if(setupShowCurrLed>5)setupShowCurrLed=0;
      }
      digitalWrite(screenPos[setupShowCurrLed], HIGH);
      
      if(screenTimerCounter >= setupShowDuration){
        digitalWrite(screenPos[setupShowCurrLed], LOW);
        screenMode = 1;
      }
      
    }else if(screenMode == 1){
      Tempo_Write(currTempo);      
      
    }else if(screenMode == 2){
      Num_Write(drumsChannel);
      if(screenTimerCounter >= DRUMS_DISPLAY_DURATION){
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
      delay(10);
  
    }
    // *************************  
    MidiUSB.flush();
  }
