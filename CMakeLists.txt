cmake_minimum_required(VERSION 3.15)

project(CloudVox VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE AND NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "14.4" CACHE STRING "Minimum macOS deployment version" FORCE)
endif()

# JUCE is expected to be available via FetchContent or as a submodule
include(FetchContent)

if(NOT DEFINED JUCE_SOURCE_DIR)
    FetchContent_Declare(
        juce
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 8.0.10
    )
    FetchContent_MakeAvailable(juce)
endif()

juce_add_plugin(CloudVox
    COMPANY_NAME "CloudVox"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE ClVx
    PLUGIN_CODE Clvx
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "CloudVox"
)

set(SOURCE_FILES
    Source/CloudVoxAudioProcessor.cpp
    Source/CloudVoxAudioProcessor.h
    Source/CloudVoxAudioProcessorEditor.cpp
    Source/CloudVoxAudioProcessorEditor.h
    Source/dsp/TiltEQ.cpp
    Source/dsp/TiltEQ.h
    Source/dsp/ModDelay.cpp
    Source/dsp/ModDelay.h
    Source/dsp/MicroPitch.cpp
    Source/dsp/MicroPitch.h
    Source/dsp/Shimmer.cpp
    Source/dsp/Shimmer.h
    Source/dsp/Saturator.cpp
    Source/dsp/Saturator.h
    Source/dsp/AutoGain.cpp
    Source/dsp/AutoGain.h
    Source/state/PresetManager.cpp
    Source/state/PresetManager.h
    Source/ui/Knob.h
    Source/ui/Meter.h
    Source/ui/PresetBar.h
)

target_sources(CloudVox PRIVATE ${SOURCE_FILES})

target_compile_definitions(CloudVox
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(CloudVox
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_extra
        juce::juce_audio_plugin_client
)
