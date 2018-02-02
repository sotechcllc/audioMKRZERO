/*
This program blends several demo scripts
 */


// LIBRARIES =============================================================== //
#include <SD.h>
#include <ArduinoSound.h>

// VARIABLES AND OBJECTS =================================================== //
const char    filename[] = "1hz0p8.WAV";                                     // filename of wave file to play
unsigned long timer;
int           peak = 20000;                                                  // pre-determined peak to find

SDWaveFile        waveFile;                                                  // variable representing the Wave File
AmplitudeAnalyzer amplitudeAnalyzer;                                         // create amplitude analyzer object

// SETUP LOOP ============================================================== //
void setup() { 
  Serial.begin(115200);                                                      // Open serial communications and wait for port to open:
  while (!Serial) {                                                          // wait for serial port to connect. Needed for native USB port only
    ; 
  }
  // Setup SD Card --------------------------------------------------------- //
  Serial.print("Initializing SD card...");
  if (!SD.begin()) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // create a SDWaveFile
  waveFile = SDWaveFile(filename);

  // check if the WaveFile is valid
  if (!waveFile) {
    Serial.println("wave file is invalid!");
    while (1); // do nothing
  }

  // print out some info. about the wave file
  long bitsPerSample = waveFile.bitsPerSample();
  Serial.print("Bits per sample = ");
  Serial.println(waveFile.bitsPerSample());

  long channels = waveFile.channels();
  Serial.print("Channels = ");
  Serial.println(channels);

  long sampleRate = waveFile.sampleRate();
  Serial.print("Sample rate = ");
  Serial.print(sampleRate);
  Serial.println(" Hz");

  long duration = waveFile.duration();
  Serial.print("Duration = ");
  Serial.print(duration);
  Serial.println(" seconds");

  // Setup I2S output ----------------------------------------------------- //
  AudioOutI2S.volume(5);                                                    // adjust volume
  if (!AudioOutI2S.canPlay(waveFile)) {                                     // check if the I2S output can play the wave file
    Serial.println("unable to play wave file using I2S!");
    while (1); // do nothing
  }
  Serial.println("starting playback");                                      // start playback
  AudioOutI2S.play(waveFile);

  // Setup AmplitudeAnalyzer ---------------------------------------------- //
  if (!amplitudeAnalyzer.input(waveFile)) {
    Serial.println("Failed to set amplitude analyzer input!");
    while (1); // do nothing
  }
  
}

void loop() {
  if (!AudioOutI2S.isPlaying()) {                                           // check if playback is still going on
    // playback has stopped

    Serial.println("playback stopped");
    while (1); // do nothing
  }
  if (amplitudeAnalyzer.available()) {                                      // check if a new analysis is available
    int amplitude = amplitudeAnalyzer.read();                               // read the new amplitude
    if (amplitude > peak) {
      timer = millis();
      Serial.print(amplitude);
      Serial.print(" || ");
      Serial.print(timer);
      Serial.print(" || ");
      Serial.println(" * Peak Found");
    }
    else {
      Serial.println(amplitude);                                              // print out the amplititude to the serial monitor
    }
  }
}
