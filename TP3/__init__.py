import threading
from flask import Flask, render_template,copy_current_request_context
from flask_socketio import SocketIO, emit
from .serial_port import USBport
from .timeStamp import toSeconds, toTimeStamp

def create_app(test_config=None):
    # create and configure the app
    app = Flask(__name__, instance_relative_config=True)
    socketio = SocketIO(app,cors_allowed_origins="*")

    @app.route("/tp3/dashboard",methods=(["GET"]))
    def dashboard():

        usb_port = USBport()       

        @copy_current_request_context
        def read_thread():
            while(usb_port.is_connected()):
                usb_port.readData();
                
        @copy_current_request_context    
        def send_time_thread():
            last_second = 0;
            while(usb_port.is_connected()):
                seconds = usb_port.getSeconds()
                if(seconds != None and last_second!=seconds):
                    last_second=seconds
                    timeStamp = toTimeStamp(seconds)
                    print(timeStamp)
                    socketio.emit('server_send_time',timeStamp,namespace="/tp3/dashboard")

        @copy_current_request_context    
        def send_data_thread():
            while(usb_port.is_connected()):
                while(usb_port.haveEvents()):
                    event = usb_port.getEvent()
                    socketio.emit('server_send_data',{
                        "time": toTimeStamp(event[0]),
                        "event":f'Interrupción del pin {event[1]}'
                        },namespace="/tp3/dashboard")
            
        
        threading.Thread(target=send_data_thread).start()
        threading.Thread(target=send_time_thread).start()
        threading.Thread(target=read_thread).start()

        @socketio.on('client_request_delete',namespace="/tp3/dashboard")
        def sendEvents():
            usb_port.sendData("~;")
        
        @socketio.on('client_request_data',namespace="/tp3/dashboard")
        def sendEvent():
            usb_port.sendData("?;")
        
        @socketio.on('client_update_time',namespace="/tp3/dashboard")
        def updateTime(request):
            usb_port.sendData("*;" + str(toSeconds(request["timeStamp"])))

        return render_template('dashboard/dashboard.html',
                            conn=usb_port.is_connected(),
                            )
    return app