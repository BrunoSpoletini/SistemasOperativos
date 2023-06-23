-module(servidorTurnos4Old).
-export([server/0, rev/1, head/1, tail/1]).


-define(PORT,8000).

% Crea un socket de escucha tcp de recepcion pasiva, crea un proceso de registro de tickets y llama a wait_connect
server() ->
    {ok, ListenSocket} = gen_tcp:listen(?PORT, [binary, {packet, 0}, {active, false}, {reuseaddr, true}]),
    PidT = spawn(fun() -> ticketMaster(0) end),
    register(ticketM, PidT),
    wait_connect(ListenSocket, 0).

ticketMaster(N)->
    receive
        {Pid} -> Pid ! {N}, ticketMaster(N+1)
    end.

sendTicket(Socket)->
    ticketM ! {self()},
    receive
        {N} -> gen_tcp:send(Socket, integer_to_binary(N))
    end.


% Espera a que se conecte un cliente, crea un socket para atenderlo y
% crea otro proceso para seguir esperando clientes
wait_connect(ListenSocket, N) ->
    case gen_tcp:accept(ListenSocket) of
            {ok, Socket} -> 
                io:fwrite("Cliente ~p conectado!~n", [Socket]),
                spawn(fun () -> wait_connect(ListenSocket, N+1)  end),
                get_request(Socket, [], N);
            {error, closed} ->
                io:fwrite("Servidor cerrado~n");
            {error, Error} ->
                gen_tpc:close(ListenSocket),
                io:fwrite("Error al aceptar clientes por motivo: ~p~nServidor cerrado~n", [Error])
    end.


% Recibe requests provenientes del cliente "Socket"
get_request(Socket, Msgs, N) ->
    io:fwrite("Esperando mensajes de ~p~n", [Socket]),
    case gen_tcp:recv(Socket, 0) of
        {ok, Ms} ->  
            io:fwrite("El mensaje Final (cuando se haya terminado de recibir) es: ~p~n", [rev(list_to_binary([rev(Ms), Msgs]))]),
            process_msg(Socket, Ms, Msgs, N);
        {error, closed} -> 
            io:fwrite("Cliente desconectado!~n");
        {error, Error} -> 
            io:fwrite("Error al recibir mensaje de cliente por motivo: ~p~n", [Error])
    end.

% Parcea los mensajes y los procesa de la siguiente forma:
% (Nuevo, simple) -> Request unica, se envia el ticket al cliente
% (Nuevo, compuesto) -> Request multiple ("pegadas"), se envia el ticket
% al cliente y se procesa la segunda request.
% (Chau) -> 
% (NoMatch) -> La request no esta completa, se espera que llegue el resto
% (badReq) -> La request recibida no es valida
process_msg(Socket, Ms, Msgs, N) ->
    case parse_msg(rev(list_to_binary([rev(Ms), Msgs]))) of 
        {nuevo, simple, MsgLeft} ->
            sendTicket(Socket),
            io:fwrite("Recibio un nuevo entero~n"), %debug
            get_request(Socket, MsgLeft, N);
        {nuevo, compuesto, MsgLeft} ->
            sendTicket(Socket),
            io:fwrite("Recibio un nuevo entero(comp)~n"), %debug
            process_msg(Socket, <<>>, MsgLeft, N+1);
        {noMatch} -> 
            get_request(Socket, [rev(Ms), Msgs], N);
        {chau} -> 
            gen_tcp:close(Socket),
            io:fwrite("Cliente desconectado!~n");
        {badReq, CleanMsg} -> 
            io:fwrite("Solicitud incorrecta~n"),
            process_msg(Socket, <<>>, CleanMsg, N)
    end.

% Parsea el mensaje y checkea si es correcto
parse_msg(<<>>) -> {noMatch};
parse_msg(<<"NUEVO\n", Rest/binary>>) -> 
    RestList = binary_to_list(Rest),
    Var = head(string:split(RestList, "\n")),
    io:fwrite("Var: ~p, RestList: ~p~n",[Var, RestList]), %debug
    if (Var /= RestList) ->
        {nuevo, compuesto, rev(Rest)};
    true ->
        {nuevo, simple, rev(Rest)}
    end;
parse_msg(<<"CHAU\n", _/binary>>) -> {chau};
parse_msg(Rest) -> io:fwrite("ASDASDSAD: ~p~n",[Rest]),
                    case is_msg_correct(Rest) of
                        {correcto} -> 
                            {noMatch};
                        {incorrecto, CleanMsg} -> 
                            {badReq, CleanMsg}
                    end.
                
% (Probablemente se pueda mejorar haciendo un mejor manejo de strings)
% Checkea que el mensaje comience por una parte de los terminos "NUEVO\n" o "CHAU\n", si no
% lo hace, el mensaje se marca como invalido.
is_msg_correct(<<"NUEVO\n">>) -> {correcto};
is_msg_correct(<<"CHAU\n">>) -> {correcto};
is_msg_correct(Msg) ->  SplitN = (head(string:split("NUEVO\n", Msg))),
                        SplitC = (head(string:split("CHAU\n", Msg))),
                        io:fwrite("SplitN = ~p, SplitC = ~p, Msg =~p~n", [SplitN,SplitC, Msg]),
                        if ((SplitN == [[]]) or (SplitC == [[]])) -> 
                            {correcto};
                        true -> 
                            CleanMsg = tail(string:split(binary_to_list(Msg), "\n")),
                            {incorrecto, rev(list_to_binary(CleanMsg))}
                        end.

% Devuelve el tail de una lista
tail([_|[]]) -> [];
tail([_|Y]) -> [Y].

% Devuelve el primer elemento de una lista
head([]) -> [];
head([[]]) -> [];
head([H|_]) -> [H].

% Devuelve el reverso de una lista en binary
rev(<<>>) -> <<>>;
rev(Binary) ->
   Size = size(Binary)*8,
   <<X:Size/integer-little>> = Binary,
   <<X:Size/integer-big>>.
