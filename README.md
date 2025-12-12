# Arduino R4 WiFi - Human Presence Radar (RSSI)

> **Status:** Working  
> **Hardware:** Arduino R4 WiFi (Renesas/ESP32)  
> **Concept:** Passive RF Sensing / Surveillance

## 📜 Project Overview
This project turns an **Arduino R4 WiFi** into a passive motion detector. Instead of using cameras or PIR sensors, it monitors the **Received Signal Strength Indicator (RSSI)** of a specific WiFi network. 

When a water-dense object (like a human body) passes between the Arduino and the router, it absorbs 2.4GHz radio waves, causing a measurable drop in signal strength ("radio shadow"). This code uses **Digital Signal Processing (DSP)** to filter out noise and detect these specific drops.

---

## ⚙️ How It Works (The Engineering)
1.  **Signal Acquisition:** The ESP32 module polls the target router (`HackerSploit_4G`) 20 times per second.
2.  **Noise Filtration:** A **Moving Average Filter** (Circular Buffer) processes the raw data to remove high-frequency RF noise and multipath interference.
3.  **Adaptive Baseline:** The system calculates a "normal" baseline signal and slowly adjusts it over time to account for environmental drift (temperature/humidity).
4.  **Anomaly Detection:** If the filtered signal drops below the `baseline - threshold`, an alarm state is triggered (LED turns ON).

---

## 🛠️ Hardware Setup
* **Microcontroller:** Arduino R4 WiFi
* **Target:** A WiFi Router or Mobile Hotspot (2.4GHz)
* **Power:** USB-C (Connected to Laptop for visualization)

**Placement Strategy:**
For best results, place the Arduino and Router 3–5 meters apart. The detection zone is the direct line of sight between them.

---

## 💻 Configuration (Code Variables)

| Variable | Value (Default) | Description |
| :--- | :--- | :--- |
| `FILTER_SIZE` | `10` | Size of the rolling buffer. Higher = Smoother data but more lag. |
| `TRIGGER_THRESHOLD` | `5` | Sensitivity in dBm. Lower (3) = High Sensitivity. Higher (8) = Less False Alarms. |
| `ssid` | `HackerSploit_4G` | **Target Network Name** (Must be 2.4GHz) |
| `pass` | `[HIDDEN]` | **Target Network Password** |

---

## 📊 How to Visualize (The Radar Screen)
This code output is formatted for the **Arduino Serial Plotter**.

1.  Connect the Arduino via USB.
2.  Open Arduino IDE.
3.  Press **`Ctrl` + `Shift` + `L`** (or Tools > Serial Plotter).
4.  **Baud Rate:** Set to `115200`.

**Legend:**
* 🔵 **Raw:** The noisy, instant signal strength.
* 🟠 **Filtered:** The processed, smooth signal.
* 🟢 **Baseline:** The "Zero" reference point.
* 🔴 **DETECTED:** Dips down when motion is detected.

---

## ⚠️ Troubleshooting Signal Strength
* **Graph is flat line / Too stable:** The signal is too strong (>-55dBm).
    * *Fix:* Move Arduino further away or wrap the antenna area in a layer of paper + aluminum foil to attenuate the signal to **-75dBm**.
* **False Positives:** The signal is too weak (<-85dBm).
    * *Fix:* Move closer to the router.

---

## 📄 License
**Educational Use Only.**
This project demonstrates principles of RF propagation and signal processing.