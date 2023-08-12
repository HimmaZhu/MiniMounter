clear;
addpath('C:\Users\Dell\Documents\MATLAB\mexopencv');
% enable_MrScope();
rate=10;
a=ConnectToCam();
b=SendInitCommands();
c=testCommand1(0,ledcommand(0));%LED
d=testCommand(gaincommand(64));%GAIN
e=testCommand1(frameratecommand1(rate),frameratecommand2(rate));
if a*b*c*d*e==0
    fprintf('camera failed to initialized\n');
    pause;
end
cam =   (0);
cam
width = 752;
height = 480;
% cam.set(3,width);
% cam.set(4,height);
if ~cam.isOpened()               % check if we succeeded
    fprintf('camera failed to initialized\n');
    pause;
else
   fprintf('camera initialized successfully\n');
end
video = cv.VideoWriter();
filename = 'test.avi';
% video.open(filename,frameSize,'FourCC','MJPG','FPS',60); 
video.open(filename,[width,height],'FourCC','MJPG','FPS',60); 
while 1
    tStart =tic;
%     img1=cam.read();
%     imshow(img1);
    img2=cam.read();
    imshow(img2);
    %video.write(img);
    pause(0.0001); %必须要有这个， 要不然程序可能无法得到你的键盘输入
    if (get(gcf,'CurrentCharacter') > 0)
        break;
    end
    tEnd=toc(tStart);
    fprintf(strcat("FPS: ",num2str(1/tEnd),"\n"));
end
cam.release();
close all;
fprintf('camera closed successfully\n');

%video
%video.release();
fprintf('video closed successfully\n');
% disable_MrScope();
% 
% 
% t = timer('TimerFcn', 'stat=false; disp(''Timer!'')',... 
%                  'StartDelay',1);
% start(t)
% 
% stat=true;
% while 1
% if(stat==true)
%   disp('.')
%   pause(1)
%   else
%     break;
% end
% 
% end
% tic
% cam.retrieve();
% toc
% tic
% cam.read();
% toc