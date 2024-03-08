import serial
from .read_thread import ReadThread
from .send_thread import SendThread
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
        ReadThread(self.serial_port,self.queue)
        SendThread(self.serial_port)

    def sendData(self,data : str):
        self.serial_port.write(data.encode("utf-8"))

    def readData(self) -> int:
        return int(self.queue.get())
    
    def is_connected(self):
        #print("puerto: "+str(self.serial_port.is_open))
        return self.serial_port.is_open
    
usb_port = USBport()
