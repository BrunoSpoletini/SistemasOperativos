-module(servidorTurnos4Old).
-export([server/0, rev/1, head/1]).
-import(lists,[reverse/1]).

-define(PORT,8000).
-define(HOST,'localhost').

% Crea un socket de escucha tcp de recepcion pasiva y llama a wait_connect
server() ->
    {ok, ListenSocket} = gen_tcp:listen(8000, [binary, {packet, 0}, {active, false}, {reuseaddr, true}]),
    wait_connect(ListenSocket, 0).

% Espera a que se conecte un cliente, crea un socket para atenderlo y
% crea otro proceso para seguir esperando clientes
wait_connect(ListenSocket, N) ->
    case gen_tcp:accept(ListenSocket) of
            {ok, Socket} -> 
                io:fwrite("Cliente ~p conectado!~n", [Socket]),
                spawn(fun () -> wait_connect(ListenSocket, N+1)  end),
                get_request(Socket, [], N);
            {error, closed} ->
                io:fwrite("Conexion con el servidor terminada~n");
            {error, Error} ->
                gen_tpc:close(ListenSocket),
                io:fwrite("Error al aceptar clientes por motivo: ~p~n", [Error])
    end.


% Recibe requests provenientes del cliente "Socket"
get_request(Socket, Msgs, N) ->
    io:fwrite("Esperando mensajes de ~p~n", [Socket]),
    case gen_tcp:recv(Socket, 0) of
        {ok, Ms} ->  

            io:fwrite("El mensaje Final (cuando se haya terminado de recibir) es: ~p~n", [rev(list_to_binary([rev(Ms), Msgs]))]),



        {error, Error} -> 
            io:fwrite("Error al recibir mensaje de cliente por motivo: ~p~n", [Error])
    end.


% Parceamos el mensaje en el orden correcto,

process_msg(Socket, Ms, Msgs, N) ->

    case parse_msg(rev(list_to_binary([rev(Ms), Msgs]))) of 
        {nuevo, MsgLeft} ->
            %gen_tcp:send(Socket, list_to_binary(integer_to_list(N))),
            io:fwrite("Recibio un nuevo entero~n"), %debug
            
            get_request(Socket, MsgLeft, N);
        {chau, MsgLeft} ->pending%%pendiente
        {noMatch} ->pending;
        {errorMsg} -> pending
    end;



% Parsea el mensaje y checkea si es correcto
parse_msg([]) -> {noMatch};
parse_msg("NUEVO\n" ++ Rest) -> {nuevo, rev(Rest)};
parse_msg("CHAU\n" ++ Rest) -> {chau, rev(Rest)};
parse_msg(Rest) -> case is_msg_correct(Rest) of
                        {correcto} -> 
                            {noMatch};
                        {incorrecto} -> 
                            io:fwrite("Solicitud incorrecta~n"),
                            {errorMsg}
                    end.
                
% (Probablemente se pueda mejorar haciendo un mejor manejo de strings)
% Checkea que el mensaje comience por el termino "NUEVO\n" o "CHAU\n", si no
% lo hacen, el mensaje se marca como invalido.
is_msg_correct(Msg) ->  SplitN = (head(string:split("NUEVO\n", Msg))),
                        SplitC = (head(string:split("CHAU\n", Msg))),
                        if ((SplitN == []) or (SplitC == [])) -> {correcto};
                        true -> {incorrecto}
                        end.

% Devuelve el primer elemento de una lista
head([]) -> [];
head([H|_]) -> [H].


rev(Binary) ->
   Size = size(Binary)*8,
   <<X:Size/integer-little>> = Binary,
   <<X:Size/integer-big>>.

%c(servidorTurnos4Old), spawn(servidorTurnos4Old,server,[]).

% client() -> 
%     {ok, Socket} = gen_tcp:connect(localhost, 8000, [{active, false}, {reuseaddr, true}]),
%     %Test = "Test",
%     %io:format("Msge enviado: ~s~n", [Test]),
%     timer:sleep(1000),


%     %ok = gen_tcp:send(Socket, Test),


%     timer:sleep(1000),
%     {ok,Resp} = gen_tcp:recv(Socket, 0),
%     io:format("msge recib por el cliente: ~s~n", [Resp]),
%     ok.
%     %ok = gen_tcp:close(Socket),
%     %Resp.
%     %clienteCerrado.

% c(servidorTurnos4), spawn(servidorTurnos4,server,[]), timer:sleep(1000), servidorTurnos4:client().