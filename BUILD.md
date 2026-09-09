# 🔥 BUILD SALEK HIGHTECH VST3

**SALEK HIGHTECH** is now ready to compile. Follow the steps below for your platform.

---

## 🚀 Quick Start (All Platforms)

### Prerequisites
- **CMake** 3.22+
- **C++17 compiler**
- **JUCE 8.0.15** (auto-downloaded)

---

## 📦 Linux (Ubuntu/Debian)

### 1. Install Dependencies
```bash
sudo apt-get update
sudo apt-get install -y clang cmake ninja-build pkg-config \
  libasound2-dev libfreetype6-dev libx11-dev libxcomposite-dev \
  libxcursor-dev libxext-dev libxinerama-dev libxrandr-dev libxrender-dev \
  libglu1-mesa-dev mesa-common-dev libjack-jackd2-dev libcurl4-openssl-dev \
  libwebkit2gtk-4.1-dev libgtk-3-dev
```

### 2. Clone JUCE
```bash
git clone --depth 1 --branch 8.0.15 https://github.com/juce-framework/JUCE.git
```

### 3. Build
```bash
cd SALEK_HIGHTECH
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DJUCE_DIR=../JUCE
cmake --build . -j$(nproc)
```

### 4. Find Your VST3
```bash
ls -lh build/SALEK_HIGHTECH_artefacts/Release/VST3/
# Copy to ~/.vst3/
cp -r build/SALEK_HIGHTECH_artefacts/Release/VST3/* ~/.vst3/
```

---

## 🍎 macOS

### 1. Install Dependencies
```bash
# Install Xcode Command Line Tools if not already installed
xcode-select --install

# Install CMake and Ninja
brew install cmake ninja
```

### 2. Clone JUCE
```bash
git clone --depth 1 --branch 8.0.15 https://github.com/juce-framework/JUCE.git
```

### 3. Build
```bash
cd SALEK_HIGHTECH
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DJUCE_DIR=../JUCE
cmake --build . -j$(sysctl -n hw.ncpu)
```

### 4. Find Your VST3
```bash
ls -lh build/SALEK_HIGHTECH_artefacts/Release/VST3/
# Copy to ~/Library/Audio/Plug-Ins/VST3/
cp -r build/SALEK_HIGHTECH_artefacts/Release/VST3/* ~/Library/Audio/Plug-Ins/VST3/
```

---

## 🪟 Windows (Visual Studio 2022)

### 1. Install Requirements
- **Visual Studio 2022 Community** (with "Desktop development with C++")
- **CMake** 3.22+ (or use Visual Studio's built-in)
- **PowerShell**

### 2. Clone JUCE
```powershell
git clone --depth 1 --branch 8.0.15 https://github.com/juce-framework/JUCE.git
```

### 3. Build
```powershell
cd SALEK_HIGHTECH
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DJUCE_DIR=../JUCE
cmake --build . --config Release
```

### 4. Find Your VST3
```powershell
dir build\SALEK_HIGHTECH_artefacts\Release\VST3\
# Copy to C:\Program Files\Common Files\VST3\
Copy-Item -Recurse build\SALEK_HIGHTECH_artefacts\Release\VST3\* "C:\Program Files\Common Files\VST3\"
```

---

## ☁️ GitHub Actions (Automatic CI/CD)

Push to `main` and GitHub Actions will automatically build for **Linux**, **macOS**, and **Windows**.

1. Go to **Actions** tab
2. Select **Build SALEK HIGHTECH VST3**
3. Download artifacts for each platform

---

## 🎵 Load in Your DAW

### Supported DAWs
- **Ableton Live** (11+)
- **FL Studio** (21+)
- **Reaper** (6.82+)
- **Bitwig Studio** (4.0+)
- **Studio One** (5.0+)
- **Cubase** (12+)
- **Logic Pro** (via wrapper)

### Default Paths

| OS | Path |
|---|---|
| **Linux** | `~/.vst3/` |
| **macOS** | `~/Library/Audio/Plug-Ins/VST3/` |
| **Windows** | `C:\Program Files\Common Files\VST3\` |

---

## 🔧 Troubleshooting

### CMake not found
```bash
# Linux
sudo apt-get install cmake

# macOS
brew install cmake

# Windows: Download from cmake.org
```

### JUCE not found
```bash
# Make sure JUCE_DIR points correctly
cmake .. -DJUCE_DIR=/full/path/to/JUCE
```

### Permission denied (Linux/macOS)
```bash
chmod +x build/SALEK_HIGHTECH_artefacts/Release/VST3/*
```

### Compilation errors
- Ensure C++17 compiler support
- Update CMake to latest version
- Try `cmake --build . --verbose` for details

---

## 🎨 What You Get

✅ **3 Wavetable Oscillators** with morphing + warp modes
✅ **Multi-mode Filter** (Lowpass, Highpass, Bandpass, Notch, Ladder)
✅ **ADSR Envelopes** (Amp, Filter, Mod)
✅ **4 LFOs** (Sine, Triangle, Saw, Square, S&H, Chaos)
✅ **Modulation Matrix** (24 sources × 48 destinations, 96 routes)
✅ **8 Macros** for live control
✅ **FX Chain** (Distortion → Delay → Reverb)
✅ **Arpeggiator + Step Sequencer**
✅ **Preset Save/Load**
✅ **16-voice Polyphony**
✅ **Psychedelic Neon UI** (1360×860px, resizable)

---

## 🚀 Next Steps

1. **Build the VST3** following the steps above
2. **Copy to your VST3 folder**
3. **Scan plugins in your DAW**
4. **Load SALEK HIGHTECH**
5. **Make hi-tech noise** 🔥

---

## 📄 License

Free · Unlocked · No feature gates

All synthesis features available to everyone. Create presets, wavetables, LFO shapes, and sequences freely.

---

## 💬 Support

Issues? Check:
- JUCE documentation: https://juce.com/learn/documentation
- CMake guide: https://cmake.org/cmake/help/latest/
- GitHub Issues: Report build problems here

---

**SALEK HIGHTECH — Real synthesis. Real power. No limits.**

🔥 170-190+ BPM · Darkpsy · Hi-Tech · FM Bass · Screech · Metallic · Alien · Cyberpunk · Laser
