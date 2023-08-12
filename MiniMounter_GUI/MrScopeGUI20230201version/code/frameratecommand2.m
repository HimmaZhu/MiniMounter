function command=frameratecommand2(rate)
    if rate==10 || rate==15 || rate==20 || rate==30 || rate==60

        switch (rate)
            case 10
                i2cValue2=1720;
            case 15
                i2cValue2=1220;
            case 20
                i2cValue2=800;
            case 30
                i2cValue2=1000;
            case 60
                i2cValue2=500;
        end
        hexstr=dec2hex(i2cValue2,4);
        hexstr1(1)=hexstr(3);
        hexstr1(2)=hexstr(4);
        hexstr1(3)=hexstr(1);
        hexstr1(4)=hexstr(2);
        commandhex=strcat(hexstr1,'0B03B8');
        command=hex2dec(commandhex);
    else
        fprintf('gain prop input error!\n');
        command=0;
    end
end

