-module(sumacon).
-export([run/0]).
-define(N,4).

% Recibe valores y envia la suma N veces hasta completar el ciclo,
% e imprime el resultado.
sumaAnillo(Index, PidSig, Suma, Nsumas) ->
    if (Nsumas == ?N) -> 
            io:fwrite("Soy hilo ~p y tengo suma ~p!~n", [Index , Suma]);
        true ->
            receive 
            { Valor , 0} -> 
                sumaAnillo( Index , PidSig , Suma+Valor, Nsumas+1);
            { Valor , P } ->  
                PidSig ! {Valor, P-1},
                sumaAnillo( Index , PidSig , Suma+Valor, Nsumas+1)
            end
        end.

% El proceso espera una señal de arranque.
% Cuando la recibe, envia su valor de Index al proceso siguiente
% e inicia la funcion para hacer N sumas
esperaArranque(Index) ->
    IndexSig = (Index rem (?N)) + 1,
    receive
        {arranco, List} -> 
            PidSig = lists:nth(IndexSig,List),
            PidSig ! {Index,?N},
            sumaAnillo( Index , PidSig , 0, 0)
    end.

% Se levantan N procesos, se les asigna una Id unica,
% y se devuelve una lista con los pid's de los procesos
levantar(Num) ->
    case Num of
        0 -> [];
        _ -> 
            Pid = spawn( fun() -> esperaArranque(Num) end),
            lists:append( levantar(Num-1), [Pid] )
    end.
        
% Levanta N procesos y envia la señal de arranque a cada uno de ellos
run() ->
    Lista = levantar(?N),
    io:fwrite("Levantamos hilos!~n"),
    lists:foreach( fun(X) -> X ! {arranco, Lista} end, Lista ),
    io:fwrite("Les mandamos señal de arranque!~n"),
    playok.
