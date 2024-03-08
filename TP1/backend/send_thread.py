from threading import Thread
from flask_socketio import emit

class SendThread(Thread):
    
    def __init__(self,serial):
        self.usb = serial

    def run(self):
        while(usb_port.is_connected()):
            send_mesure(self)   

    def send_mesure(self):
        mesure = self.usb.readData()
        emit('server_send_mesure',mesure)