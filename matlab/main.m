function main
    
hd = localfunctions;
p = @putcharhandle;
disp(p);
p;
s = SimpleDPP(p,hd{1},hd{2});
ret = s.send(uint8('Hello'));
if(ret == SimpleDPP.SIMPLEDPP_SENDFAILED)
    disp("Send error!")
end
ret = s.send_datas(uint8('Hello'),uint8('world!'));
if(ret == SimpleDPP.SIMPLEDPP_SENDFAILED)
    disp("Send error!")
end

disp(hd);


    function putcharhandle(c)
        disp(char(c));
        s.parse(c);
    end
end


function SimpleDPPRecvhandle(datas)

disp(datas);
disp(char(datas));
end

function SimpleDPPErrorhandle(error_code)

disp(error_code);
end

