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

## Current Engine
- 3 oscillators + multi-frame wavetable (128 frames) with morph
- Warp modes: Fold / Phase Distortion / Sync / Mirror / Crush
- Real FM / AM / RM / PM
- Sub + Noise
- Multi-mode filter (SVF + Ladder) + drive
- 3 ADSR envelopes
- 4 LFOs (Sine/Tri/Saw/Square/S&H/Chaos + one-shot)
- Real modulation matrix (default routes active on open)
- 8 Macros
- Arpeggiator + step sequencer class
- Distortion → Delay → Reverb
- Preset save/load system
- 16-voice polyphony
- Original extreme UI (neon / acid / cyberpunk / void / large title)

## UI Pages
SYNTH · WAVETABLE · MOD · MATRIX · MACROS · FX · SEQ · PRESETS

Central animated visualizer + live matrix route preview.

## Build

```bash
# Get JUCE 8.x
git clone --depth 1 --branch 8.0.15 https://github.com/juce-framework/JUCE.git

cd SALEK_HIGHTECH
mkdir Builds && cd Builds
cmake .. -DCMAKE_BUILD_TYPE=Release -DJUCE_DIR=../../JUCE
cmake --build . -j$(nproc)
```

Linux packages (Ubuntu example):
```bash
sudo apt install libasound2-dev libfreetype6-dev libx11-dev libxcomposite-dev \
  libxcursor-dev libxext-dev libxinerama-dev libxrandr-dev libxrender-dev \
  libglu1-mesa-dev mesa-common-dev libjack-jackd2-dev libcurl4-openssl-dev \
  libwebkit2gtk-4.1-dev libgtk-3-dev
```

## Sound Design Starting Points
- Osc1 high WT + heavy Warp + FM from Osc2 → laser / screech
- Ladder filter + fast Filter Env + LFO1 → cutoff = acid / darkpsy movement
- Macro1 controls FM amount + resonance for performance
- Short delay + distortion for aggressive tails

SALEK HIGHTECH — free · loud · real.
