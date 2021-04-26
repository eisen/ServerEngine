const io = require("socket.io-client")
const yargs = require('yargs/yargs')
const { hideBin } = require('yargs/helpers')

const argv = yargs(hideBin(process.argv)).argv

const URL = "http://localhost:8080";
const socket = io(URL, { reconnection: true});

var timeout = 1 // seconds

var games = {}

const name = argv.name

const EMPTY = 0

socket.on("set timeout", (in_timeout) => {
    timeout = in_timeout
    console.log("Timeout:", timeout)
})

socket.on("set opponent", (game_id, name) => {
    var game = {
        id: game_id,
        opponent: name
    }

    games[game_id] = game
})

socket.on("make move", (game_id, in_board, turn) => {
    out_board = in_board
    for(var idx = 0; idx < 64; ++idx)
    {
        if(in_board[idx] === EMPTY)
        {
            out_board[idx] = turn
            socket.emit("move", game_id, out_board)
            return
        }
    }

    // Board is full
    socket.emit("pass", game_id)
})

socket.on("game ended", (game_id, black_count, white_count) => {
    var game_name = name + " vs " + games[game_id].opponent
    console.log("Game ended:", game_name, black_count, white_count)
    delete games[game_id]
})

socket.on("tournament ended", (game_count, win_count, tie_count) => {
    console.log("Tournament results for:", name, game_count, win_count, tie_count)
})

socket.on('connect', function (sock) {
    socket.emit("set team", socket.id, name)
})

socket.on("disconnect", () => {
    process.exit()
})