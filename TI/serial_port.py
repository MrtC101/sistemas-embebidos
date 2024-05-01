import serial
from queue import Queue

class USBport:
    serial_port : serial.Serial;
    eventQueue : Queue = Queue();
    currSeconds : int = None;
    currMesure: tuple = (0,'-');

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
        #print("sent:",data,data.encode("utf-8"))
        self.serial_port.write(data.encode("utf-8"))

    def readData(self) -> bool:
        frame = self.serial_port.readline().decode("utf-8")
        if frame != '':
            frame = frame.strip('\n').strip('\r')
            #print("returned:",frame)
            subStr = frame.split(";")
            if(subStr[0]=='t'):
                self.currSeconds = int(subStr[1])
            elif(subStr[0]=='e'):
                self.eventQueue.put((int(subStr[1]),subStr[2]))
            elif(subStr[0]=='l'):
                values = frame.split(";")
                alarm = "off"
                if values[2].find('!') != -1:
                    alarm = "on"
                self.currMesure = (int(values[1]),alarm)
        return True
    
    def getEvent(self):
        return self.eventQueue.get()
    
    def haveEvents(self):
        return (not self.eventQueue.empty())
    
    def getSeconds(self):
        return self.currSeconds

    def getMesure(self):
        return self.currMesure

    def is_connected(self):
        #print("puerto: "+str(self.serial_port.is_open))
        return self.serial_port.is_open
    
