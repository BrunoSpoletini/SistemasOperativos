-module(hello).
-export([init/0, saludo/0]).

init() -> spawn(hello, saludo, []).

saludo() -> 
    io:fwrite("Hello~p~n", [self()]),
    timer:sleep(1000),
    hello:saludo().

% c(hello), hello:init().
% cambias el mensaje "hello" por "hola" 
% c(hello). % en la misma terminal 
