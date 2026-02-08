cmake_minimum_required(VERSION 3.15)

project(CodexPianoVST3 VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.8
)

FetchContent_MakeAvailable(JUCE)

juce_add_plugin(CodexPianoVST3
  COMPANY_NAME "Codex"
  IS_SYNTH TRUE
  NEEDS_MIDI_INPUT TRUE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS FALSE
  COPY_PLUGIN_AFTER_BUILD TRUE
  PLUGIN_MANUFACTURER_CODE CdxA
  PLUGIN_CODE Cdp1
  FORMATS VST3 Standalone
  PRODUCT_NAME "Codex Piano VST3"
)

juce_generate_juce_header(CodexPianoVST3)

juce_add_binary_data(CodexPianoAssets
  SOURCES
    Assets/placeholder.txt
)

target_sources(CodexPianoVST3
  PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)

target_compile_definitions(CodexPianoVST3
  PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(CodexPianoVST3
  PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_audio_processors
    CodexPianoAssets
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
