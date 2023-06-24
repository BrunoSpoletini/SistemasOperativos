-module(hello).
-export([init/0]).

hello() ->
    receive after 1000 -> ok end,
        io:fwrite("Hola ~p~n", [case rand:uniform(10) of 10 -> 1/uno; _ -> self() end]), % si sale 10, la queda, sino, dice hola a un proceso.
        hello().

hello_backup() ->
    process_flag(trap_exit, true), %Con esto pedimos que el error llegue en forma de mensaje.
    spawn_link(fun () -> hello() end), % con esto el mensaje nos llega a nosotros.
    receive
    { 'EXIT',_Reason,_Stack } -> io:fwrite("Upa, se nos cayo Hello, ahi lo levantamos.~n"),
                                hello_backup()
    end.

init() -> spawn(fun () -> hello_backup() end).
