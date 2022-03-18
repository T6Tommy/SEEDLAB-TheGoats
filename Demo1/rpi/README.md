# SEED Lab Group 7 - The Goats

# Demo 1

## EENG 350 B, Spring 2022

### Raspberry Pi: Python Code

In this folder, "rpi", there are two sets of Python code used on the Raspberry Pi.

The first code is labeled, "Demo1.py". This Python code is the marker detection scheme used with the Pi Camera to detect the 1" blue tape marker.
This Python code also detects the horizontal angle of the 1" blue tape marker to the camera.

The second code is labeled, "i2c.py". This Python code takes in variables sent by the "MP1.py" to determine the quandrant the marker is in on the camera.
It interprets this data to send to the Arduino for the position or angle the wheel needs to be in.
