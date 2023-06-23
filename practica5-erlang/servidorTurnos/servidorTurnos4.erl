-module(servidorTurnos4).
-export([server/0, rev/1, head/1, tail/1]).
-define(PORT,8000).
-define(DEBUG, true).
% c(servidorTurnos4), spawn(servidorTurnos4,server,[]).

% Crea un socket de escucha tcp de recepcion pasiva, 
% crea un proceso de registro de tickets y llama a wait_connect
server() ->
    {ok, ListenSocket} = gen_tcp:listen(?PORT, [binary, {packet, 0}, {active, false}, {reuseaddr, true}]),
    PidT = spawn(fun() -> ticketMaster(0) end),
    register(ticketM, PidT),
    wait_connect(ListenSocket).

% Recibe el pid de un proceso y le envia un ticket (entero) unico
ticketMaster(N)->
    receive
        {Pid} -> Pid ! {N}, ticketMaster(N+1)
    end.

% Funcion para recibir un ticket y enviarlo al cliente por medio de un socket con protocolo tcp
sendTicket(Socket)->
    ticketM ! {self()},
    receive
        {N} -> gen_tcp:send(Socket, integer_to_binary(N))
    end.

% Espera conecciones por el socket del servidor
% Cuando se conecta un cliente, pasa a atenderlo,
% y crea otro proceso para seguir esperando conecciones.
wait_connect(ListenSocket) ->
    case gen_tcp:accept(ListenSocket) of
            {ok, Socket} -> 
                print("Cliente ~p conectado!~n", [Socket], ?DEBUG),
                spawn(fun () -> wait_connect(ListenSocket)  end),
                get_request(Socket, []);
            {error, closed} ->
                print("Servidor cerrado~n", [], ?DEBUG);
            {error, Error} ->
                gen_tpc:close(ListenSocket),
                print("Error al aceptar clientes por motivo: ~p~nServidor cerrado~n", [Error], ?DEBUG)
    end.

% Recibe requests provenientes del cliente "Socket"
get_request(Socket, Msgs) ->
    print("Esperando mensajes de ~p~n", [Socket], ?DEBUG),
    case gen_tcp:recv(Socket, 0) of
        {ok, Ms} ->  
            process_msg(Socket, Ms, Msgs);
        {error, closed} -> 
            print("Cliente desconectado!~n", [], ?DEBUG);
        {error, Error} -> 
            print("Error al recibir mensaje de cliente por motivo: ~p~n", [Error], ?DEBUG)
    end.

% Parcea los mensajes y los procesa de la siguiente forma:
% (Nuevo, simple) -> Request unica, se envia el ticket al cliente
% (Nuevo, compuesto) -> Request multiple ("pegadas"), se envia el ticket
% al cliente y se procesa la segunda request.
% (Chau) -> Desconecta al cliente
% (NoMatch) -> Request incompleta, se espera que llegue el resto
% (badReq) -> La request recibida no es valida
process_msg(Socket, Ms, Msgs) ->
    case parse_msg(rev(list_to_binary([rev(Ms), Msgs]))) of 
        {nuevo, simple, MsgLeft} ->
            sendTicket(Socket),
            get_request(Socket, MsgLeft);
        {nuevo, compuesto, MsgLeft} ->
            sendTicket(Socket),
            process_msg(Socket, <<>>, MsgLeft);
        {noMatch} -> 
            get_request(Socket, [rev(Ms), Msgs]);
        {chau} -> 
            gen_tcp:close(Socket),
            print("Cliente desconectado!~n", [], ?DEBUG);
        {badReq, CleanMsg} -> 
            print("Solicitud incorrecta~n", [], ?DEBUG),
            process_msg(Socket, <<>>, CleanMsg)
    end.

% Parsea el mensaje y checkea si es correcto
parse_msg(<<>>) -> {noMatch};
parse_msg(<<"NUEVO\n", Rest/binary>>) -> 
    RestList = binary_to_list(Rest),
    Var = head(string:split(RestList, "\n")),
    if (Var /= RestList) ->
        {nuevo, compuesto, rev(Rest)};
    true ->
        {nuevo, simple, rev(Rest)}
    end;
parse_msg(<<"CHAU\n", _/binary>>) -> {chau};
parse_msg(Rest) ->  case is_msg_correct(Rest) of
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
                        print("SplitN = ~p, SplitC = ~p, Msg =~p~n", [SplitN,SplitC, Msg], ?DEBUG),
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

% Imprime por pantalla lo recibido como parametro
% si DEBUG esta seteado en 1
print(X, Args, Debug) -> if Debug -> 
                        io:fwrite(X,Args);
                    true -> ok
                  end.