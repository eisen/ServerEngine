const io = require('socket.io')();
const { v4: uuidv4, NIL } = require('uuid');
const yargs = require('yargs/yargs')
const { hideBin } = require('yargs/helpers');
const { exit } = require('yargs');

const argv = yargs(hideBin(process.argv)).argv

var sockets = {}
var clients = []
var games = []

var tournament_running = false

const timeout = 3 // seconds

const BLACK = 1
const WHITE = 2

const START_BOARD = [
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 2, 0, 0, 0,
    0, 0, 0, 2, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
]

var admin = NIL

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

function connection(socket)
{
    sockets[socket.id] = socket

    socket.on("set team", (data) => {
        var name = data["name"]
        console.log("Team name:", name)
        clients.push({
            "id": socket.id,
            "socket": sockets[socket.id],
            name: name,
            game_count: 0,
            win_count: 0,
            tie_count: 0
        })

        sockets[socket.id].on("move", move)
        sockets[socket.id].on("pass", pass)
    })

    socket.on("set admin", set_admin)
}

function start(type)
{
    console.log("Start tournament of type:", type)
    if(tournament_running == true)
    {
        console.log("Tournament running, can't start a new one")
        return
    }

    var data = {
        timeout: timeout
    }
    for(let client of clients)
    {
        client.socket.emit("set timeout", data)
    }

    if(type === "round robin")
    {
        tournament_running = true

        for(var idx = 0; idx < clients.length; ++idx )
        {
            for(var jdx = idx + 1; jdx < clients.length; ++jdx ) // Everybody plays everybody once
            {
                var game = {
                    id: uuidv4(),
                    client1: clients[idx],
                    client2: clients[jdx],
                    black: Math.random() < 0.5 ? clients[idx] : clients[jdx],
                    white: NIL, // Need to determine below
                    turn: BLACK,
                    board: START_BOARD,
                    ended: false
                }

                // Set Opponents
                var opponent = {
                    game_id: game.id,
                    name: game.client2.name
                }
                game.client1.socket.emit("set opponent", opponent)
                opponent.name = game.client1.name
                game.client2.socket.emit("set opponent", opponent )

                ++game.client1.game_count
                ++game.client2.game_count

                // Determine white player
                if(game.client1 === game.black) game.white = game.client2
                else game.white = game.client1

                games.push(game)
            }
        }

        for( let game of games)
        {
            // Kickoff each game
            var data = {
                game_id: game.id,
                board: ToString(game.board),
                turn: game.turn
            }
            game.black.socket.emit("make move", data)
        }
    }
    else if(type === "double elimination")
    {

    }
}

function get_game_idx(game_id)
{
    var idx = 0
    for(let game of games)
    {
        if(game.id === game_id) return idx
        ++idx
    }
    return -1
}

function move(data)
{
    var game_id = data["game_id"]
    var out_board = ToArray(data["board"])
    var game_idx = get_game_idx(game_id)
    games[game_idx].board = out_board;
    next_turn(games[game_idx])
}

function pass(data)
{
    var game_id = data["game_id"]
    var game_idx = get_game_idx(game_id)
    next_turn(games[game_idx])
}

function next_turn(game)
{
    if(game_ended(game) === false)
    {
        var data = {
            game_id: game.id,
            board: ToString(game.board),
            turn: game.turn
        }

        if(game.turn === BLACK)
        {
            game.turn = WHITE
            data.turn = game.turn
            game.white.socket.emit("make move", data)
        }
        else
        {
            game.turn = BLACK
            data.turn = game.turn
            game.black.socket.emit("make move", data)
        }
    }
    else
    {
        // Game ended
        calculate_scores(game)
    }
}

function game_ended(game)
{
    var empty_cells = 64;
    for( let cell of game.board)
    {
        if(cell === 0) break
        --empty_cells
    }

    if(empty_cells === 0)
    {
        game.ended = true
    }

    return game.ended
}

function calculate_scores(game)
{
    var data = {
        game_id: game.id,
        black_count: 0,
        white_count: 0
    }
    for(let cell of game.board)
    {
        if(cell === 1) ++data.black_count
        else if(cell === 2) ++data.white_count
    }

    game.client1.socket.emit("game ended", data)
    game.client2.socket.emit("game ended", data)

    if(data.black_count > data.white_count)
    {
        ++game.black.win_count
    }
    else if(data.white_count > data.black_count)
    {
        ++game.white.win_count
    }
    else
    {
        ++game.black.tie_count
        ++game.white.tie_count
    }

    tournament_ended()
}

function tournament_ended()
{
    var games_ended = 0
    for(let game of games)
    {
        if(game.ended === true)
        {
            ++games_ended
        }
    }

    if(games_ended === games.length) // Tournament complete
    {
        tournament_running = false
        console.log("Results: name | game count | win count | tie count")
        for( let client of clients)
        {
            console.log(client.name, client.game_count, client.win_count, client.tie_count)
            var data = {
                name: client.name,
                game_count: client.game_count,
                win_count: client.win_count,
                tie_count: client.tie_count
            }
            client.socket.emit("tournament ended", data)
        }
        setTimeout(() => {
            process.exit(0)
        }, 1000) // Quit after 1 second
    }
}

function set_admin(socket_id)
{
    console.log("Setting admin")
    var socket = sockets[socket_id]

    socket.on("start", start)
    socket.emit("ready")
}

io.on('connection', connection);

io.on("start", start)

console.log("Port: ", argv.port)
io.listen(argv.port);