/*
This program blends several demo scripts
 */


// LIBRARIES =============================================================== //
#include <SD.h>
#include <ArduinoSound.h>

// VARIABLES AND OBJECTS =================================================== //
const char    filename[] = "500hzp5.wav";                                     // filename of wave file to play
unsigned long timer;
unsigned long timer_stop;
unsigned long sample_window = 10;                                           //ms window to check peak
float           peak = 0;                                                  // pre-determined peak to find
int           full_scale;
double        full_scale_inverse;

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
  full_scale=pow(2,bitsPerSample-1);
  full_scale_inverse = 1/full_scale;

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
  timer_stop = millis() + sample_window;
}

void loop() {
  if (!AudioOutI2S.isPlaying()) {                                           // check if playback is still going on
    // playback has stopped

    Serial.println("playback stopped");
    while (1); // do nothing
  }
  if (amplitudeAnalyzer.available()) {                                      // check if a new analysis is available
    timer = millis();
    Serial.print(timer);
    if ( timer <  timer_stop) {
      float amplitude = amplitudeAnalyzer.read();  // read the new amplitude
      amplitude =amplitude/full_scale;
      if (amplitude > peak) {
        peak = amplitude;
      }
        
    } else {
        Serial.print(peak);
        Serial.print(" || ");
        Serial.print(timer);
        Serial.print(" || ");
        Serial.println(" * Peak Found");
         timer_stop = millis() + sample_window;
         peak = 0;
      }
    }
}

