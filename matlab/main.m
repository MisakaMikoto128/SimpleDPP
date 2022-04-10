function main
    
hd = localfunctions;
p = @putcharhandle;

s = SimpleDPP(p,hd{1},hd{2});
ret = s.send('Hello');
if(ret == SimpleDPP.SIMPLEDPP_SENDFAILED)
    disp("Send error!");
end

ret = s.send_datas('Hello','world!');
if(ret == SimpleDPP.SIMPLEDPP_SENDFAILED)
    disp("Send error!")
end

    function putcharhandle(c)
        disp(char(c));
        s.parse(uint8(c));
    end
end


function SimpleDPPRecvhandle(datas)
disp(char(datas));
end

function SimpleDPPErrorhandle(error_code)

disp(error_code);
end

