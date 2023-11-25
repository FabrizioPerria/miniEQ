# 3-Band EQ

## Disclaimer
This plugin is inspired by Matkat's [free tutorial](https://www.youtube.com/watch?v=ZKmFZpJEZ3k&t=974s).

## Components:
 - lowcut EQ with 4 different slope levels
 - peak EQ with gain and width selector
 - lowcut EQ with 4 different slope levels
 - frequency response curve
 - spectrum analyzer of the audio currently played
 - bypass buttons for each component

## Development
The plugin is entirely developed in JUCE framework and can be used as sandbox for further experimentation.
I organized the classes differently than what Chuck did so that the code is more decoupled and allows sandboxing.

## Future steps
At some point i will probably try to render the frequency response and the spectrum analyzer using openGL.


