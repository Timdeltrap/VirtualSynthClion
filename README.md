# VirtualSynth
The idea of this project is to use an arduino as a controller in an Samson Graphite 49 to transform it into a synthesizer instead of a midi controller.
While it's hard to create a digital synthesizer the first objective has been formed into: use an arduino as the MIDI controller in the Samson graphite 49 MIDI controller.

In the folder src you will find the file main.cpp which is the code to use your arduino as a MIDI controller. As soon as this code is   complete to control a Samson graphite 49 MIDI controller, the code will be made more dynamic and builtin in a GUI in which you can assemble a MIDI controller. This program also substitutes the programs needed on your PC to transform Arduino serial messages into MIDI messages and to simulate virtual midi ports.

TODO:
  MIDIFControllerFirmware
    [-Analog controls (sliders, mod- and pitchwheel) support.]
    -Button interaction
  GUI
    -Still needs to be created
