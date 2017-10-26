# Source for a full *chess-playing* robot


|File                             | Purpose                                                                                             |
|---------------------------------|-----------------------------------------------------------------------------------------------------|
|intelligence.py                  |An AI for finding best move using [`negamax`](https://en.wikipedia.org/wiki/Negamax)                 |
|vision.py                        |A (partial) computer vision library using `numpy` - only minimal functions                           |
|recognition.py                   |Using `vision.py`, detects the locations of the pieces and identies their colours                    |
|controller.py                    |A GUI written with TKinter for controlling the robot arm                                             |
|sketch.ino                       |Sketch for the Aruino Nano that controls the arm                                                     |
