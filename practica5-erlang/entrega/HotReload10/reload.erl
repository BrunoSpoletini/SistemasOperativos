-module(reload).
-export([init/0, hello/0]).

hello() ->
    receive after 1000 -> ok end,
        io:fwrite("Hello ~p~n", [case rand:uniform(10) of 10 -> 1/uno; _ -> self() end]), % si sale 10, la queda, sino, dice hola a un proceso.
        reload:hello().

hello_backup() ->
    process_flag(trap_exit, true), %Con esto pedimos que el error llegue en forma de mensaje.
    spawn_link(?MODULE, hello, []),
    receive
    { 'EXIT', _Reason, _Stack } -> io:fwrite("Upa, se nos cayo Hello, ahi lo levantamos.~n"),
                                hello_backup()
    end.

init() -> spawn(fun () -> hello_backup() end).


% c(reload). -> reload:init() -> cambias el mensaje "hello" por "hola" -> c(reload). en la misma terminal -> listo.
