-module(a).
-export([init/1, rev/1,ticketMaster/1,sendTicket/1]).

init(_) -> 
   PidT = spawn(?MODULE, ticketMaster, [0] ),
   register(ticketM, PidT),
   ok.

ticketMaster(N)->
    receive
        {Pid} -> Pid ! {N}, ticketMaster(N+1)
    end.

sendTicket(_)->
    ticketM ! {self()},
    receive
        {N} -> io:fwrite( "Ticket es: ~p~n", [integer_to_binary(N)])
    end.

rev(Binary) ->
   Size = size(Binary)*8,
   <<X:Size/integer-little>> = Binary,
   <<X:Size/integer-big>>.

% c(a),a:init().

% my_binary_to_list(<<H,T/binary>>) ->
%     [H|my_binary_to_list(T)];
% my_binary_to_list(<<>>) -> [].


% <<"NUEVO\nNUEVO\nNU">>
% 18> binary_to_list(<<"NUEVO\nNUEVO\nNU">>).
% "NUEVO\nNUEVO\nNU"

% 5> a:rev("asd").
% ** exception error: bad argument
%      in function  size/1
%         called as size("asd")
%         *** argument 1: not tuple or binary
%      in call from a:rev/1 (a.erl, line 7)
% 6> a:rev(<<"asd">>).
% <<"dsa">>