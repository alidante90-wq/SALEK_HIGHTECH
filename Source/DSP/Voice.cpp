#include "Voice.h"

SalekVoice::SalekVoice (juce::AudioProcessorValueTreeState& apvts, ModulationMatrix& matrix)
    : apvts (apvts), modMatrix (matrix)
{
}

bool SalekVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<SalekSound*> (sound) != nullptr;
}

void SalekVoice::prepare (double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    osc1.prepare (sampleRate);
    osc2.prepare (sampleRate);
    osc3.prepare (sampleRate);
    subOsc.prepare (sampleRate);
    filter.prepare ({ sampleRate, (juce::uint32) samplesPerBlock, 1 });
    ampEnv.prepare (sampleRate);
    filterEnv.prepare (sampleRate);
    modEnv.prepare (sampleRate);
    lfo1.prepare (sampleRate);
    lfo2.prepare (sampleRate);
    lfo3.prepare (sampleRate);
    lfo4.prepare (sampleRate);
    noiseRandom.setSeedRandomly();
    isPrepared = true;
}

void SalekVoice::startNote (int midiNoteNumber, float vel, juce::SynthesiserSound*, int)
{
    currentNote = midiNoteNumber;
    velocity = vel;
    noteHz = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
    ampEnv.noteOn();
    filterEnv.noteOn();
    modEnv.noteOn();
    lfo1.reset();
    lfo2.reset();
    lfo3.reset();
    lfo4.reset();
}

void SalekVoice::stopNote (float, bool allowTailOff)
{
    if (allowTailOff)
    {
        ampEnv.noteOff();
        filterEnv.noteOff();
        modEnv.noteOff();
    }
    else
    {
        clearCurrentNote();
    }
}

void SalekVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (!isPrepared) return;

    updateParameters();

    for (int i = 0; i < numSamples; ++i)
    {
        // Update modulation sources
        modSources[ModulationMatrix::LFO1] = lfo1.process();
        modSources[ModulationMatrix::LFO2] = lfo2.process();
        modSources[ModulationMatrix::LFO3] = lfo3.process();
        modSources[ModulationMatrix::LFO4] = lfo4.process();
        modSources[ModulationMatrix::Env1] = ampEnv.getLevel();
        modSources[ModulationMatrix::Env2] = filterEnv.getLevel();
        modSources[ModulationMatrix::Env3] = modEnv.getLevel();
        modSources[ModulationMatrix::Velocity] = velocity;
        modSources[ModulationMatrix::NoteNumber] = currentNote / 127.0f;
        modSources[ModulationMatrix::ModWheel] = 0.5f; // placeholder
        modSources[ModulationMatrix::Macro1] = 0.5f; // will be set from processor
        modSources[ModulationMatrix::Macro2] = 0.5f;
        modSources[ModulationMatrix::Macro3] = 0.5f;
        modSources[ModulationMatrix::Macro4] = 0.5f;
        modSources[ModulationMatrix::Macro5] = 0.5f;
        modSources[ModulationMatrix::Macro6] = 0.5f;
        modSources[ModulationMatrix::Macro7] = 0.5f;
        modSources[ModulationMatrix::Macro8] = 0.5f;

        // Clear mod destinations
        std::fill (modDests, modDests + ModulationMatrix::numDests, 0.0f);

        // Process modulation matrix
        modMatrix.process (modSources, modDests);

        // Apply modulation and render
        applyModulation (modDests);

        // OSCs
        float osc1Out = osc1.process();
        float osc2Out = osc2.process();
        float osc3Out = osc3.process();
        float subOut = subOsc.process();

        // Noise
        float noise = 0.0f;
        if (noiseLevel > 0.001f)
            noise = (noiseRandom.nextFloat() * 2.0f - 1.0f) * noiseLevel;

        // Mix
        float mixed = osc1Out + osc2Out + osc3Out + subOut + noise;

        // Filter
        float filtered = filter.processSample (mixed);

        // Envelopes
        float ampLev = ampEnv.process();
        filterEnv.process();
        modEnv.process();

        // Output
        float out = filtered * ampLev * velocity * 0.5f;

        for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
            outputBuffer.addSample (ch, startSample + i, out);
    }

    if (!ampEnv.isActive())
        clearCurrentNote();
}

void SalekVoice::updateParameters()
{
    auto getf = [this](const char* id) -> float {
        if (auto* p = apvts.getRawParameterValue (id)) return p->load();
        return 0.0f;
    };

    // OSC 1
    float freq1 = noteHz * std::pow (2.0f, (getf ("osc1_octave") + getf ("osc1_semitone") / 12.0f + getf ("osc1_detune") / 1200.0f));
    osc1.setFrequency (freq1);
    osc1.setWavetablePos (getf ("osc1_wtpos"));
    osc1.setMorph (getf ("osc1_morph"));
    osc1.setWarp (getf ("osc1_warp"));
    osc1.setFM (getf ("osc1_fm") * osc2.getLastSample() * 200.0f);
    osc1.setLevel (getf ("osc1_level"));

    // OSC 2
    float freq2 = noteHz * std::pow (2.0f, (getf ("osc2_octave") + getf ("osc2_semitone") / 12.0f + getf ("osc2_detune") / 1200.0f));
    osc2.setFrequency (freq2);
    osc2.setWavetablePos (getf ("osc2_wtpos"));
    osc2.setMorph (getf ("osc2_morph"));
    osc2.setWarp (getf ("osc2_warp"));
    osc2.setFM (getf ("osc2_fm") * osc1.getLastSample() * 200.0f);
    osc2.setLevel (getf ("osc2_level"));

    // OSC 3
    float freq3 = noteHz * std::pow (2.0f, (getf ("osc3_octave") + getf ("osc3_semitone") / 12.0f + getf ("osc3_detune") / 1200.0f));
    osc3.setFrequency (freq3);
    osc3.setWavetablePos (getf ("osc3_wtpos"));
    osc3.setMorph (getf ("osc3_morph"));
    osc3.setWarp (getf ("osc3_warp"));
    osc3.setFM (getf ("osc3_fm") * osc2.getLastSample() * 200.0f);
    osc3.setLevel (getf ("osc3_level"));

    // Sub
    float subFreq = noteHz * std::pow (2.0f, getf ("sub_octave"));
    subOsc.setFrequency (subFreq);
    subOsc.setLevel (getf ("sub_level"));
    subOsc.setWavetablePos (0.0f);

    // Noise
    noiseLevel = getf ("noise_level");

    // Filter
    float cutoff = getf ("filter_cutoff");
    float keytrack = getf ("filter_keytrack");
    cutoff *= std::pow (2.0f, (currentNote - 60) * keytrack / 12.0f);
    cutoff += modDests[ModulationMatrix::FilterCutoff] * 3000.0f;
    filter.setCutoff (juce::jlimit (20.0f, 20000.0f, cutoff));
    filter.setResonance (getf ("filter_res") + modDests[ModulationMatrix::FilterRes] * 0.4f);
    filter.setDrive (getf ("filter_drive"));

    // Envelopes
    ampEnv.setAttack (getf ("env1_attack"));
    ampEnv.setDecay (getf ("env1_decay"));
    ampEnv.setSustain (getf ("env1_sustain"));
    ampEnv.setRelease (getf ("env1_release"));

    filterEnv.setAttack (getf ("env2_attack"));
    filterEnv.setDecay (getf ("env2_decay"));
    filterEnv.setSustain (getf ("env2_sustain"));
    filterEnv.setRelease (getf ("env2_release"));

    // LFOs
    lfo1.setRate (getf ("lfo1_rate"));
    lfo1.setDepth (getf ("lfo1_depth"));
    lfo1.setShape ((LFO::Shape)(int)getf ("lfo1_shape"));

    lfo2.setRate (getf ("lfo2_rate"));
    lfo2.setDepth (getf ("lfo2_depth"));
    lfo2.setShape ((LFO::Shape)(int)getf ("lfo2_shape"));

    lfo3.setRate (getf ("lfo3_rate"));
    lfo3.setDepth (getf ("lfo3_depth"));
    lfo3.setShape ((LFO::Shape)(int)getf ("lfo3_shape"));

    lfo4.setRate (getf ("lfo4_rate"));
    lfo4.setDepth (getf ("lfo4_depth"));
    lfo4.setShape ((LFO::Shape)(int)getf ("lfo4_shape"));
}

void SalekVoice::applyModulation (float* destOffsets)
{
    // Destinations are already offset by modMatrix
    // Just apply them to parameters that support modulation
}
