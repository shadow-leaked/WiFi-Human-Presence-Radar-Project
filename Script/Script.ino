/*
  ARDUINO R4 WIFI - HUMAN PRESENCE RADAR
  --------------------------------------
  Detects motion by monitoring perturbations in WiFi Signal Strength (RSSI).
  Uses a Moving Average Filter to smooth noise.
*/

#include "WiFiS3.h"

// --- CONFIGURATION ---
char ssid[] = "HackerSploit_4G";        // <--- CHANGE THIS
char pass[] = "@Ransome#7";        // <--- CHANGE THIS

// SENSITIVITY SETTINGS
const int FILTER_SIZE = 10;            // Higher = Smoother line, slower reaction
const int TRIGGER_THRESHOLD = 5;       // How big a drop triggers the alarm (dBm)

// GLOBALS
int rssiBuffer[FILTER_SIZE];
int bufferIndex = 0;
long filteredRSSI = 0;
long baselineRSSI = 0;
bool calibrationComplete = false;

void setup() {
  Serial.begin(115200);
  
  // 1. Connect to WiFi
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi Module Failed!");
    while (true);
  }

  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  
  // 2. Initialize Buffer
  long current = WiFi.RSSI();
  for(int i=0; i<FILTER_SIZE; i++) {
    rssiBuffer[i] = current;
  }
  
  Serial.println("Calibrating baseline... (Don't move!)");
}

void loop() {
  // 1. Get Raw Signal
  long rawRSSI = WiFi.RSSI();

  // 2. Apply Moving Average Filter (DSP)
  // Subtract the oldest reading
  long sum = 0;
  rssiBuffer[bufferIndex] = rawRSSI; // Overwrite oldest with new
  bufferIndex = (bufferIndex + 1) % FILTER_SIZE; // Advance index

  // Calculate Average
  for(int i=0; i<FILTER_SIZE; i++) {
    sum += rssiBuffer[i];
  }
  filteredRSSI = sum / FILTER_SIZE;

  // 3. Calibration Logic (First 50 loops)
  if (millis() < 10000) {
    baselineRSSI = filteredRSSI; // constantly update baseline while starting
  } else {
    calibrationComplete = true;
  }

  // 4. Detection Logic
  // If the signal drops significantly below the baseline...
  int delta = baselineRSSI - filteredRSSI; 
  
  // 5. Output for Serial Plotter
  // We print in a format "Label:Value" so the graph picks it up.
  Serial.print("Raw:");
  Serial.print(rawRSSI);
  Serial.print(",");
  Serial.print("Filtered:");
  Serial.print(filteredRSSI);
  Serial.print(",");
  Serial.print("Baseline:");
  Serial.print(baselineRSSI);
  Serial.print(",");
  
  if (calibrationComplete && delta > TRIGGER_THRESHOLD) {
    Serial.print("DETECTED:");
    Serial.println(filteredRSSI - 10); // Draw a "dip" on graph for detection
    // Turn on built-in LED
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    Serial.print("DETECTED:"); 
    Serial.println(baselineRSSI); // Flat line
    digitalWrite(LED_BUILTIN, LOW);
  }

  // Auto-adjust baseline slowly to account for temperature/environmental drift
  if (calibrationComplete) {
     // Very slow drift towards current value
     if (filteredRSSI > baselineRSSI) baselineRSSI++;
     if (filteredRSSI < baselineRSSI) baselineRSSI--;
  }

  delay(50); // 20Hz Sample Rate
}