
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

socket.io.on('server_send_mesure',
    (mesure) =>{
        document.getElementById("mesure").innerText = mesure;
        // Mapear el valor de entrada (0-1024) al rango de luminosidad HSL (0-100)
        var lightness = (Number(mesure) / 1024) * 100;
        // Mapear el valor de entrada al rango de tonalidad HSL (60-120)
        var hue = (Number(mesure) / 1024) * 60 + 60;
        // Convertir el color de HSL a RGB
        var color = `hsl(${hue}, 100%, ${lightness}%)`;
        document.getElementById("mesure-card").style.backgroundColor = color;
    }
);