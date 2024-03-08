from threading import Thread
from queue import Queue

class ReadThread(Thread):
    queue : Queue;

    def __init__(self,serial ,queue : Queue):
        self.usb = serial
        self.queue = queue

    def run(self):
        while(self.usb.is_connected()):
            value = self.usb.readline().decode("urf-8")
            self.queue.put(value)