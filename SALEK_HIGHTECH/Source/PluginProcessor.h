#pragma once

#include <JuceHeader.h>
#include "DSP/Voice.h"
#include "DSP/ModulationMatrix.h"
#include "DSP/FXProcessor.h"
#include "DSP/Arpeggiator.h"

//==============================================================================
class SalekHightechAudioProcessor : public juce::AudioProcessor
{
public:
    SalekHightechAudioProcessor();
    ~SalekHightechAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    ModulationMatrix& getModMatrix() { return modMatrix; }
    juce::Synthesiser& getSynth() { return synth; }
    Arpeggiator& getArp() { return arp; }
    FXProcessor& getFX() { return fx; }

    std::array<float, 8> macros {0.5f};

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::Synthesiser synth;
    ModulationMatrix modMatrix;
    Arpeggiator arp;
    FXProcessor fx;

    static constexpr int maxVoices = 16;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SalekHightechAudioProcessor)
};
