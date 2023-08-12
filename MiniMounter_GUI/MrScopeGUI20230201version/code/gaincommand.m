function command=gaincommand(gain)
    if gain==16 || gain==32 || gain==64
        hexstr=dec2hex(gain,2);
        hexstr1(1)=hexstr(1);
        hexstr1(2)=hexstr(2);
        hexstr1(3)='0';
        hexstr1(4)='0';
        commandhex=strcat(hexstr1,'3504B8');
        command=hex2dec(commandhex);
    else
        fprintf('gain prop input error!\n');
        command=0;
    end
end

