# i2c_float.ino

A simple Arduino sketch intended to test I2C functionality, particularly the transfer of floating point values.
The sketch takes 6 bytes from an I2C controller and interprets them from first to last as follows:
- Byte 1: peripheral address (expects 0x20)
- Byte 2: register address (0x01 modifies the distance variable, while 0x02 moidifies the rotation variable)
- Bytes 3-6: 32-bit floating point magnitude

The resulting variable is then printed onto the Serial console.
