-module(temporizacion2).
-export([init/0, wait/1, aux/3, cronometro/3]).

wait( Ms ) ->
    receive
        {imp} -> notOk
    after
        Ms -> ok
    end.

aux( _, 0, _ ) -> ok;
aux( Fun, Hasta, Periodo ) ->
    wait( Periodo ),
    Fun(),
    aux( Fun, max((Hasta-Periodo),0), Periodo ).

cronometro( Fun, Hasta, Periodo ) ->
    Fun(),    
    spawn(temporizacion2 ,aux, [Fun, max((Hasta-Periodo),0), Periodo]).
    
init() -> 
    %io:fwrite("Ping"),
    %wait(1000),
    %&io:fwrite("Pong").
    cronometro(fun () -> io:fwrite("Tick~n") end, 60000, 5000).

