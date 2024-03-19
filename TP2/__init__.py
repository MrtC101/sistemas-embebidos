import threading
from flask import Flask, render_template,copy_current_request_context
from flask_socketio import SocketIO, emit
from .serial_port import USBport

def create_app(test_config=None):
    # create and configure the app
    app = Flask(__name__, instance_relative_config=True)
    socketio = SocketIO(app,cors_allowed_origins="*")

    @app.route("/tp2/dashboard",methods=(["GET"]))
    def dashboard():

        usb_port = USBport()       

        @copy_current_request_context
        def read_thread():
            while(usb_port.is_connected()):
                usb_port.readData();
                
        @copy_current_request_context    
        def send_thread():
            while(usb_port.is_connected()):
                (mesure, alarm) = usb_port.queue_get()
                print(mesure,alarm)
                if(mesure!=''):
                    socketio.emit('server_send_mesure',{"mesure":mesure,"alert":alarm},namespace="/tp2/dashboard")

        threading.Thread(target=send_thread).start()
        threading.Thread(target=read_thread).start()

        @socketio.on('client_send_params',namespace="/tp2/dashboard")
        def read_params(request):
            str_params = "";
            str_params += str(request["params"])
            usb_port.sendData(str_params)

        return render_template('dashboard/dashboard.html',
                            conn=usb_port.is_connected(),
                            )
    return app