-module(sumacon).
-export([run/0]).
-define(N, 100).

sumaAnillo(Index, PidSig, Suma, Nsumas) ->
    if (Nsumas == ?N) -> io:fwrite("Soy hilo ~p y tengo suma ~p!~n", [Index , Suma]);
        true ->
            receive 
            { Valor , 0} -> sumaAnillo( Index , PidSig , Suma+Valor, Nsumas+1);
                        
            { Valor , P } ->  PidSig ! {Valor, P-1},
                            sumaAnillo( Index , PidSig , Suma+Valor, Nsumas+1)
            end
        end.

esperaArranque(Index, Indexsig) ->
    receive
        {arranco, N, List} -> PidSig = lists:nth(Indexsig,List),
                        PidSig ! {Index,N},
                        sumaAnillo( Index , PidSig , 0, 0)
    end.

levantar(Num, Tot) ->
    case Num of
        0 -> [];
        _ -> 
            IndexSig = (Num rem Tot) + 1,
            Pid = spawn( fun() -> esperaArranque(Num , IndexSig) end),
            lists:append( levantar(Num-1, Tot), [Pid] )
    end.
        
run() ->
    Lista = levantar(?N,?N),
    io:fwrite("Levantamos hilos!"),
    lists:foreach( fun(X) -> X ! { arranco , ?N, Lista} end , Lista ),
    io:fwrite("Les mandamos señal de arranque!~n"),
    playok.
