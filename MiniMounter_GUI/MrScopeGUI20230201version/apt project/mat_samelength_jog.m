function p = mat_samelength_jog(h1,h2,stepsize,direction)
    h1.SetJogStepSize(0,stepsize);
    h2.SetJogStepSize(0,stepsize);
    
    %direction 1-forward 2-inverse
    h1.MoveJog(0,direction);
    h2.MoveJog(0,direction);

    timeout = 60;
    t1 = clock; % current time
    p = [];
    while(etime(clock,t1)<timeout) 
        % wait while the motor is active; timeout to avoid dead loop
        s1 = h1.GetStatusBits_Bits(0);
        s2 = h2.GetStatusBits_Bits(0);
        p1 = h1.GetPosition_Position(0);
        p2 = h2.GetPosition_Position(0);
        p = [p;p1,p2];
        pause(0.05);
        if (IsMoving(s1) == 0) && (IsMoving(s2) == 0)
          pause(0.5);
          break;
        end
    end
end