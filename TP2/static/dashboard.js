
var socket = io("http://127.0.0.1:5000/tp2/dashboard");

function sendParameters(){
    socket.emit('client_send_params',{params:1});
}

socket.on('server_send_mesure',
    (response) =>{
        mesure = response.mesure
        document.getElementById("mesure").innerText = mesure;
        // Mapear el valor de entrada (0-1024) al rango de luminosidad HSL (0-100)
        var lightness = (1024.55 - mesure*0.55) * (100/1024);
        // Convertir el color de HSL a RGB
        var color = `hsl(56, 100%, ${lightness}%)`;
        document.getElementById("mesure").style.backgroundColor = color;
        
        alarm = response.alert
        //console.log(alarm)
        const icon = document.querySelector(".alarm-icon")
        if(alarm=="on"){
            icon.classList.add("on")
        }else{
            icon.classList.remove("on")
        }
    }
);