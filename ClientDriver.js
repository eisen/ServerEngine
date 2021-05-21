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

socket.on("set timeout", (data) => {
    timeout = data["timeout"]
    console.log("Timeout: ", timeout, "seconds")
})

socket.on("set opponent", (data) => {
    var game = {
        id: data["game_id"],
        opponent: data["name"]
    }

    games[game.id] = game
})

function ToString(arrayBoard)
{
    var strBoard = ""
    for(let cell of arrayBoard)
    {
        strBoard += cell.toString()
    }
    return strBoard
}

function ToArray(strBoard)
{
    let arrayBoard = [
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    ]

    for (var idx = 0; idx < strBoard.length; idx++)
    {
        arrayBoard[idx] = parseInt(strBoard.charAt(idx))
    }

    return arrayBoard
}

socket.on("make move", (data) => {
    var game_id = data["game_id"]
    var in_board = ToArray(data["board"])
    var turn = data["turn"]
    out_board = in_board
    setTimeout( () => {
        for(var idx = 0; idx < 64; ++idx)
        {
            if(in_board[idx] === EMPTY)
            {
                out_board[idx] = turn
                var outData = {
                    "game_id": game_id,
                    "board": ToString(out_board)
                }
                socket.emit("move", outData)
                return
            }
        }

        // Board is full
        socket.emit("pass", data)
    }, timeout * 1000) // Timeout in seconds
})

socket.on("game ended", (data) => {
    var game_id = data["game_id"]
    var black_count = data["black_count"]
    var white_count = data["white_count"]
    var game_name = name + " vs " + games[game_id].opponent
    console.log("Game ended:", game_name, black_count, white_count)
    delete games[game_id]
})

socket.on("tournament ended", (data) => {
    var name = data["name"]
    var game_count = data["game_count"]
    var win_count = data["win_count"]
    var tie_count = data["tie_count"]
    console.log("Tournament results for:", name, game_count, win_count, tie_count)
})

socket.on('connect', function () {
    var data = {
        socket_id: socket.id,
        name: name
    }
    socket.emit("set team", data)
})

socket.on("disconnect", () => {
    process.exit()
})