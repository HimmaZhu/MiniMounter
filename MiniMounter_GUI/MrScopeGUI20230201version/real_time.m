%function rawdataimshow()
f1 = figure('Name','MrScope real-time capture');
f1.Position = [39 39 1100 600];

x_axis=zeros(480,752);
y_axis=zeros(480,752);
r=zeros(480,752);
for i=1:480
    for j=1:752
        x_axis(i,j)=i;
        y_axis(i,j)=j;
    end
end
for i=1:480
    for j=1:752
        r(i,j)=sqrt((x_axis(i,j)-241)^2+(y_axis(i,j)-377)^2);
        
    end
end
r=round(r);
se = strel('diamond',3);
H=[];
SP=[];
M=[];
invSTD2=[];
p=[1:2*pi/(752E-6):2*pi/(752E-6)*447];
%p=[1:1:447];
p1=log(p);
% 
 while 1
    %% frame
    img = rgb2gray(imread(".\settings\1.tif"));
    load(".\settings\color_map.mat");
    load(".\settings\low.mat");
    load(".\settings\up.mat");
    load(".\settings\threshold_en.mat");
    load(".\settings\index_clear.mat");
    
    if index_clear
        index_clear = 0;
        save("./settings/index_clear.mat","index_clear");
        H=[];
        SP=[];
        M=[];
        invSTD2=[];
    end

    %% index
    img3 = img;img2 = img;
    h = histogram(img3,256);
    counts = h.Values;
    
    sum_counts = cumsum(counts);
    [~,threshold] = min(abs(sum_counts-0.5*480*752));
    mask = imbinarize(img3, threshold/255);
    mask1 = imerode(mask,se);
    mask1 = imdilate(mask1,se);
    
    img2=double(mask1).*double(img2);
    H=[H entropy(img2)/20*mean2(img)];

     subplot(3,4,4),
     plot(H);
     xlabel("frame number");
     ylabel("Entropy of frame");
    % spectrum
    S=[];
    fft_I=fft2(img);
    fft_shift=fftshift(fft_I);
    fft_shift=abs(fft_shift);
%     T=log(fft_shift+1);
    Spectrum=fft_shift.^2;
    for i=1:447
        [x,y]=find(r==(i-1));
        sum1=0;
        for j=1:length(x)
            sum1=sum1+Spectrum(x(j),y(j));
        end
        S=[S sum1/length(x)];
    end
    
    S=log(S+1);
    %invSTD2=[invSTD2 std(S(floor(0.4*size(S,2)):end))];
    M = [M mean(S(floor(0.4*size(S,2)):end))];
    S1=S/max(S);
    SP=[SP S1*p1'];
    
    subplot(3,4,8),
    plot(SP);
    xlabel("frame number");
    ylabel("Spectrum analysis of frame");
    subplot(3,4,12),
    plot(M);
    xlabel("frame number");
    ylabel("STD of Spectrum");
    
    subplot(3,4,[1:3,5:7,9:11]),
    imagesc(img,[low up]);
    colorbar;
    colormap(f1,color_map);
    %%
    pause(0.01); %必须要有这个， 要不然程序可能无法得到你的键盘输入
    if (get(gcf,'CurrentCharacter') > 0)
        break;
    end
end
