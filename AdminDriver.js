const io = require("socket.io-client")
const yargs = require('yargs/yargs')
const { hideBin } = require('yargs/helpers');
const { exit } = require('yargs');

const argv = yargs(hideBin(process.argv)).argv

const URL = "http://engine.rmd-hackathon.xyz:8080";
const socket = io(URL, { reconnection: true});

socket.on("ready", () => {
    if(argv.rr === true)
    {
        console.log("Start round robin tournament")
        socket.emit("start", {
            type: "round robin"
        })
    }
    else if(argv.de === true)
    {
        console.log("Start double elimination tournament")
        socket.emit("start", {
            type: "double elimination", 
            seeds: [
                {
                    player1: 'eisen1',
                    player2: 'by'
                },
                {
                    player1: 'eisen2',
                    player2: 'by'
                },
                {
                    player1: 'eisen3',
                    player2: 'eisen4'
                }
            ]
        })
    }
})

socket.on('connect', function (sock) {
    console.log('Admin connection accepted!')
    socket.emit("set admin", socket.id)
})

socket.on("disconnect", () => {
    process.exit()
})