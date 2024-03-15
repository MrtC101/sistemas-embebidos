
var socket = io("http://127.0.0.1:5000/tp1/dashboard");

function sendParameters(){
    socket.emit('client_send_params',
                {
                    params: [
                        Number(document.getElementById("pin13").checked),
                        document.getElementById("pin9").value,
                        document.getElementById("pin10").value,
                        document.getElementById("pin11").value                                        
                    ]
                }
            )
}


socket.on('server_send_mesure',
    (mesure) =>{
        document.getElementById("mesure").innerText = mesure;
        // Mapear el valor de entrada (0-1024) al rango de luminosidad HSL (0-100)
        var lightness = (1024.55 - mesure*0.55) * (100/1024);
        // Convertir el color de HSL a RGB
        var color = `hsl(56, 100%, ${lightness}%)`;
        document.getElementById("mesure").style.backgroundColor = color;
    }
);