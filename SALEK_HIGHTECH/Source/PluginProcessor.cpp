#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP/Voice.h"

//==============================================================================
SalekHightechAudioProcessor::SalekHightechAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                      ),
#else
     :
#endif
       apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    // Add voices
    for (int i = 0; i < maxVoices; ++i)
        synth.addVoice (new SalekVoice (apvts, modMatrix));

    synth.addSound (new SalekSound());

    // Init mod matrix with some default routes later
}

SalekHightechAudioProcessor::~SalekHightechAudioProcessor() {}

//==============================================================================
const juce::String SalekHightechAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SalekHightechAudioProcessor::acceptsMidi() const { return true; }
bool SalekHightechAudioProcessor::producesMidi() const { return false; }
bool SalekHightechAudioProcessor::isMidiEffect() const { return false; }
double SalekHightechAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int SalekHightechAudioProcessor::getNumPrograms() { return 1; }
int SalekHightechAudioProcessor::getCurrentProgram() { return 0; }
void SalekHightechAudioProcessor::setCurrentProgram (int) {}
const juce::String SalekHightechAudioProcessor::getProgramName (int) { return {}; }
void SalekHightechAudioProcessor::changeProgramName (int, const juce::String&) {}

//==============================================================================
void SalekHightechAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* v = dynamic_cast<SalekVoice*> (synth.getVoice (i)))
            v->prepare (sampleRate, samplesPerBlock);
    }
    modMatrix.prepare (sampleRate);
    arp.prepare (sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32) samplesPerBlock;
    spec.numChannels = (juce::uint32) getTotalNumOutputChannels();
    fx.prepare (spec);
}

void SalekHightechAudioProcessor::releaseResources() {}

bool SalekHightechAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}

void SalekHightechAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Macros
    for (int m = 0; m < 8; ++m)
    {
        auto* p = apvts.getRawParameterValue ("macro" + juce::String(m + 1));
        if (p) macros[m] = p->load();
    }

    // Update FX params from APVTS
    auto getf = [this](const char* id) -> float {
        if (auto* p = apvts.getRawParameterValue (id)) return p->load();
        return 0.0f;
    };
    fx.setDistortionDrive (getf ("dist_drive"));
    fx.setDelayTime (getf ("delay_time") * 0.75f + 0.01f);
    fx.setDelayFeedback (getf ("delay_feedback"));
    fx.setDelayMix (getf ("delay_mix"));
    fx.setReverbSize (getf ("reverb_size"));
    fx.setReverbMix (getf ("reverb_mix"));

    // Synth
    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());

    // FX chain (real DSP)
    fx.process (buffer);

    // Master
    float master = getf ("master_gain");
    buffer.applyGain (master * 0.9f);
}

//==============================================================================
bool SalekHightechAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* SalekHightechAudioProcessor::createEditor()
{
    return new SalekHightechAudioProcessorEditor (*this);
}

//==============================================================================
void SalekHightechAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SalekHightechAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout SalekHightechAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // === OSCILLATORS (3) ===
    for (int o = 1; o <= 3; ++o)
    {
        juce::String prefix = "osc" + juce::String(o) + "_";

        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            prefix + "level", "Osc " + juce::String(o) + " Level",
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), o == 1 ? 0.8f : 0.0f));

        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            prefix + "wtpos", "Osc " + juce::String(o) + " WT Pos",
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.0f));

        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            prefix + "morph", "Osc " + juce::String(o) + " Morph",
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.0f));

        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            prefix + "detune", "Osc " + juce::String(o) + " Detune",
            juce::NormalisableRange<float> (-24.0f, 24.0f, 0.01f), 0.0f));

        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            prefix + "octave", "Osc " + juce::String(o) + " Octave",
            juce::NormalisableRange<float> (-3.0f, 3.0f, 1.0f), 0.0f));

        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            prefix + "semitone", "Osc " + juce::String(o) + " Semi",
            juce::NormalisableRange<float> (-12.0f, 12.0f, 1.0f), 0.0f));

        // Warp / Wavefold amount
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            prefix + "warp", "Osc " + juce::String(o) + " Warp",
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));

        // FM amount (from previous osc or global)
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            prefix + "fm", "Osc " + juce::String(o) + " FM",
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    }

    // Sub
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "sub_level", "Sub Level", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.3f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "sub_octave", "Sub Octave", juce::NormalisableRange<float> (-2.0f, 0.0f, 1.0f), -1.0f));

    // Noise
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "noise_level", "Noise Level", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        "noise_type", "Noise Type", juce::StringArray {"White", "Pink", "Digital"}, 0));

    // === FILTER ===
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "filter_cutoff", "Filter Cutoff",
        juce::NormalisableRange<float> (20.0f, 20000.0f, 0.1f, 0.3f), 1200.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "filter_res", "Filter Resonance",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.2f));
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        "filter_type", "Filter Type",
        juce::StringArray {"Lowpass", "Highpass", "Bandpass", "Notch", "Ladder"}, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "filter_drive", "Filter Drive",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "filter_keytrack", "Filter Keytrack",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.5f));

    // === ENVELOPES (Amp + Filter + Mod) ===
    auto addEnv = [&](const juce::String& id, const juce::String& name)
    {
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            id + "_attack", name + " Attack",
            juce::NormalisableRange<float> (0.001f, 5.0f, 0.001f, 0.4f), 0.01f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            id + "_decay", name + " Decay",
            juce::NormalisableRange<float> (0.001f, 5.0f, 0.001f, 0.4f), 0.2f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            id + "_sustain", name + " Sustain",
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.7f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            id + "_release", name + " Release",
            juce::NormalisableRange<float> (0.001f, 8.0f, 0.001f, 0.4f), 0.3f));
    };
    addEnv ("env1", "Amp Env");
    addEnv ("env2", "Filter Env");
    addEnv ("env3", "Mod Env");

    // === LFOs ===
    for (int l = 1; l <= 4; ++l)
    {
        juce::String prefix = "lfo" + juce::String(l) + "_";
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            prefix + "rate", "LFO " + juce::String(l) + " Rate",
            juce::NormalisableRange<float> (0.01f, 40.0f, 0.01f, 0.4f), 1.0f));
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            prefix + "depth", "LFO " + juce::String(l) + " Depth",
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.5f));
        params.push_back (std::make_unique<juce::AudioParameterChoice> (
            prefix + "shape", "LFO " + juce::String(l) + " Shape",
            juce::StringArray {"Sine", "Triangle", "Saw", "Square", "S&H", "Chaos"}, 0));
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            prefix + "sync", "LFO " + juce::String(l) + " Sync", false));
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            prefix + "oneshot", "LFO " + juce::String(l) + " OneShot", false));
    }

    // === MACROS ===
    for (int m = 1; m <= 8; ++m)
    {
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            "macro" + juce::String(m), "Macro " + juce::String(m),
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.5f));
    }

    // Master
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "master_gain", "Master Gain",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.7f));

    // FX simple starters
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "dist_drive", "Distortion Drive",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "delay_time", "Delay Time",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.3f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "delay_feedback", "Delay Feedback",
        juce::NormalisableRange<float> (0.0f, 0.95f, 0.01f), 0.4f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "delay_mix", "Delay Mix",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "reverb_size", "Reverb Size",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.4f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "reverb_mix", "Reverb Mix",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));

    return { params.begin(), params.end() };
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SalekHightechAudioProcessor();
}