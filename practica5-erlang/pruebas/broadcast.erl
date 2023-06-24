-module(broadcast).

%Funciones de control
-export([start/0,stop/1]).

%Funciones interaccion
-export([broadcast/2, registrar/1]).
-export([loopBroadcast/1]).
-export([test/0, client/1]).

-export([sleep/1, divzero/0]).

% ?MODULE hace referencia al modulo cactual
% Variables tienen que empezar con  mayuscula,
% si inician en minuscula son atomos

% origen ! {mensaje} %envio de msje

% el tercer argumento de spawn es una lista de argumentos
start()->
    spawn(?MODULE, loopBroadcast,[[]]). 

stop(Broadcaster) ->
    Broadcaster ! fin. 

broadcast(Broadcaster, Msg)->
    Broadcaster ! {env, Msg}.

registrar(Broadcaster)->
    Broadcaster ! {reg, self()}.


% Si es la ultima guarda, no va nada.
% sino, va un ; separando guardas.
% Para instrucciones consecutivas dentro dela misma guarda, se separan con ,
loopBroadcast(Clientes) -> 
    receive
        {fin} -> ok;
        {env,Msg} -> lists:foreach( fun(Client) -> Client ! Msg end, Clientes),
                    io:fwrite("Mandamos mensajes ~n"),
                    loopBroadcast(Clientes);
        {reg,Pid} -> 
            io:fwrite("client ~p registered ~n", [Pid]),
            loopBroadcast([ Pid | Clientes ])
    end.


test() ->
    Broadcaster = broadcast:start(),
    spawn( ?MODULE , client , [Broadcaster] ),
    spawn( ?MODULE , client , [Broadcaster] ),
    broadcast:sleep(2000),
    broadcast(Broadcaster, hola),
    timer:sleep(500).
    %stop(Broadcaster).

% Los parametros del fwrite van en forma de lista tambien.
client(Broadcaster) ->
        registrar(Broadcaster),

        io:fwrite("client ~p esperando mensaje  ~n", [self()] ),
         
        receive
            Msg -> io:fwrite("client ~p recibio  ~p ~n", [self() , Msg] )
        end.
        

sleep(Time) ->
    receive
    after
        Time -> ok
    end.

empty_mailbox() ->
    receive
    _ -> empty_mailbox()
    after
        0 -> ok
    end.

%ponerle prioridad a los mensajes.


% throw , catch , exit(normar), error().

throwExep() -> throw

divzero() ->  case catch( 0  / 0 ) of
            { 'EXIT' ,{badarith,Stack} } -> io:fwrite("Dividiste por 0. ~n");
                _ -> io:fwrite("Otra clausula ~n")
            end.


