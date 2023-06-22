-module(servidorTurnos4).
-export([server/0]).

-define(PORT,8000).
-define(HOST,'localhost').

% Crea un socket de escucha tcp de recepcion activa y llama a wait_connect
server() ->
    {ok, ListenSocket} = gen_tcp:listen(8000, [{active, true}, {reuseaddr, true}]), %{packet, 4},
    wait_connect(ListenSocket, 0).

% Espera a que se conecte un cliente, crea un socket para atenderlo y
% crea otro proceso para seguir esperando clientes
wait_connect(ListenSocket, N) ->
    {ok, Socket} = gen_tcp:accept(ListenSocket),
    io:fwrite("Cliente ~p conectado!~n", [Socket]),
    spawn(fun () -> wait_connect(ListenSocket, N+1) end),
    get_request(Socket).

% Recibe requests provenientes del cliente "Socket"
get_request(Socket) ->
    io:fwrite("Esperando mensajes de ~p~n", [Socket]),
    receive
        {tcp, Socket, Mensaje} -> 
            parse_msg(Mensaje),
            get_request(Socket);
        {tcp_closed, Socket, Error} ->
            io:write("Error: ~p", Error),
            gen_tcp:close(Socket);
    %after 2000 -> io:write("Timed out") 
    end.


    % Res = "120\n",
    % io:format("~s", [Res]),
    % gen_tcp:send(Socket, Res),

% client() -> 
%     {ok, Socket} = gen_tcp:connect('localhost', 8000, [{active, true}, {reuseaddr, true}]), % {packet, 4},
    
%     timer:sleep(1000),

%     % Mandamos la request al servidor
%     %Req = "NUEVO\n",
%     %io:format("ASDAWD ~s", [Req]),

%     gen_tcp:send(Socket, "NUEVO"),

%     timer:sleep(100000),
%     % Recibimos la respuesta del servidor
%     % receive
%     %     {ok, Res} -> io:write("Respuesta recibida: ~s~n", Res);
%     %     {error, Error} -> 
%     %         io:write("Error: ~p", Error),
%     %         gen_tcp:close(Socket);
%     %         _ -> io:write("Problema de parseo")
%     % end,
    
%     okClient.
%     %ok = gen_tcp:close(Socket),

% c(servidorTurnos4), spawn(servidorTurnos4,server,[]), timer:sleep(1000), servidorTurnos4:client().
% % c(servidorTurnos4), spawn(servidorTurnos4,server,[]).