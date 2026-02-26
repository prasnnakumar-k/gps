# Simulation Validation Report

## Scope
Validated build/run readiness for the uploaded receiver monitor and transmitter code.

## Commands executed
1. `cd GPS_Monitor && mkdir -p build && cd build && cmake .. && make -j4`
2. `cd GPS_Transmitter && mkdir -p build && cd build && cmake .. && make -j4`
3. `python3 -m py_compile GPS_Transmitter/motion.py GPS_Transmitter/dynamic_dt_correction.py GPS_Transmitter/nmea_reader.py`
4. `printf '1.2\n' | timeout 2s python3 GPS_Transmitter/dynamic_dt_correction.py`

## Findings

### 1) GPS_Monitor currently does not configure in this environment
- **Error:** `Could NOT find Boost (missing: Boost_INCLUDE_DIR system)`.
- **Impact:** monitor binary cannot be built until Boost dev package is installed.
- **Fix:** install Boost headers/libraries (`libboost-all-dev`) and rerun CMake.

### 2) GPS_Transmitter currently does not configure in this environment
- **Error:** CMake cannot find `UHDConfig.cmake` / `uhd-config.cmake` (UHD SDK missing).
- **Impact:** transmitter binary cannot be built until UHD dev package/toolchain is installed.
- **Fix:** install UHD development package and set `UHD_DIR` or `CMAKE_PREFIX_PATH` if needed.

### 3) Runtime bug in dynamic dt sender script
- **Issue:** previous script packed raw string input with `struct.pack("d", dt)`, which expects float.
- **Observed behavior:** broad `except:` swallowed the resulting exception, causing silent failure.
- **Fix applied:** converted input to `float`, added validation + user feedback, and explicit shutdown flow.

### 4) Naming mismatch in docs/scripts
- README refers to `dynamic_dt.py` while repository file is `dynamic_dt_correction.py`.
- This can confuse runtime operation and automation scripts.

## Suggested improvements
1. Add a **dependency check script** (Boost/UHD/Protobuf/GNSS-SDR) that fails fast with actionable install hints.
2. Add `--ip` and `--port` CLI options to all Python helper scripts (`motion.py`, `nmea_reader.py`, `dynamic_dt_correction.py`) to avoid hardcoded localhost/ports.
3. Replace broad `except:` blocks with targeted exceptions and logging.
4. Add a small integration harness (mock UDP receiver) to verify message formats before hardware tests.
5. Document expected run order in one command block (receiver -> monitor -> transmitter -> dynamic updates).
