## Requirements:
OpenCV
ViGEm
Python 3.5 /w numpy
Tensorflow
Windows 7 (tested)
Visual Studio 2017
CMake 3.10

## Build instructions:
place ViGEmUM.dll in bin/Release and bin/Debug
run build.cmd
compile resulting visual studio project

## Goal:
Create AI for the game Mortal Kombat X that adapts to the opponent playstyle.
AI will take frame data from the game and respond by simulating game controller output.
Neural network is used to detect state from frame data and then MCTS algorithm is run
for the given state to predict outputs.


## Current state:
Game controller simulation - working
Frame capture and processing - working
Neural network training for detecting game states - TODO
States and Monte-Carlo Tree Search pre-training - TODO
