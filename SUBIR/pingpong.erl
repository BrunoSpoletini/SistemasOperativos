
% https://www.erlang.org/doc/getting_started/conc_prog.html

-module(pingpong).
-export([play/0,pong/0,ping/0,ping_link/1, pong_aux/0]).

% Pong

pong_aux()->
    process_flag(trap_exit, true), %te llega como mensaje la salida del proceso linkeado
    pong().
pong() ->
    receive
        {0 , PidPing} ->
            io:fwrite("Final Pong!~n"),
            PidPing ! {fin, self()},
            pongok; % returned value (this is an atom)
        {N , PidPing} ->
            io:fwrite("Pong! Recv: ~p ~n",[N]), % ~p for each value in the list and ~n for new line
            PidPing ! {(N-1) , self()},
            pong(); % call pong again to wait new messages
        {'EXIT', From, Reason} -> io:fwrite("Pong! La quede! ~n"), ok,
    end.


%link crea conexion bidireciconal entre los procesos. Si algunos crashea, crashean los dos.
% Ping

ping_link(PidPong) ->
        link(PidPong),
        ping().
ping() ->
    receive
        {fin , _PidPong } -> 
        io:fwrite("Final Ping!~n"),
        pingok;
        {N , PidPong} ->
            link(PidPong),
            io:fwrite("Ping!"),
            error(quedo),
            PidPong ! {N , self()}, % self() Returns the pid of the calling processes.
            ping()
    end.

play() ->
    PidPong = spawn(pingpong, pong_aux, []),
    PidPing = spawn(pingpong, ping_link, [PidPong]),
    PidPong ! { 10 , PidPing },
    playok.
