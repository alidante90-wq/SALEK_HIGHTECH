#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/LookAndFeel_SALEK.h"

class SalekHightechAudioProcessorEditor : public juce::AudioProcessorEditor,
                                          private juce::Timer
{
public:
    SalekHightechAudioProcessorEditor (SalekHightechAudioProcessor&);
    ~SalekHightechAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    void setPage (int page);
    void setupSlider (juce::Slider& s);
    void paintBackground (juce::Graphics& g);
    void paintTitle (juce::Graphics& g);
    void paintVisualizer (juce::Graphics& g);
    void paintMatrixPreview (juce::Graphics& g);
    void updateVisibility();

    SalekHightechAudioProcessor& processor;
    LookAndFeel_SALEK salekLnF;

    int currentPage = 0; // 0 SYNTH, 1 WAVETABLE, 2 MOD, 3 MATRIX, 4 MACROS, 5 FX, 6 SEQ, 7 PRESETS

    juce::Label titleLabel;
    juce::Label pageLabel;

    juce::TextButton synthBtn {"SYNTH"}, wtBtn {"WAVETABLE"}, modBtn {"MOD"},
                     matrixBtn {"MATRIX"}, macrosBtn {"MACROS"}, fxBtn {"FX"},
                     seqBtn {"SEQ"}, presetsBtn {"PRESETS"};

    // SYNTH page controls
    juce::Slider osc1Level, osc1WTPos, osc1Warp, osc1FM;
    juce::Slider osc2Level, osc2WTPos, osc2Warp, osc2FM;
    juce::Slider osc3Level, osc3WTPos, osc3Warp, osc3FM;
    juce::Slider filterCutoff, filterRes, filterDrive;
    juce::Slider masterGain;
    juce::Slider subLevel, noiseLevel;

    // ENV / LFO quick access
    juce::Slider env1Attack, env1Decay, env1Sustain, env1Release;
    juce::Slider lfo1Rate, lfo1Depth;

    // FX page
    juce::Slider distDrive, delayTime, delayFeedback, delayMix, reverbSize, reverbMix;

    // Macros
    std::array<juce::Slider, 8> macroSliders;

    // Attachments
    using Att = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Att> osc1LevelAtt, osc1WTPosAtt, osc1WarpAtt, osc1FMAtt;
    std::unique_ptr<Att> osc2LevelAtt, osc2WTPosAtt, osc2WarpAtt, osc2FMAtt;
    std::unique_ptr<Att> osc3LevelAtt, osc3WTPosAtt, osc3WarpAtt, osc3FMAtt;
    std::unique_ptr<Att> filterCutoffAtt, filterResAtt, filterDriveAtt, masterGainAtt;
    std::unique_ptr<Att> subLevelAtt, noiseLevelAtt;
    std::unique_ptr<Att> env1AAtt, env1DAtt, env1SAtt, env1RAtt;
    std::unique_ptr<Att> lfo1RateAtt, lfo1DepthAtt;
    std::unique_ptr<Att> distAtt, delayTimeAtt, delayFbAtt, delayMixAtt, revSizeAtt, revMixAtt;
    std::array<std::unique_ptr<Att>, 8> macroAtts;

    // Visualizer
    std::vector<float> waveformDisplay;
    float phaseAnim = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SalekHightechAudioProcessorEditor)
};
