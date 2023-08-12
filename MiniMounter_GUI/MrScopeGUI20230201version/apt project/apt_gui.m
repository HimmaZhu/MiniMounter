clear; close all; clc;
global h1; % make h a global variable so it can be used outside the main
global h2;          % function. Useful when you do event handling and sequential           move
%% Create Matlab Figure Container
fpos    = get(0,'DefaultFigurePosition'); % figure default position
fpos(1) = 450;
fpos(2) = 100;
fpos(3) = 500; % figure window size;Width
fpos(4) = 700; % Height
 
f = figure('Position', fpos,...
           'Menu','None',...
           'Name','APT GUI');
%% Create ActiveX Controller
h1 = actxcontrol('MGMOTOR.MGMotorCtrl.1',[20 340 600 325 ], f);
h2 = actxcontrol('MGMOTOR.MGMotorCtrl.1',[20 20 600 325 ], f);
 
%% Initialize
% Start Control
h1.StartCtrl;
h2.StartCtrl;
 
% Set the Serial Number
SN = 26004285; % put in the serial number of the hardware
set(h1,'HWSerialNum', SN);
SN = 26004469; % put in the serial number of the hardware
set(h2,'HWSerialNum', SN);
 
% Indentify the device
h1.Identify;
h2.Identify;
 
pause(5); % waiting for the GUI to load up;
%% Controlling the Hardware
h1.MoveHome(0,0); % Home the stage. First 0 is the channel ID (channel 1)
h2.MoveHome(0,0);% second 0 is to move immediately
%% Event Handling
h1.registerevent({'MoveComplete' 'MoveCompleteHandler'});
h2.registerevent({'MoveComplete' 'MoveCompleteHandler'});
 
%% Sending Moving Commands
timeout = 10; % timeout for waiting the move to be completed
%h.MoveJog(0,1); % Jog
 
% Move a absolute distance
% h1.SetAbsMovePos(0,3);
% h1.MoveAbsolute(0,1==0);
% h2.SetAbsMovePos(0,2);
% h2.MoveAbsolute(0,1==0);
% movetogether(h1,h2);
for i = 1:200
    mat_samelength(h1,h2,-0.001);
end

tic
mat_moveabsposboth(h1,h2,0.002,0.002);
toc

 
t1 = clock; % current time
while(etime(clock,t1)<timeout) 
% wait while the motor is active; timeout to avoid dead loop
    s1 = h1.GetStatusBits_Bits(0);
    s2 = h2.GetStatusBits_Bits(0);
    if (IsMoving(s1) == 0)
      pause(2); % pause 2 seconds;
      h1.MoveHome(0,0);
      disp('Home Started!');
      break;
    end
    if (IsMoving(s2) == 0)
      pause(2); % pause 2 seconds;
      h2.MoveHome(0,0);
      disp('Home Started!');
      break;
    end
end
