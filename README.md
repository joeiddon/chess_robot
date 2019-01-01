# Source for a full *chess-playing* robot.

*Note that this project is not finished.*

File/Directory|Purpose
----|---
[sketch/sketch.ino](sketch.ino)|Sketch for the Aruino Nano that controls the arm.
[vision.py](vision.py)|A (partial) computer vision library using `numpy` - only minimal functions.
[recognition.py](recognition.py)|Using `vision.py`, detects the locations of the pieces and identies their colours.
[tkinter_controller.py](tkinter_controller.py)|A GUI written with TKinter for controlling the robot arm.
[intelligence/](intelligence)|An AI written in C for finding the best chess move using [`negamax`](https://en.wikipedia.org/wiki/Negamax).
[model/](model)|Contains `.stl` and `.scad` designs for the 3d printed arm.

See some videos of the arm in progress here:

https://photos.app.goo.gl/uKwEp8JHDU8QnPrb8
