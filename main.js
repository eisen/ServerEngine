const io = require('socket.io')();
const { v4: uuidv4, NIL } = require('uuid');

var clients = []
var games = []

var tournament_running = false

const timeout = 3 // seconds

const empty_board = [
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 2, 0, 0, 0,
    0, 0, 0, 2, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
]

function connection(socket)
{
    clients.push({
        "id": socket.id,
        "socket": socket,
        game_count: 0,
        win_count: 0,
        tie_count: 0
    })

    socket.emit("set timeout", timeout) // seconds

    socket.on("move", move)
    socket.on("pass", pass)
    socket.on("set_team", set_team)
}

function start(type)
{
    if(tournament_running == true)
    {
        console.log("Tournament running, can't styart a new one")
        return
    }

    if(type === "round robin")
    {
        tournament_running = true

        for(var idx = 0; idx < client.length - 1; ++idx ) // Everybody plays everybody once
        {
            var game = {
                id: uuidv4(),
                client1: client[idx],
                client2: client[idx + 1],
                black: Math.random() < 0.5 ? client[idx] : client[idx + 1],
                white: NIL, // Need to determine below
                turn: "black",
                board: start_board,
                ended: false
            }

            // Set Opponents
            game.client1.socket.emit("set oppponent", game.id, client2.name)
            game.client2.socket.emit("set oppponent", game.id, client1.name)

            ++game.client1.game_count
            ++game.client2.game_count

            // Determine white player
            if(game.client1 === game.black) game.white = game.client2
            else game.white = game.client1

            games.push(game)
        }

        foreach(game in games)
        {
            // Kickoff each game
            game.black.socket.emit("make move", game.id, game.board, game.turn)
        }
    }
    else if(type === "double elimination")
    {

    }
}

function move(game_id, out_board)
{
    games[game_id].board = out_board;
    next_turn(games[game_id])
}

function pass(game_id)
{
    next_turn(games[game_id])
}

function next_turn(game)
{
    if(game_ended(game) === false)
    {
        if(game.turn === "black")
        {
            game.turn = "white"
            game.white.socket.emit("make move", game.id, game.board, game.turn)
        }
        else
        {
            game.turn = "black"
            game.black.socket.emit("make move", game.id, game.board, game.turn)
        }
    }
    else
    {
        // Game ended
        calculate_scores(game)
    }
}

function set_team(client_id, name)
{
    client[client_id].name = name
}

function game_ended(game)
{
    var empty_cells = 64;
    foreach( cell in game.board)
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
    var black_count = 0;
    var white_count = 0;
    foreach(cell in game.board)
    {
        if(cell === 1) ++black_count
        else if(cell === 2) ++white_count
    }

    game.client1.socket.emit("game ended", game_id, black_count, white_count)
    game.client2.socket.emit("game ended", game_id, black_count, white_count)

    if(black_count > white_count)
    {
        ++game.black.win_count
    }
    else if(white_count > black_count)
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
    foreach(game in games)
    {
        if(game.ended === true)
        {
            ++games_ended
        }
    }

    if(games_ended === games.length) // Tournament complete
    {
        tournament_running = false
        foreach( client in clients )
        {
            client.socket.emit("tournament ended", client.game_count, client.win_count)
        }
    }
}

io.on('connection', connection);

io.on("start", start)

io.listen(8080);