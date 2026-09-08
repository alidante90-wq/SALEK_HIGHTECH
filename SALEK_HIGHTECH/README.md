# ████ SALEK HIGHTECH ████

**Real working VST3 synthesizer** — free, unlocked, no feature gates.

Built for 170–190+ BPM Hi-Tech / Darkpsy / Psychedelic / FM Bass / Screech / Metallic / Alien / Cyberpunk / Laser.

## Free User Philosophy
Everything is available:
- Create and save presets
- Use all oscillators, warps, FM/AM/RM, filters, envelopes, LFOs
- Full modulation matrix
- Macros, arpeggiator, step sequencer skeleton
- FX chain
- No locked synthesis features

## Build (Local)

### Linux / macOS
```bash
git clone --depth 1 --branch 8.0.15 https://github.com/juce-framework/JUCE.git
cd SALEK_HIGHTECH
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DJUCE_DIR=../JUCE
cmake --build . -j
```

### Windows (PowerShell + Visual Studio 2022)
1. Install Visual Studio 2022 Community with “Desktop development with C++”
2. Open PowerShell:

```powershell
cd C:\Users\YOURNAME\SALEK_HIGHTECH
git clone --depth 1 --branch 8.0.15 https://github.com/juce-framework/JUCE.git
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DJUCE_DIR="C:/Users/YOURNAME/SALEK_HIGHTECH/JUCE"
cmake --build . --config Release
```

The VST3 will be in `build\SALEK_HIGHTECH_artefacts\Release\VST3\`

## GitHub Actions (CI)

A ready workflow is included at:

`.github/workflows/build.yml`

It installs dependencies, clones JUCE, builds with CMake + Ninja, and uploads the VST3 artifact.

**Do NOT use `./configure`** — this is a pure CMake + JUCE project.

Push to GitHub → Actions tab → run “Build SALEK HIGHTECH” → download the artifact.

## Linux dependencies
```bash
sudo apt install clang cmake ninja-build pkg-config \
  libasound2-dev libfreetype6-dev libx11-dev libxcomposite-dev \
  libxcursor-dev libxext-dev libxinerama-dev libxrandr-dev libxrender-dev \
  libglu1-mesa-dev mesa-common-dev libjack-jackd2-dev libcurl4-openssl-dev \
  libwebkit2gtk-4.1-dev libgtk-3-dev
```

SALEK HIGHTECH — free · loud · real.
