function command=frameratecommand1(rate)
    if rate==10 || rate==15 || rate==20 || rate==30 || rate==60

        switch (rate)
            case 10
                i2cValue=49153250;
            case 15
                i2cValue=43057902;
            case 20
                i2cValue=57016641;
            case 30
                i2cValue=6160929;
            case 60
                i2cValue=6094881;
        end
        hexstr=dec2hex(i2cValue,8);
        hexstr1(1)=hexstr(7);
        hexstr1(2)=hexstr(8);
        hexstr1(3)=hexstr(5);
        hexstr1(4)=hexstr(6);
        hexstr1(5)=hexstr(3);
        hexstr1(6)=hexstr(4);
        hexstr1(7)=hexstr(1);
        hexstr1(8)=hexstr(2);
        commandhex=strcat(hexstr1,'05B9');
        command=hex2dec(commandhex);
    else
        fprintf('gain prop input error!\n');
        command=0;
    end
end

