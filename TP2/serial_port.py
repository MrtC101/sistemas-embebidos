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
        frame = self.serial_port.readline().decode("utf-8")
        if frame != '':
            values = frame.split(";")
            alarm = "off"
            #print(values[1])
            if values[1].find('!') != -1:
                alarm = "on"
            self.queue.put((int(values[0]),alarm))
        return True
    
    def queue_get(self):
        value = self.queue.get()
        return value

    def is_connected(self):
        #print("puerto: "+str(self.serial_port.is_open))
        return self.serial_port.is_open
    
