import serial, os.path

"""
==Joe's Serial Protocol==
  First byte is message type; last byte is a full terminater byte (0xff).
  The number of enclosed bytes (NEBs) is dependent on the message type.
  First byte  | Message (type)  | NEBs | Enclosed bytes meaning
  0           | home            | 0    | N/A
  1           | motor state     | 1    | (0/1 => disable/enable)
  2           | set grabber     | 1    | (0/1 => grab off/grab on
  3           | move position   | 6    | (x: [0¬340, y: [100¬410], z: [0¬200])
                                          ^          ^             ^
                         these uint16s are sent *little-endian* (see INT_FROM_BYTES)
  Will return a standard success code: 0/1 indicating success/failure, respectively.
"""

LIMITS = {'x': (0, 340),
          'y': (100, 410),
          'z': (0, 200)}

class Arm():
    '''This class implements the necessary methods to communicate over serial
    with an Arduino that is running the corresponding "Joe's Serial Protocol"
    C++ code on.'''
    def __init__(self, port):
        '''Initialises an arm instance.
            - port <= serial port string
        '''
        self.port = port
        self._connect_to_arduino()
    def home(self):
        '''Moves the arm to its home position which is
        defined on the Arduino's side.'''
        self.serial.write([0, 0xff])
    def set_motors(self, state):
        '''Enables or disables the stepper and servo motors on the arm.
            - state <= boolean: True=enable; False=disable'''
        self.serial.write([1, state, 0xff])
    def set_grabber(self, state):
        '''Sets the state of the grabber on the arm's effector.
            - state <= boolean: True=grab on; False=grab off'''
        self.serial.write([2, state, 0xff])
    def move_to(self, x, y, z):
        '''Moves the arm to the specified coordinate; given in millimeters.'''
        if not (LIMITS['x'][0] <= x <= LIMITS['x'][1] and \
                LIMITS['y'][0] <= y <= LIMITS['y'][1] and \
                LIMITS['z'][0] <= z <= LIMITS['z'][1]):
            raise ValueError(f'Position [{x},{y},{z}] out of limits.')
        self.serial.write([3,
                           *self._bytes_from_int(x),
                           *self._bytes_from_int(y),
                           *self._bytes_from_int(z),
                           0xff])
    def _send_joes_protocol(self, message):
        self.serial.read(self.serial.is_waiting)
        self.serial.write(message)
        if self.serial.is_waiting != 1:
            raise Exception(f'Arduino replied with more than one byte when sending: {message}.')
        if self.serial.read()[0] != 0:
            raise Exception(f'Arduino status code non-zero wwhen sending: {message}.')
    def _connect_to_arduino(self):
        try:
            self.serial = serial.Serial(self.port)
        except serial.serialutil.SerialException:
            raise Exception(f'Could not open [{port}].')
    def _bytes_from_int(self, i):
        return (i >> 8, i & 0xff)
