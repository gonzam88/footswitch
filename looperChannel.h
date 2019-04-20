class LooperChannel
{
	public: // members available for the outside world
		LooperChannel(int _recPin, int _clearPin, int _ledPin, int _midiRecNote, int _midiPauseNote, int _midiClearNote); // the constructor
		
		void setup();
		void loop();
		int status();
		void RecButtonPressed();
		void RecButtonReleased();
		void ClearButtonPressed();
		void ClearButtonReleased();
		void StartRecording();
		void StopRecording();
		void PausePlayback();
		void ResumePlayback();
		void Clear();

		int recPin;
		int clearPin;
		int ledPin;

    int prevRecState = 0;
    int currRecState = 0;
    int prevClearState = 0;
    int currClearState = 0;

		int midiRecNote;
    int midiPauseNote;
		int midiClearNote;

    bool isPaused = false;

	private: // members for private use only

		int _status;
		int _ledState = 0;
		// status
		// 0 = clear
		// 1 = recording
		// 2 = playing loop
		// 3 = paused
		const int blinkInterval = 200;
		unsigned long previousMillis = 0;
    int fadeValue = 20;
    bool fadeGoingUp = true;
    const int fadeSpeed = 5;
};


LooperChannel::LooperChannel(int _recPin, int _clearPin, int _ledPin, int _midiRecNote, int _midiPauseNote, int _midiClearNote) // the constructor at work
{
	recPin 		= _recPin;
	clearPin 	= _clearPin;
	ledPin 		= _ledPin;
	_status 	= 0;
	midiRecNote 	= _midiRecNote;
  midiPauseNote = _midiPauseNote;
	midiClearNote	= _midiClearNote;
}


void LooperChannel::setup() // set the selected PIN to be a digital output
{
	pinMode(recPin,   INPUT_PULLUP);
	pinMode(clearPin, INPUT_PULLUP);
	pinMode(ledPin,   OUTPUT);
}

void LooperChannel::loop() // set the selected PIN to be a digital output
{
//  Serial.println(_status);
	if(_status == 1){
		// blinking led: https://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
		unsigned long currentMillis = millis();
		if (currentMillis - previousMillis >= blinkInterval) {
			// save the last time you blinked the LED
			previousMillis = currentMillis;

			// if the LED is off turn it on and vice-versa:
			if (_ledState == 0) {
				_ledState = 255;
			} else {
				_ledState = 0;
			}
			// set the LED with the _ledState of the variable:
			analogWrite(ledPin, _ledState);
		}
	
	}else if(_status == 3){
    // Pause pulsing LED
    if(fadeGoingUp){
      fadeValue += fadeSpeed;
      if(fadeValue > 100) fadeGoingUp = false;
    }else{
      fadeValue -= fadeSpeed;
      if(fadeValue < 20) fadeGoingUp = true;
    }

    fadeValue = max(fadeValue,20);
    fadeValue = min(fadeValue,100);
    analogWrite(ledPin, fadeValue);
    
	}
}

int LooperChannel::status()
{
  return _status;
}


void LooperChannel::RecButtonPressed()
{
  if(_status == 0){
		

	}else if(_status == 1){
		

	}else if(_status == 2){
		

	}else if(_status == 3){
		
	}
}

void LooperChannel::RecButtonReleased()
{
//  u.println("REC BT Release");
}

void LooperChannel::ClearButtonPressed()
{
//  Serial.println("CLEAR BT PRESS");
	this->Clear();
}
void LooperChannel::ClearButtonReleased()
{
//  Serial.println("CLEAR BT Release");
	
}

void LooperChannel::StartRecording()
{
	_status = 1;
	// blinking leds on loop()
}

void LooperChannel::StopRecording()
{
	this->ResumePlayback();

}

void LooperChannel::ResumePlayback()
{
	_status = 2;
	analogWrite(ledPin, 255);
}

void LooperChannel::PausePlayback()
{
	_status = 3;
  //fade in/out in loop   
}

void LooperChannel::Clear()
{
	_status = 0;
	analogWrite(ledPin, 0);
}
