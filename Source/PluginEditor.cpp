#include "PluginEditor.h"

SalekHightechAudioProcessorEditor::SalekHightechAudioProcessorEditor (SalekHightechAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setLookAndFeel (&salekLnF);
    setSize (1360, 860);
    setResizable (true, true);
    setResizeLimits (1100, 720, 1920, 1200);

    titleLabel.setText ("SALEK HIGHTECH", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (juce::FontOptions (46.0f).withStyle ("Bold")));
    titleLabel.setColour (juce::Label::textColourId, juce::Colour (0xffff22ee));
    titleLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (titleLabel);

    pageLabel.setFont (juce::Font (juce::FontOptions (16.0f).withStyle ("Bold")));
    pageLabel.setColour (juce::Label::textColourId, juce::Colour (0xff00ffcc));
    pageLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (pageLabel);

    auto setupNav = [this](juce::TextButton& b, int page)
    {
        b.setClickingTogglesState (true);
        b.setRadioGroupId (9001);
        b.onClick = [this, page] { setPage (page); };
        addAndMakeVisible (b);
    };
    setupNav (synthBtn, 0);   synthBtn.setToggleState (true, juce::dontSendNotification);
    setupNav (wtBtn, 1);
    setupNav (modBtn, 2);
    setupNav (matrixBtn, 3);
    setupNav (macrosBtn, 4);
    setupNav (fxBtn, 5);
    setupNav (seqBtn, 6);
    setupNav (presetsBtn, 7);

    auto addSlider = [this](juce::Slider& s, const juce::String& id, std::unique_ptr<Att>& att)
    {
        setupSlider (s);
        addAndMakeVisible (s);
        att = std::make_unique<Att> (processor.getAPVTS(), id, s);
    };

    // Oscillators
    addSlider (osc1Level, "osc1_level", osc1LevelAtt);
    addSlider (osc1WTPos, "osc1_wtpos", osc1WTPosAtt);
    addSlider (osc1Warp,  "osc1_warp",  osc1WarpAtt);
    addSlider (osc1FM,    "osc1_fm",    osc1FMAtt);
    addSlider (osc2Level, "osc2_level", osc2LevelAtt);
    addSlider (osc2WTPos, "osc2_wtpos", osc2WTPosAtt);
    addSlider (osc2Warp,  "osc2_warp",  osc2WarpAtt);
    addSlider (osc2FM,    "osc2_fm",    osc2FMAtt);
    addSlider (osc3Level, "osc3_level", osc3LevelAtt);
    addSlider (osc3WTPos, "osc3_wtpos", osc3WTPosAtt);
    addSlider (osc3Warp,  "osc3_warp",  osc3WarpAtt);
    addSlider (osc3FM,    "osc3_fm",    osc3FMAtt);

    addSlider (filterCutoff, "filter_cutoff", filterCutoffAtt);
    addSlider (filterRes,    "filter_res",    filterResAtt);
    addSlider (filterDrive,  "filter_drive",  filterDriveAtt);
    addSlider (masterGain,   "master_gain",   masterGainAtt);
    addSlider (subLevel,     "sub_level",     subLevelAtt);
    addSlider (noiseLevel,   "noise_level",   noiseLevelAtt);

    addSlider (env1Attack,  "env1_attack",  env1AAtt);
    addSlider (env1Decay,   "env1_decay",   env1DAtt);
    addSlider (env1Sustain, "env1_sustain", env1SAtt);
    addSlider (env1Release, "env1_release", env1RAtt);

    addSlider (lfo1Rate,  "lfo1_rate",  lfo1RateAtt);
    addSlider (lfo1Depth, "lfo1_depth", lfo1DepthAtt);

    // FX
    addSlider (distDrive,     "dist_drive",     distAtt);
    addSlider (delayTime,     "delay_time",     delayTimeAtt);
    addSlider (delayFeedback, "delay_feedback", delayFbAtt);
    addSlider (delayMix,      "delay_mix",      delayMixAtt);
    addSlider (reverbSize,    "reverb_size",    revSizeAtt);
    addSlider (reverbMix,     "reverb_mix",     revMixAtt);

    for (int i = 0; i < 8; ++i)
    {
        setupSlider (macroSliders[i]);
        macroSliders[i].setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xffffaa00));
        addAndMakeVisible (macroSliders[i]);
        macroAtts[i] = std::make_unique<Att> (processor.getAPVTS(), "macro" + juce::String (i + 1), macroSliders[i]);
    }

    // Default useful mod routes so it immediately feels alive
    auto& matrix = processor.getModMatrix();
    matrix.addRoute (ModulationMatrix::LFO1, ModulationMatrix::FilterCutoff, 0.45f);
    matrix.addRoute (ModulationMatrix::LFO1, ModulationMatrix::Osc1WTPos, 0.35f);
    matrix.addRoute (ModulationMatrix::Env2, ModulationMatrix::FilterCutoff, 0.7f);
    matrix.addRoute (ModulationMatrix::LFO2, ModulationMatrix::Osc1Warp, 0.4f);
    matrix.addRoute (ModulationMatrix::Macro1, ModulationMatrix::Osc1FM, 0.5f);
    matrix.addRoute (ModulationMatrix::Macro1, ModulationMatrix::FilterRes, 0.3f);

    waveformDisplay.resize (512, 0.0f);
    setPage (0);
    startTimerHz (28);
}

SalekHightechAudioProcessorEditor::~SalekHightechAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void SalekHightechAudioProcessorEditor::setupSlider (juce::Slider& s)
{
    s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 58, 16);
    s.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff00ffcc));
    s.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

void SalekHightechAudioProcessorEditor::setPage (int page)
{
    currentPage = page;
    static const char* names[] = { "SYNTH", "WAVETABLE", "MOD", "MATRIX", "MACROS", "FX", "SEQ", "PRESETS" };
    pageLabel.setText (juce::String ("// ") + names[juce::jlimit (0, 7, page)], juce::dontSendNotification);
    updateVisibility();
    resized();
    repaint();
}

void SalekHightechAudioProcessorEditor::updateVisibility()
{
    auto showSynth = currentPage == 0 || currentPage == 1;
    auto showMod   = currentPage == 2 || currentPage == 0;
    auto showFx    = currentPage == 5;
    auto showMacros= currentPage == 4 || currentPage == 0;

    osc1Level.setVisible (showSynth); osc1WTPos.setVisible (showSynth);
    osc1Warp.setVisible (showSynth);  osc1FM.setVisible (showSynth);
    osc2Level.setVisible (showSynth); osc2WTPos.setVisible (showSynth);
    osc2Warp.setVisible (showSynth);  osc2FM.setVisible (showSynth);
    osc3Level.setVisible (showSynth); osc3WTPos.setVisible (showSynth);
    osc3Warp.setVisible (showSynth);  osc3FM.setVisible (showSynth);
    filterCutoff.setVisible (showSynth); filterRes.setVisible (showSynth);
    filterDrive.setVisible (showSynth); masterGain.setVisible (true);
    subLevel.setVisible (showSynth); noiseLevel.setVisible (showSynth);

    env1Attack.setVisible (showMod); env1Decay.setVisible (showMod);
    env1Sustain.setVisible (showMod); env1Release.setVisible (showMod);
    lfo1Rate.setVisible (showMod); lfo1Depth.setVisible (showMod);

    distDrive.setVisible (showFx); delayTime.setVisible (showFx);
    delayFeedback.setVisible (showFx); delayMix.setVisible (showFx);
    reverbSize.setVisible (showFx); reverbMix.setVisible (showFx);

    for (auto& m : macroSliders)
        m.setVisible (showMacros);
}

void SalekHightechAudioProcessorEditor::paint (juce::Graphics& g)
{
    paintBackground (g);
    paintTitle (g);
    paintVisualizer (g);

    if (currentPage == 3)
        paintMatrixPreview (g);

    g.setColour (juce::Colour (0xff00ffcc));
    g.setFont (juce::Font (juce::FontOptions (13.0f).withStyle ("Bold")));

    if (currentPage == 0 || currentPage == 1)
    {
        g.drawText ("OSC 1", 40, 125, 200, 18, juce::Justification::centred);
        g.drawText ("OSC 2", 280, 125, 200, 18, juce::Justification::centred);
        g.drawText ("OSC 3", 520, 125, 200, 18, juce::Justification::centred);
        g.drawText ("FILTER", 780, 125, 180, 18, juce::Justification::centred);
        g.drawText ("SUB / NOISE", 40, 380, 200, 18, juce::Justification::centredLeft);
    }
    if (currentPage == 4 || currentPage == 0)
        g.drawText ("MACROS  (assign to anything)", 40, 520, 400, 18, juce::Justification::centredLeft);
    if (currentPage == 5)
        g.drawText ("FX CHAIN  //  DIST → DELAY → REVERB", 40, 140, 500, 20, juce::Justification::centredLeft);
    if (currentPage == 3)
        g.drawText ("MODULATION MATRIX  (live routes)", 40, 140, 400, 20, juce::Justification::centredLeft);
}

void SalekHightechAudioProcessorEditor::paintBackground (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff020008));

    juce::ColourGradient vg (juce::Colour (0xff14002a), 0, 0,
                             juce::Colour (0xff000006), 0, (float) getHeight(), false);
    g.setGradientFill (vg);
    g.fillAll();

    // Circuit / energy lines
    g.setColour (juce::Colour (0xff00ffcc).withAlpha (0.11f));
    for (int i = 0; i < 28; ++i)
    {
        float x = std::fmod ((float)(i * 55) + phaseAnim * 40.0f, (float) getWidth() + 60.0f) - 30.0f;
        g.drawLine (x, 0.0f, x + 50.0f, (float) getHeight(), 1.2f);
    }

    g.setColour (juce::Colour (0xffff00aa).withAlpha (0.07f));
    for (int i = -6; i < 18; ++i)
    {
        float y = i * 70.0f + std::sin (phaseAnim * 1.3f + i * 0.7f) * 25.0f;
        g.drawLine (0.0f, y, (float) getWidth(), y + 140.0f, 2.0f);
    }

    // Central holographic glow
    auto c = getLocalBounds().getCentre().toFloat();
    juce::ColourGradient cg (juce::Colour (0xffff00cc).withAlpha (0.13f), c.x, c.y - 120.0f,
                             juce::Colours::transparentBlack, c.x, c.y + 220.0f, true);
    g.setGradientFill (cg);
    g.fillEllipse (c.x - 300.0f, c.y - 200.0f, 600.0f, 400.0f);
}

void SalekHightechAudioProcessorEditor::paintTitle (juce::Graphics& g)
{
    auto area = juce::Rectangle<float> (0, 4, (float) getWidth(), 64);

    g.setColour (juce::Colour (0xffff00ee).withAlpha (0.35f));
    g.setFont (juce::Font (juce::FontOptions (54.0f).withStyle ("Bold")));
    g.drawText ("SALEK HIGHTECH", area.translated (1, 2), juce::Justification::centred);
    g.drawText ("SALEK HIGHTECH", area.translated (-1, 1), juce::Justification::centred);

    juce::ColourGradient tg (juce::Colour (0xffff33ee), area.getX(), area.getY(),
                             juce::Colour (0xff00ffee), area.getRight(), area.getBottom(), false);
    g.setGradientFill (tg);
    g.setFont (juce::Font (juce::FontOptions (50.0f).withStyle ("Bold")));
    g.drawText ("SALEK HIGHTECH", area, juce::Justification::centred);

    g.setColour (juce::Colour (0xff00ffcc).withAlpha (0.75f));
    g.setFont (juce::Font (juce::FontOptions (12.0f)));
    g.drawText ("HYPERCORE  //  FREE  //  170-190+ BPM  //  DARKPSY  //  HI-TECH  //  ALIEN  //  NO LIMITS",
                0, 58, getWidth(), 16, juce::Justification::centred);
}

void SalekHightechAudioProcessorEditor::paintVisualizer (juce::Graphics& g)
{
    auto viz = juce::Rectangle<float> ((float) getWidth() * 0.58f, 150.0f, 480.0f, 240.0f);
    g.setColour (juce::Colour (0xff080018).withAlpha (0.9f));
    g.fillRoundedRectangle (viz, 10.0f);
    g.setColour (juce::Colour (0xff00ffcc).withAlpha (0.65f));
    g.drawRoundedRectangle (viz, 10.0f, 2.0f);

    juce::Path wave;
    bool started = false;
    for (size_t i = 0; i < waveformDisplay.size(); ++i)
    {
        float x = viz.getX() + 12.0f + (viz.getWidth() - 24.0f) * (float) i / (float)(waveformDisplay.size() - 1);
        float y = viz.getCentreY() + waveformDisplay[i] * (viz.getHeight() * 0.38f);
        if (!started) { wave.startNewSubPath (x, y); started = true; }
        else wave.lineTo (x, y);
    }
    g.setColour (juce::Colour (0xffff00aa));
    g.strokePath (wave, juce::PathStrokeType (2.2f));

    // Spectrum bars
    g.setColour (juce::Colour (0xff00ffee).withAlpha (0.45f));
    for (int i = 0; i < 36; ++i)
    {
        float h = (0.25f + 0.75f * std::abs (std::sin (phaseAnim * 2.2f + i * 0.35f))) * 48.0f;
        float x = viz.getX() + 18.0f + i * 12.5f;
        g.fillRect (x, viz.getBottom() - 18.0f - h, 9.0f, h);
    }
}

void SalekHightechAudioProcessorEditor::paintMatrixPreview (juce::Graphics& g)
{
    auto& routes = processor.getModMatrix().getRoutes();
    g.setColour (juce::Colour (0xff00ffcc));
    g.setFont (juce::Font (juce::FontOptions (13.0f)));

    int y = 180;
    g.drawText ("ACTIVE ROUTES:", 50, y, 400, 20, juce::Justification::left);
    y += 28;
    for (size_t i = 0; i < routes.size() && i < 12; ++i)
    {
        auto& r = routes[i];
        juce::String line = "SRC " + juce::String (r.sourceId) + "  →  DEST " + juce::String (r.destId)
                          + "   amt " + juce::String (r.amount, 2);
        g.setColour (r.enabled ? juce::Colour (0xffff00aa) : juce::Colour (0xff666666));
        g.drawText (line, 50, y, 600, 18, juce::Justification::left);
        y += 22;
    }
    if (routes.empty())
        g.drawText ("(no routes yet — defaults are added on open)", 50, y, 500, 20, juce::Justification::left);
}

void SalekHightechAudioProcessorEditor::resized()
{
    auto r = getLocalBounds();
    titleLabel.setBounds (r.removeFromTop (70));
    pageLabel.setBounds (20, 70, 300, 22);

    auto nav = r.removeFromTop (34);
    int bw = nav.getWidth() / 8;
    synthBtn.setBounds   (nav.removeFromLeft (bw).reduced (2));
    wtBtn.setBounds      (nav.removeFromLeft (bw).reduced (2));
    modBtn.setBounds     (nav.removeFromLeft (bw).reduced (2));
    matrixBtn.setBounds  (nav.removeFromLeft (bw).reduced (2));
    macrosBtn.setBounds  (nav.removeFromLeft (bw).reduced (2));
    fxBtn.setBounds      (nav.removeFromLeft (bw).reduced (2));
    seqBtn.setBounds     (nav.removeFromLeft (bw).reduced (2));
    presetsBtn.setBounds (nav.reduced (2));

    r.removeFromTop (16);

    int k = 72;
    auto place4 = [&](int x, int y, juce::Slider& a, juce::Slider& b, juce::Slider& c, juce::Slider& d)
    {
        a.setBounds (x, y, k, k + 18);
        b.setBounds (x + 85, y, k, k + 18);
        c.setBounds (x, y + 100, k, k + 18);
        d.setBounds (x + 85, y + 100, k, k + 18);
    };

    if (currentPage == 0 || currentPage == 1)
    {
        place4 (40,  155, osc1Level, osc1WTPos, osc1Warp, osc1FM);
        place4 (280, 155, osc2Level, osc2WTPos, osc2Warp, osc2FM);
        place4 (520, 155, osc3Level, osc3WTPos, osc3Warp, osc3FM);

        filterCutoff.setBounds (800, 155, 85, 100);
        filterRes.setBounds    (900, 155, 85, 100);
        filterDrive.setBounds  (1000, 155, 85, 100);
        masterGain.setBounds   (1100, 155, 85, 100);

        subLevel.setBounds (40, 410, k, k + 18);
        noiseLevel.setBounds (130, 410, k, k + 18);
    }

    if (currentPage == 0 || currentPage == 2)
    {
        env1Attack.setBounds  (40,  155, k, k + 18);
        env1Decay.setBounds   (130, 155, k, k + 18);
        env1Sustain.setBounds (220, 155, k, k + 18);
        env1Release.setBounds (310, 155, k, k + 18);
        lfo1Rate.setBounds    (40,  280, k, k + 18);
        lfo1Depth.setBounds   (130, 280, k, k + 18);
    }

    if (currentPage == 5)
    {
        distDrive.setBounds     (40,  180, k + 10, k + 20);
        delayTime.setBounds     (150, 180, k + 10, k + 20);
        delayFeedback.setBounds (260, 180, k + 10, k + 20);
        delayMix.setBounds      (370, 180, k + 10, k + 20);
        reverbSize.setBounds    (480, 180, k + 10, k + 20);
        reverbMix.setBounds     (590, 180, k + 10, k + 20);
    }

    if (currentPage == 4 || currentPage == 0)
    {
        for (int i = 0; i < 8; ++i)
            macroSliders[i].setBounds (40 + i * 95, 560, 85, 105);
    }
}

void SalekHightechAudioProcessorEditor::timerCallback()
{
    phaseAnim += 0.048f;
    if (phaseAnim > juce::MathConstants<float>::twoPi)
        phaseAnim -= juce::MathConstants<float>::twoPi;

    for (size_t i = 0; i < waveformDisplay.size(); ++i)
    {
        float t = (float) i / (float) waveformDisplay.size();
        waveformDisplay[i] = 0.55f * std::sin (t * 7.5f * juce::MathConstants<float>::twoPi + phaseAnim * 2.8f)
                           + 0.28f * std::sin (t * 17.0f * juce::MathConstants<float>::twoPi - phaseAnim * 1.9f)
                           + 0.14f * std::sin (t * 31.0f * juce::MathConstants<float>::twoPi + phaseAnim * 3.1f);
    }
    repaint();
}
