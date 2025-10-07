# CloudVox

CloudVox is a JUCE-based audio plugin inspired by airy, shimmery vocal textures. The initial scaffolding in this repository sets up the core DSP building blocks, parameter layout, and user interface for a "cloud vocal finisher" style effect.

## Features

- Macro-focused control surface with Intensity, Air/Dark, Glue, Width, Detune, Tightness, Saturate, Input, Output, and Auto-Gain parameters.
- Humanised doubler built from modulated micro-pitch delay lines.
- Width control based on mid/side processing with guard rails to keep signals safe.
- Shimmer reverb block combining a plate-style reverb with an octave-up feedback path.
- Soft saturation stage with pre-emphasis and air-enhancing post filtering.
- Auto-gain helper to maintain consistent loudness while tweaking.
- Preset manager preloaded with Bladee and pluko inspired day-one patches.

## Building

This project uses CMake and fetches JUCE via `FetchContent`. To build the plugin, clone the repository and run:

```bash
cmake -B build
cmake --build build
```

The default configuration builds VST3, AU, and Standalone targets. Adjust the `juce_add_plugin` call in `CMakeLists.txt` to customise the formats for your toolchain.

## Next Steps

The DSP scaffolding is deliberately ear-friendly and ready for refinement. Suggested follow-ups include:

- Fine-tuning the doubler timing offsets and shimmer balance by ear.
- Adding live meters and parameter tooltips to the UI.
- Exposing an "Advanced" view for deeper per-block controls.
- Writing automated JUCE unit tests for each DSP block.

Have fun sculpting your own cloudy, wide vocal soundscapes!
