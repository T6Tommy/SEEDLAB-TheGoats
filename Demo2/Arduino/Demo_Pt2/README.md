This is the code for the second part of Demo 2.

The Arduino can be in one of 5 states:
* Seek: The arduino turns the robot counter-clockwise until it recieves a message that a marker has been detected
* Move right/left: When the marker is in view, the arduino turns the robot counter-clockwise (if the marker is on the left side of the camera's view) or clockwise (if the marker is on the right side of the camera's view) until the marker is centered in the camera's view.
* Foreward: When the marker is centered, the robot moves forward until it recieves a signal from the Pi that the tape has reached the bottom of the camera's view
* Follow: Then, the robot continues to follow the tape until it recieves a signal indicating that the tape has left the camera's view.
* Halt: At this point, the robot stops at the end of the tape.
