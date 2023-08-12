function command=ledcommand(led0)
    if led0>255||led0<0
        fprintf('led prop input error!\n');
        command=0;
    else
        hexstr=dec2hex(led0,2);
        hexstr1(1)=hexstr(2);
        hexstr1(2)='0';
        hexstr1(3)='0';
        hexstr1(4)=hexstr(1);
        commandhex=strcat(hexstr1,'0398');
        command=hex2dec(commandhex);
    end
end

