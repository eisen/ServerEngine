const { v4: uuidv4, NIL } = require('uuid');
const yargs = require('yargs/yargs')
const { hideBin } = require('yargs/helpers');
const { exit } = require('yargs');

const argv = yargs(hideBin(process.argv)).argv

const io = require('socket.io')({
    allowEIO3: true,
    cors: {
        credentials: true, // This is important.
        origin: (origin, callback) => {
            return callback(null, true)
        }
    }
});

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

const validIPs = [
    "54.67.123.203",
    "18.144.50.111",
    "54.177.207.33",
    "13.56.12.96",
    "54.67.47.117",
    "52.53.130.123",
    "54.215.180.67",
    "18.144.56.8",
    "54.219.97.244",
    "54.67.92.131",
    "54.183.27.25",
    "18.144.15.199",
    "3.101.80.163",
    "54.153.106.236",
    "52.53.230.44",
]

var admin = NIL

let connected = 0

function ToString(arrayBoard) {
    var strBoard = ""
    for (let cell of arrayBoard) {
        strBoard += cell.toString()
    }
    return strBoard
}

function ToArray(strBoard) {
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

    for (var idx = 0; idx < strBoard.length; idx++) {
        arrayBoard[idx] = parseInt(strBoard.charAt(idx))
    }

    return arrayBoard
}

function connection(socket) {
    let connection = socket.request.connection._peername
    let ip = connection.address.split(":")[3];
    console.log('Connection from:', ip);

    if (tournament_running || validIPs.includes(ip) === false) {
        socket.disconnect(true)
        return
    }
    
    sockets[socket.id] = socket

    socket.on("set team", (data) => {
        var name = data["name"]
        if (name == "") {
            socket.disconnect(true)
            return
        }
        console.log("Team name:", name)
        clients.push({
            "id": socket.id,
            "socket": sockets[socket.id],
            name: name,
            game_count: 0,
            win_count: 0,
            lost_count: 0,
            tie_count: 0,
            busy: false,
            disconnected: false
        })

        sockets[socket.id].on("move", move)
        sockets[socket.id].on("pass", pass)
        sockets[socket.id].on("disconnecting", () => {
            idx = clients.findIndex((client) => {
                return client.id === socket.id
            })
            console.log(clients[idx].name + " disconnected!")
            if (tournament_running) {
                clients[idx].disconnected = true

                games.find((game) => {
                    if (game.black.id === socket.id || game.white.id === socket.id) {
                        if (game.black.id === socket.id) {
                            game.board = ToArray("2000200020002000200020002000200020002000200020002000200020002000")
                        }
                        else if (game.white.id === socket.id) {
                            game.board = ToArray("1000100010001000100010001000100010001000100010001000100010001000")
                        }
                        game.ended = true
                    }
                })
            } else {
                clients.splice(idx, 1);
            }
        })
    })

    socket.on("set admin", set_admin)
}

function start(data) {
    if (tournament_running == true) {
        console.log("Tournament running, can't start a new one")
        return
    }
    
    var dataOut = {
        timeout: timeout
    }
    for (let client of clients) {
        client.socket.emit("set timeout", dataOut)
    }

    const type = data['type']
    console.log("Start tournament of type:", type)

    if (type === "round robin") {
        tournament_running = true

        for (var idx = 0; idx < clients.length; ++idx) {
            if (clients[idx].disconnected) continue
            for (var jdx = idx + 1; jdx < clients.length; ++jdx) // Everybody plays everybody once
            {
                if (clients[jdx].disconnected) continue
                var game = {
                    id: uuidv4(),
                    client1: clients[idx],
                    client2: clients[jdx],
                    black: Math.random() < 0.5 ? clients[idx] : clients[jdx],
                    white: NIL, // Need to determine below
                    turn: BLACK,
                    board: START_BOARD,
                    pass_count: 0,
                    ended: false,
                    pending: true,
                    watchdog: -1
                }

                // Set Opponents
                var opponent = {
                    game_id: game.id,
                    name: game.client2.name
                }
                game.client1.socket.emit("set opponent", opponent)
                opponent.name = game.client1.name
                game.client2.socket.emit("set opponent", opponent)

                ++game.client1.game_count
                ++game.client2.game_count

                // Determine white player
                if (game.client1 === game.black) game.white = game.client2
                else game.white = game.client1

                games.push(game)
            }
        }

        launch_games()
    }
    else if (type === "double elimination") {
        tournament_running = true
        const seeds = data['seeds']

        games = []
        for (var idx = 0; idx < seeds.length; ++idx) {

            const player1 = clients.find((client) => {
                return client.name === seeds[idx].player1
            })

            const player2 = clients.find((client) => {
                return client.name === seeds[idx].player2
            })

            if ( typeof player1 === "undefined") {
                // This is a by or team foerfeit
                player2.win_count++
                continue
            }

            if ( typeof player2 === "undefined") {
                // This is a by or team foerfeit
                player1.win_count++
                continue
            }

            var game = {
                id: uuidv4(),
                client1: player1,
                client2: player2,
                black: Math.random() < 0.5 ? player1 : player2,
                white: NIL, // Need to determine below
                turn: BLACK,
                board: START_BOARD,
                pass_count: 0,
                ended: false,
                pending: true,
                watchdog: -1
            }

            // Set Opponents
            var opponent = {
                game_id: game.id,
                name: game.client2.name
            }
            game.client1.socket.emit("set opponent", opponent)
            opponent.name = game.client1.name
            game.client2.socket.emit("set opponent", opponent)

            ++game.client1.game_count
            ++game.client2.game_count

            // Determine white player
            if (game.client1 === game.black) game.white = game.client2
            else game.white = game.client1

            games.push(game)
        }

        launch_games()
    }
}

function launch_games() {
    for (let game of games) {
        if (game.ended === false) {
            if (game.black.busy === false &&
                game.white.busy === false) {
                game.pending = false
                game.black.busy = true
                game.white.busy = true
                // Kickoff each game
                var data = {
                    game_id: game.id,
                    board: ToString(game.board),
                    turn: game.turn
                }
                game.black.socket.emit("make move", data)

                start_watchdog(game)
            }
        }
    }
}

function start_watchdog(game) {
    game.watchdog = setTimeout(() => {
        game.ended = true
        calculate_scores(game)
    }, 10 * 60 * 1000)
}

function get_game_idx(game_id) {
    var idx = 0
    for (let game of games) {
        if (game.id === game_id) return idx
        ++idx
    }
    return -1
}

function move(data) {
    var game_id = data["game_id"]
    var game_idx = get_game_idx(game_id)

    var out_board = ToArray(data["board"])

    games[game_idx].pass_count = 0
    games[game_idx].board = out_board;
    clearTimeout(games[game_idx].watchdog)
    next_turn(games[game_idx])
}

function pass(data) {
    var game_id = data["game_id"]
    var game_idx = get_game_idx(game_id)

    games[game_idx].pass_count += 1
    clearTimeout(games[game_idx].watchdog)
    if (games[game_idx].pass_count < 2) {
        next_turn(games[game_idx])
    }
    else {
        // Game is a tie or no valid moves left, finish game
        games[game_idx].ended = true
        calculate_scores(games[game_idx])
    }
}

function next_turn(game) {
    if (game_ended(game) === false) {
        var data = {
            game_id: game.id,
            board: ToString(game.board),
            turn: game.turn
        }

        if (game.turn === BLACK) {
            game.turn = WHITE
            data.turn = game.turn
            game.white.socket.emit("make move", data)
        }
        else {
            game.turn = BLACK
            data.turn = game.turn
            game.black.socket.emit("make move", data)
        }
        start_watchdog(game)
    }
    else {
        // Game ended
        calculate_scores(game)
    }
}

function game_ended(game) {
    var empty_cells = 64;
    for (let cell of game.board) {
        if (cell === 0) break
        --empty_cells
    }

    if (empty_cells === 0) {
        game.ended = true
    }

    return game.ended
}

function calculate_scores(game) {
    var data = {
        game_id: game.id,
        black_count: 0,
        white_count: 0
    }

    for (let cell of game.board) {
        if (cell === 1) ++data.black_count
        else if (cell === 2) ++data.white_count
    }

    game.client1.socket.emit("game ended", data)
    game.client2.socket.emit("game ended", data)

    if (data.black_count > data.white_count) {
        ++game.black.win_count
        ++game.white.lost_count
    }
    else if (data.white_count > data.black_count) {
        ++game.white.win_count
        ++game.black.lost_count
    }
    else {
        ++game.black.tie_count
        ++game.white.tie_count
    }

    game.black.busy = false
    game.white.busy = false
    launch_games()

    tournament_ended()
}

function tournament_ended() {
    var games_ended = 0
    for (let game of games) {
        if (game.ended === true) {
            ++games_ended
        }
    }

    if (games_ended === games.length) // Tournament complete
    {
        tournament_running = false
        console.log("Results: name | game count: | win count | loss count | tie count ")
        for (let client of clients) {
            console.log(client.name, client.game_count, ":  ", client.win_count, client.lost_count, client.tie_count)
            var data = {
                name: client.name,
                game_count: client.game_count,
                win_count: client.win_count,
                tie_count: client.tie_count
            }
            client.socket.emit("tournament ended", data)
        }
        console.log("Viewers: connected")
    }
}

function set_admin(socket_id) {
    console.log("Setting admin")
    var socket = sockets[socket_id]

    socket.on("start", start)
    socket.emit("ready")
}

io.on('connection', connection)

io.on("start", start)


// Viewers namespace
const viewersNamespace = io.of("/viewers")

function sendGameStatus(socket, id) {
    let idx = get_game_idx(id)
    if (typeof games[idx] != 'undefined') {
        socket.emit("game-board", {
            id: games[idx].id,
            board: ToString(games[idx].board),
            blackName: games[idx].black.name,
            whiteName: games[idx].white.name
        })
    }
}

viewersNamespace.on("connection", (socket) => {
    let intervalId = -1
    connected++

    socket.on("game-status", (id) => {
        clearInterval(intervalId)

        sendGameStatus(socket, id)

        intervalId = setInterval(() => {
            sendGameStatus(socket, id)
        }, timeout * 1000)
    })
});

setInterval(() => {
    let games_data = []
    for (let game of games) {
        // Kickoff each game
        var game_data = {
            id: game.id,
            black: game.black.name,
            white: game.white.name,
            board: ToString(game.board),
            ended: game.ended,
            pending: game.pending,
            disconnected: game.black.disconnected || game.white.disconnected
        }
        games_data.push(game_data)
    }
    viewersNamespace.emit("games-list", games_data)
}, 1000)

setInterval(() => {
    let participants = []

    clients.forEach(client => {
        let participant = {
            id: client.id,
            name: client.name,
            game_count: client.game_count,
            win_count: client.win_count,
            lost_count: client.lost_count,
            tie_count: client.tie_count,
            disconnected: client.disconnected
        }

        participants.push(participant)
    });
    viewersNamespace.emit("participants-list", participants)
}, 5000)

console.log("Port: ", argv.port)
io.listen(argv.port)