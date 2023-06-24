
-module(pepe).
-export([run/1, hacer/1, recorrer2/2]).

hacer(N) ->
    case N of
    0 -> [];
    P -> lists:append( [P] , hacer(N-1) )
    end.


recorrer2(N,Xs) -> case N of
                    0 -> ok;
                    P -> io:fwrite("Soy el num ~p ~n", [ lists:nth(P,Xs) ] ),
                        recorrer2(N-1,Xs)
                    end.

recorrer( [] ) -> ok;
recorrer( [ X | Xs] ) -> io:fwrite("Soy el num ~p", [X]),
                     recorrer([Xs]).


run(N) ->
    Lista = hacer(N),
    lists:foreach( fun(X) -> io:fwrite("Soy el num ~p ~n",[X]) end , Lista ),
    io:fwrite("Terminamos!"),
    playok.


    
   
