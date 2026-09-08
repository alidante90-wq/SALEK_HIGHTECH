cmake_minimum_required(VERSION 3.22)

project(SALEK_HIGHTECH VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Point to JUCE source (set -DJUCE_DIR=/path/to/JUCE if needed)
set(JUCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE_CLEAN" CACHE PATH "Path to JUCE")
if(NOT EXISTS "${JUCE_DIR}/CMakeLists.txt")
    set(JUCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE-8.0.15" CACHE PATH "Path to JUCE" FORCE)
endif()
if(NOT EXISTS "${JUCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "JUCE not found at ${JUCE_DIR}. Download JUCE 8.x from github.com/juce-framework/JUCE and place it next to this project or pass -DJUCE_DIR=...")
endif()

add_subdirectory(${JUCE_DIR} ${CMAKE_BINARY_DIR}/JUCE EXCLUDE_FROM_ALL)

juce_add_plugin(SALEK_HIGHTECH
    COMPANY_NAME "SALEK"
    BUNDLE_ID "com.salek.hightech"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Salek
    PLUGIN_CODE HiTc
    FORMATS VST3 Standalone
    PRODUCT_NAME "SALEK HIGHTECH"
    VST3_CATEGORIES "Instrument Synth"
)

target_sources(SALEK_HIGHTECH
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/DSP/Voice.cpp
        Source/DSP/Oscillator.cpp
        Source/DSP/Filter.cpp
        Source/DSP/Envelope.cpp
        Source/DSP/LFO.cpp
        Source/DSP/ModulationMatrix.cpp
        Source/UI/LookAndFeel_SALEK.cpp
)

target_compile_definitions(SALEK_HIGHTECH
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(SALEK_HIGHTECH
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_opengl
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

juce_generate_juce_header(SALEK_HIGHTECH)