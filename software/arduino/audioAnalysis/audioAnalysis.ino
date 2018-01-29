/*
This program blends several demo scripts
 */


// LIBRARIES =============================================================== //
#include <SD.h>
#include <ArduinoSound.h>

// VARIABLES AND OBJECTS =================================================== //
// filename of wave file to play
const char filename[] = "1hz0p8.WAV";

// variable representing the Wave File
SDWaveFile waveFile;
AmplitudeAnalyzer amplitudeAnalyzer;

// SETUP LOOP ============================================================== //
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Setup SD Card --------------------------------------------------------- //
  // setup the SD card, depending on your shield of breakout board
  // you may need to pass a pin number in begin for SS
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

  // Setup I2S input ------------------------------------------------------ //
  //if (!AudioInI2S.begin(44100, 16)) {                       // adjust to the parameters of the audio file from the sd card
  //  Serial.println("Failed to initialize I2S input!");
  //  while (1); // do nothing
  //} 

  // Setup AmplitudeAnalyzer ---------------------------------------------- //
  if (!amplitudeAnalyzer.input(waveFile)) {
    Serial.println("Failed to set amplitude analyzer input!");
    while (1); // do nothing
  }
  
}

void loop() {
  // check if playback is still going on
  if (!AudioOutI2S.isPlaying()) {
    // playback has stopped

    Serial.println("playback stopped");
    while (1); // do nothing
  }

  // check if a new analysis is available
  if (amplitudeAnalyzer.available()) {
    // read the new amplitude
    int amplitude = amplitudeAnalyzer.read();

    // print out the amplititude to the serial monitor
    Serial.println(amplitude);
  }
  
}
