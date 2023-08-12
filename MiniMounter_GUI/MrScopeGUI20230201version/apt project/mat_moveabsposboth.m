function [p,time] = mat_moveabsposboth(h1,h2,abs_pos1,abs_pos2)
    h1.SetAbsMovePos(0,abs_pos1); 
    h1.MoveAbsolute(0,1==0);
    h2.SetAbsMovePos(0,abs_pos2); 
    h2.MoveAbsolute(0,1==0);

    timeout = 60;
    t1 = clock; % current time
    p = [];
    while(etime(clock,t1)<timeout) 
        % wait while the motor is active; timeout to avoid dead loop
        s1 = h1.GetStatusBits_Bits(0);
        s2 = h2.GetStatusBits_Bits(0);
%         p = [p;h.GetPosition_Position(0)];
        %pause(0.05);
        if (IsMoving(s1) == 0) && (IsMoving(s2) == 0)
          pause(0.001);
          break;
        end
    end
end
