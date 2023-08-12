function [p,time] = mat_moveabspos(h,abs_pos)
    h.SetAbsMovePos(0,abs_pos); 
    h.MoveAbsolute(0,1==0);

    timeout = 60;
    t1 = clock; % current time
    p = [];
    while(etime(clock,t1)<timeout) 
        % wait while the motor is active; timeout to avoid dead loop
        s = h.GetStatusBits_Bits(0);
%         p = [p;h.GetPosition_Position(0)];
        %pause(0.05);
        if (IsMoving(s) == 0)
          pause(0.001);
          break;
        end
    end
end
