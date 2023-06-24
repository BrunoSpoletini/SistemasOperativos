-module(sync).
-export([createLock/0, lock/1, unlock/1, destroyLock/1]).
-export([createSem/1, semP/1, semV/1, destroySem/1]).
-export([testLock/0, testSem/0]).



semaphore(N, Xs) ->
    if ( (N > 0) and (Xs /= []) ) ->
            lists:last(Xs) ! {downok},
            semaphore(N-1, lists:droplast(Xs));
        true ->
            receive
            {down, Pid} -> semaphore(N, lists:append( [Pid], Xs  ) );
            {up} -> semaphore( N+1, Xs)
        end
    end.

    
createSem (_N) -> spawn( fun() -> semaphore(_N, []) end).

destroySem (_S) -> exit( _S , kill ).

semP (_S) -> _S ! {down, self() },
            receive
                {downok} -> ok
            end.
semV (_S) -> _S ! {up}.



createLock () -> createSem(1).
lock (_L) -> semP (_L).
unlock (_L) -> semV (_L).
destroyLock (_L) -> destroySem (_L).



f (L, W) ->
    lock(L),
    % regioncritica(),
    io:format("uno ~p~n", [self()]),
    io:format("dos ~p~n", [self()]),
    io:format("tre ~p~n", [self()]),
    io:format("cua ~p~n", [self()]),
    unlock(L),
    W ! finished.

waiter (L, 0) -> destroyLock(L);
waiter (L, N) -> receive finished -> waiter(L, N-1) end.

waiter_sem (S, 0) -> destroySem(S);
waiter_sem (S, N) -> receive finished -> waiter_sem(S, N-1) end.

testLock () ->
    L = createLock(),
    W = spawn(fun () -> waiter(L, 3) end),
    spawn (fun () -> f(L, W) end),
    spawn (fun () -> f(L, W) end),
    spawn (fun () -> f(L, W) end),
    ok.

sem (S, W) ->
    semP(S),
    %regioncritica(), bueno, casi....
    io:format("uno ~p~n", [self()]),
    io:format("dos ~p~n", [self()]),
    io:format("tre ~p~n", [self()]),
    io:format("cua ~p~n", [self()]),
    io:format("cin ~p~n", [self()]),
    io:format("sei ~p~n", [self()]),
    semV(S),
    W ! finished.
testSem () ->
    S = createSem(2), % a lo sumo dos usando io al mismo tiempo
    W = spawn (fun () -> waiter_sem (S, 5) end),
    spawn (fun () -> sem (S, W) end),
    spawn (fun () -> sem (S, W) end),
    spawn (fun () -> sem (S, W) end),
    spawn (fun () -> sem (S, W) end),
    spawn (fun () -> sem (S, W) end),
    ok.
