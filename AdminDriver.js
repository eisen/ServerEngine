const io = require("socket.io-client")

const URL = "http://localhost:8080";
const socket = io(URL, { reconnection: true});

socket.on("ready", () => {
    console.log("Start tournament")
    socket.emit("start", "round robin")
})

socket.on('connect', function (sock) {
    console.log('Admin connected!')
    socket.emit("set admin", socket.id)
})

socket.on("disconnect", () => {
    process.exit()
})