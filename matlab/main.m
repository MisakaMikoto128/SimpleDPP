function main
hd = localfunctions;
p = @putcharhandle;
disp(p);
p;
s = SimpleDPP(p,hd{1},hd{2});
s.send(uint8('Hello'));
s.send_datas(uint8('Hello'),uint8('world!'));
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

