from . import socketio
from flask_socketio import emit
from .backend.serial_port import usb_port

@socketio.on('client_send_params',namespace="/dashboard")
def read_params(params):
    str_params = "";
    for i in range(0,len(params)):
        str_params += str(params[i])
        if( len(params)-1 > i ):
            str_params += ";"
    usb_port.sendData(str_params)