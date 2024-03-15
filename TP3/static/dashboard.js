
var socket = io("http://127.0.0.1:5000/tp1/dashboard");


function updateTime(){
    //Build TimeStump
    timeStamp = ""
    document.getElementById();
    document.getElementById();
    document.getElementById();
    document.getElementById();
    socket.emit('client_update_time',{
        "timeStamp":timeStamp
    });
}

function requestData(){
    socket.emit('client_request_data')
}
socket.on('server_send_time',
    (time) =>{
        document.getElementById("time").innerText = time;
    }
);

socket.on('server_send_data',
    (event_data) =>{
        var tabla = document.getElementById("event-table").getElementsByTagName('tbody')[0];

        var fila = tabla.insertRow();
    
        var timeCell = fila.insertCell(0);
        var eventCell = fila.insertCell(1);
    
        timeCell.textContent = event_data.time;
        eventCell.textContent = event_data.event;
    }
);