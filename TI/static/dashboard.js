url = "http://127.0.0.1:5000/ti/dashboard"

var socket = io(url);

socket.on('server_send_time',
    (time) =>{
        console.log(time)
        document.getElementById("time").innerText = time;
    }
);

function contar(num){
    if(num===0) return 1;
    return Math.floor(Math.log10(Math.abs(num)))+1;
}

function verifyContent(input){
    // Obtener el valor del input que disparó el evento
    verified = document.getElementById("year_input").checkValidity()
    && document.getElementById("month_input").checkValidity()
    && document.getElementById("day_input").checkValidity()
    && document.getElementById("hour_input").checkValidity()
    && document.getElementById("minute_input").checkValidity()
    && document.getElementById("second_input").checkValidity()
    //console.log(verified)
    document.getElementById("update_button").disabled=!verified
}
function form(value){
    if(!value){
        str = "00"    
    }else{
        str = value.toString()
        if(value < 10){
            str = str.padStart(2,'0')
        }
    }
    str.trimEnd("\n");
    str.trimEnd("\r");
    return str
}
function updateTime(event){
    year = document.getElementById("year_input").value 
    month = document.getElementById("month_input").value
    day = document.getElementById("day_input").value
    hour = document.getElementById("hour_input").value
    minute = document.getElementById("minute_input").value
    second = document.getElementById("second_input").value
    //console.log(`${form(year)}/${form(!month?1:month)}/${form(!day?1:day)}-${form(hour)}:${form(minute)}:${form(second)}`)
    socket.emit('client_update_time',{
        "timeStamp":`${form(year)}/${form(!month?1:month)}/${form(!day?1:day)}-${form(hour)}:${form(minute)}:${form(second)}`
    });
}

function requestData(){
    var tabla = document.getElementById("event-table").getElementsByTagName('tbody')[0];
    while (tabla.rows.length > 0) {
        tabla.deleteRow(0);
    }
    socket.emit('client_request_data')
}

function deleteData(){
    socket.emit('client_request_delete')
}

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

//TP1 y TP2

function sendParameters(){
    socket.emit('client_send_params',
                {
                    params: [
                        Number(document.getElementById("pin13").checked),
                        document.getElementById("pin9").value,
                        document.getElementById("pin10").value,
                        document.getElementById("pin11").value,
                        Number(document.getElementById("onof").checked),                                     
                    ]
                }
            )
}


socket.on('server_send_mesure',
    (mesure) =>{
        mesure = response.mesure
        document.getElementById("mesure").innerText = mesure;
        // Mapear el valor de entrada (0-1024) al rango de luminosidad HSL (0-100)
        var lightness = (1024.55 - mesure*0.55) * (100/1024);
        // Convertir el color de HSL a RGB
        var color = `hsl(56, 100%, ${lightness}%)`;
        document.getElementById("mesure").style.backgroundColor = color;
        
        alarm = response.alert
        console.log(alarm)
        const icon = document.querySelector(".alarm-icon")
        if(alarm=="on"){
            icon.classList.add("on")
        }else{
            icon.classList.remove("on")
        }
    }
);