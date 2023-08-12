function p = mat_samelength(h1,h2,length)
    p_h1 = h1.GetPosition_Position(0);
    p_h2 = h2.GetPosition_Position(0);

    absp_h1 = p_h1 + length;
    absp_h2 = p_h2 + length;

    h1.SetAbsMovePos(0,absp_h1);
    h1.MoveAbsolute(0,1==0);
    h2.SetAbsMovePos(0,absp_h2);
    h2.MoveAbsolute(0,1==0);
    

    timeout = 60;
    t1 = clock; % current time
    p = [];
    while(etime(clock,t1)<timeout) 
        % wait while the motor is active; timeout to avoid dead loop
        s1 = h1.GetStatusBits_Bits(0);
        s2 = h2.GetStatusBits_Bits(0);
%         p1 = h1.GetPosition_Position(0);
%         p2 = h2.GetPosition_Position(0);
%         p = [p;p1,p2];
        if (IsMoving(s1) == 0) && (IsMoving(s2) == 0)
          pause(0.001);
          break;
        end
    end
end