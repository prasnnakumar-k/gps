# GPS_Spoofer
# 📡 GPS Spoofing Signal Generator

This project simulates and generates spoofed GPS signals using real satellite ephemeris and a target location. It is structured into multiple parts — starting with receiving GPS signals using SDR.

---

## ✅ Prerequisites

Before running any part of the project, make sure the following components are correctly installed on your system.

---

### 1️⃣ UHD and GNU Radio Installation

Follow the official guide from Ettus Research:

🔗 [UHD and GNU Radio Installation Guide](https://kb.ettus.com/Building_and_Installing_the_USRP_Open-Source_Toolchain_(UHD_and_GNU_Radio)_on_Linux)

📌 **Important Fixes** for Git URLs:

- Replace:
  ```bash
  git clone https://github.com/EttusResearch/uhd
  ```
  With:
  ```bash
  git clone https://github.com/EttusResearch/uhd.git
  ```

- Replace:
  ```bash
  git clone --recursive https://github.com/gnuradio/gnuradio
  ```
  With:
  ```bash
  git clone --recursive https://github.com/gnuradio/gnuradio.git
  ```
- if it not works use this :
  ```bash
  sudo apt-get install gnuradio
  ```

✅ After installation, verify UHD is correctly installed and your USRP device is detected using:

```bash
uhd_usrp_probe
```

If a USRP is connected, it will display hardware details.

---

### 2️⃣ Protobuf 3.6.1 Installation

Download Protobuf 3.6.1 from the official GitHub:

🔗 [Protobuf v3.6.1](https://github.com/protocolbuffers/protobuf/tree/v3.6.1)

#### 📦 Installation Steps:

```bash
# Extract the downloaded archive
tar -xzf protobuf-3.6.1.tar.gz

# Move into the directory
cd protobuf-3.6.1

# Install dependencies
sudo apt-get install autoconf automake libtool curl make g++ unzip

# Run the autogen script
./autogen.sh

# Configure the build
./configure

# Compile Protobuf
make

# Install it
sudo make install
```

---

### 3️⃣ GNSS-SDR Installation

Install GNSS-SDR using APT:

```bash
sudo apt-get install gnss-sdr
```

✅ Verify the installation:

```bash
gnss-sdr --version
```

---

Once all the prerequisites are installed, you can proceed with the individual parts of the project as outlined below.


## 🛰️ Part 1: Receive GPS Signal Using SDR

The folder `gps_receiver/` contains the configuration file needed to receive GPS signals using a USRP device.

### 📂 File:
```
GPS_Receiver/SDR.conf
```

### ⚙️ Modify `SDR.conf` According to Your USRP Device

| USRP Model | `device_address`                | `signal_source` |
|------------|----------------------------------|------------------|
| B210       | *(Leave empty)*                 | `A:A`            |
| N210       | `addr=192.168.10.2` *(example)* | `A:0`            |

#### Example for **USRP B210**:
```ini
[usrp_source]
device_address = 
device_args = num_recv_frames=128
signal_source = A:A
```

#### Example for **USRP N210**:
```ini
[usrp_source]
device_address = addr=192.168.10.2
device_args = num_recv_frames=128
signal_source = A:0
```

---

### ▶️ Run GNSS-SDR

Once `SDR.conf` is configured, navigate to the `gps_receiver/` folder and run:

```bash
cd gps_receiver
UHD_IMAGES_DIR=/usr/local/share/uhd/images gnss-sdr --config_file=./SDR.conf
```

This will start GNSS-SDR and begin receiving GPS signals using your USRP hardware.

---

## 📟 Part 2: Monitor the Received GPS Signal

This step allows you to monitor the GPS signal received by the SDR in real-time. The monitoring tool is located in the `GPS_Monitor/` folder.

### 📂 File:
```
GPS_Monitor/gnss_synchro_udp_source.cc
```

### ⚙️ Modify the Receiver IP Address

Open the file `gnss_synchro_udp_source.cc` and update the IP address of the receiver to match your system’s configuration.

```cpp
// Example line to modify
std::string receiver_ip = "127.0.0.1"; // Replace with your actual IP if needed
```

---

### 🛠️ Build the Monitor

After editing the IP address:

```bash
cd GPS_Monitor
mkdir build && cd build
cmake ..
make
```

> 🔁 If you make any changes to the source files later, rebuild using:

```bash
make clean && make
```

---

### ▶️ Run the Monitoring Client

Once compiled, you can run the monitoring client:

```bash
./monitoring-client 1234
```

This starts the real-time GPS signal monitor.

---

### 🖥️ Note:

Both the `gps_receiver/` and `GPS_Monitor/` components are intended to run on the **same receiver PC**.


## 🚀 Part 3: Transmit Spoofed GPS Signal

This section covers the transmission of spoofed GPS signals using a separate transmitter PC. The relevant files are located in the `transmitter/` folder.

### 📂 Files:
```
transmitter/socket.h
transmitter/usrpgps
transmitter/motion.py
transmitter/dynamic_dt.py
```

---

### ⚙️ Step 1: Configure `socket.h`

Open the file `socket.h` and verify or update the following parameters:

- **Port Number**: Ensure it matches the receiver's listening port.
- **Position Parameters**: Set the desired spoofed location coordinates.

Example:
```c
#define PORT 1234
#define LATITUDE 37.7749
#define LONGITUDE -122.4194
#define ALTITUDE 30.0
```

---

### 🛠️ Step 2: Build the Transmitter

Navigate to the `transmitter/` directory and build the project:

```bash
cd transmitter
mkdir build && cd build
cmake ..
make
```

> 🔁 If you make any changes to the source files, rebuild using:

```bash
make clean && make
```

---

### 🌐 Step 3: Download Ephemeris File

Download the appropriate ephemeris file (e.g., `brdc` file) from the official source:

📥 [https://cddis.nasa.gov/archive/gnss/data/daily/](https://cddis.nasa.gov/archive/gnss/data/daily/)

Place the downloaded file in the `build/` directory.

---

### ▶️ Step 4: Run the Transmitter

Execute the transmitter with the ephemeris file and desired location:

```bash
./usrpgps -e <brdc_filename> -l <latitude,longitude,altitude>
```

Replace `<brdc_filename>` with the name of your ephemeris file and `<latitude,longitude,altitude>` with your target coordinates.

Example:
```bash
./usrpgps -e brdc1230.25n -l 37.7749,-122.4194,30.0
```

---

### 🛰️ Step 5: Dynamic Location Updates (Optional)

To simulate movement or add delays:

1. **Simulate Motion**: Run `motion.py` to update the spoofed location dynamically.

   ```bash
   python3 motion.py
   ```

2. **Add Delay**: Run `dynamic_dt.py` to introduce additional delays if needed.

   ```bash
   python3 dynamic_dt.py
   ```

> ⚠️ Ensure that `motion.py` and `dynamic_dt.py` are executed **after** starting the transmitter.

---

### 🖥️ Note:

- The `transmitter/` component is intended to run on a **separate transmitter PC**.
- Ensure network connectivity between the transmitter and receiver PCs for proper operation.

---
