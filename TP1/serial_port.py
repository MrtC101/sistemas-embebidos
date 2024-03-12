import serial
from queue import Queue

class USBport:
    serial_port : serial.Serial;
    queue : Queue = Queue();

    def __init__(self):
        self.serial_port = serial.Serial(
            port='/dev/ttyACM0',
            baudrate=9600,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=1,
            write_timeout=10,
            dsrdtr=False,
            inter_byte_timeout=None,
            exclusive=None
            )
        self.serial_port.reset_input_buffer()
        self.serial_port.reset_output_buffer()

    def sendData(self,data : str) -> bool:
        self.serial_port.write(data.encode("utf-8"))

    def readData(self) -> bool:
        value = self.serial_port.readline().decode("utf-8")
        #print(value)
        if value != '':
            self.queue.put(int(value))
        return True
    
    def queue_get(self):
        try:
            value = self.queue.get()
            return value
        except:
            return None

    def is_connected(self):
        #print("puerto: "+str(self.serial_port.is_open))
        return self.serial_port.is_open
    
