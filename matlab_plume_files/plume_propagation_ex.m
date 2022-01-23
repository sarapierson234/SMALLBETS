% Sara Pierson
% SMALLBETS 
% Georgia Institute of Technology, Summer 2021
% Static plume code with "2D slice" parameters equal to those found in a
% typical scrimmage smallbets-v1.xml run --  
% I took one of the OceanParam.csv files created during the simulation loop
% to create this 2d view here. 

%x axis: [-2.05 to 7.94] --> 101 entries ~10 meters
%y axis: [1.50 to 25.03] --> 101 entries ~25 meters

%in the absence of cross-currents ALL BUOYANT PLUMES spread at the rate:
%tan(theta) = 0.11; where theta = 6.3 degrees. 
%Given our x axis is ~10 m and our y ~25 m I want our first static plume to
%be around 5m in diameter, 2.5m radius. therefore:
opp = 2.5; %m
rad = opp; %radius 
%adj = ?
theta = 6.3; %degrees
adj = opp/tand(theta); %m

%now that we know the height and diameter/radius of the plume, we can
%populate the area with the other information we know -- which is that a
%plume in a static environment has a gaussian concentration distribution 

%typical oceanic salinity: 35.2 g/kg 
%since our resolution is .01 m^3, (kinda), each pixel is ~10kg 
%which would have something around 352 g 
%we will keep this number in mind for the concentration max below. 
%ALTERNATIVELY
%we can consider the c_max the temperature since the first task was to do a
%temperature or "hot blob" bellhop reduction. 
%based on the sound speed, which we import from the OceanParam.csv we know
%the basic temperature distribution of the water column is ~22*C to a depth
%of 200m, we will use this value for now. 

%% all the equations below come from EFM laws-- found in:
%% Mixing in Inland and Coastal Waters - Fisher 1978
lambda = 1.2; %entrainment coefficient, empirically determined. this is true for all 2D plumes for 3d plumes it is 1.19
z = adj; %m, this is the distance from the nozzel from which the plume originates
b_g = 0.105*z; %m, empirical equation
c_max = 25; %this number (max concentration) is of our choosing, in an in-situ scenario we would measure this
%in this example, c_max is 25*celcius temperature 
conc_x_r = c_max*exp(-(rad^2/(lambda*b_g)));

%we can make a gaussian distribution in 2 ways.
% 1. use the equation above (conc_x_r) and make a half-vector and mirror it 
% 2. use the gaussian distrbution equation. 
%   a. notes on Gaussian (aka. Normal) dist equ:
%    mu: expected value
%    variance == sigma_sqrd: distribution (when accounting for both sides
%    of the bell curve) within this bound accounts for 60% of all density

%we are going to generate the entire ssp needed for bellhop by using the
%gaussian dist equ and 2 for loops 
x_axis = 10; %meters, approximate
y_axis = 25; %meters, approximate
slice_size = 101; %see above for this number and the two above it

ambient_temp = 22; %deg C
max_temp = 25; %deg C

vec = ones(1,101)*ambient_temp;
x_axis = [0:.1:100];
mu = ambient_temp/max_temp; %bc we need the vec above tohave a max temp 
%normal dist of 25C
var_sqrd = linspace(5,.1,101);
%ssp = zeros(101,101);
for j = 1:length(var_sqrd);
    %for reducing size of gauss curve with y-axis
    varience_sqrd = var_sqrd(j);
    %varience = sqrt(varience_sqrd);
    %norm_vec = linspace(1,.5333,101);
    for i = 1:length(x_axis); 
         %populating x-axis rows with normalized gauss curve mu
         %multiplication values
         %gauss_vec(j,i) = (1/(varience*sqrt(2*pi)))*exp(- ((x_axis(i)-0)^2/(2*varience_sqrd)) );
         %this way of doing it introduced a lot of uncertainty and
         %unexpected amplitude shifts for some reason. I tried debugging it
         %but instead found an easier way -- gaussmf functionality (you
         %need the Fuzzy Logic toolbox to run it)
         y_axis = gaussmf(x_axis, [varience_sqrd, 50]);
    end
    tempC(j,:) = y_axis;%(y_axis*2 +22);
end

for i = 1:length(x_axis)
    plot(tempC(i,:));
    hold on
end 

%% Building BELLHOP .ssp file

x_step = .1; %meters (resolution in your lateral direction)
x_end = 10; %meters (total length of lateral direction)
    x_vec = 0:x_step:x_end;
y_step = 1; %meters (resolution in your longitudinal/depth direction)
y_end = 101; %meters (total depth)

depth = linspace(0,999,101);

fs = '%6.2f\t'; %format spec
fid = fopen('scrim_plume.ssp', 'w');
fprintf(fid, '%6.0f', length(x_axis)); %for the bellhop ssp this must come first, this is the number of columns in the ssp
    %NOTE it is WILDLY IMPORTANT that the above value not have any decimal
    %places in it--which is why it is not using the 'fs' variable listed above.
    %otherwise you will get a fortran error
fprintf(fid, '\n');
fprintf(fid, fs, x_axis); %second mandatory line in ssp specifying the x vector
fprintf(fid, '\n');

a1 = 1448.98; a2 = 4.591; a3 = -.05304; a4 = .0002374; a5 = 1.34; 
a6 = .0163; a7 = 1.675*10^-7; a8 = -.01025; a9 = -7.139*10^-13;
salinity = 36; %ppt
ssp = zeros(101,101);
ssp_vec = [];

for jj = 1:length(depth)
    for ii = 1:length(x_axis)

        speedofsound = a1 + a2*tempC(jj,ii) + a3*tempC(jj,ii)^2 + a4*tempC(jj,ii)^3 ...
            + a5*(salinity-35) + a6*depth(jj) + a7*depth(jj)^2 + ...
            + a8*tempC(jj,ii)*(salinity-35) + a9*tempC(jj,ii)*depth(jj)^3;
        %this is speed of sound in saltwater. 
            %CHANGE THIS if you are look at the speed of sound in anther medium
        ssp_vec = [ssp_vec speedofsound];
    end 
    fprintf(fid, fs, ssp_vec);
    fprintf(fid, '\n');
    ssp(jj,:) = [ssp_vec];
    ssp_vec = [];
end
fclose(fid);

%% Building BELLHOP .env 
%a code for writing the bellhop .env file since otherwise it would take
%forever to manually input all of the vectors. 

fid = fopen('scrim_plume.env', 'w')
fs = '%6.2f\n';
fprintf(fid, '%s\n', '''scrimmage plume example ray trace 1'''); %line 1 in
%bellhop .env file describing the purpose of your data reduction. 

    prompt = 'What frequency is your source?';
    frequency = input(prompt)
fprintf(fid, fs, frequency); %line 2 
fprintf(fid, '%1.0f \n', 1); %line 3 dummy variable -- it is very important that this is just an integer with no decimals
                        %otherwise it just doesnt work for some reason
    
    options1vec = '''QVM''';
    %eventually we will want this to be an input prompt
fprintf(fid, '%s\n', options1vec) %line 4, stands for: quadratic interpoloation (uses the ssp), v:vaccum above surface, m: dB/m. you can change this to suit your needs, see bellhop manual
                                  % bellhop manual: https://oalib-acoustics.org/AcousticsToolbox/Bellhop-2010-1.pdf
    
    top_vec = [length(ssp) depth(1) depth(end)]; %line5 topline vector: length of ssp_vector, starting depth, ending depth
fprintf(fid, '%6.0f\t %6.0f\t %6.2f\t \n', top_vec);    
%important that the first two numbers printed in the line above are
%INTEGERS (I know, this is annoying. Idk why this is the case but if there
%are decimals there, fortran wont run. the last number in that list can
%have decimals. dont ask me why. The internet doesnt seem to know either)
fprintf(fid, '%6.2f\t %6.2f\t %6.2f\t %6.2f\t %6.2f\t %6.2f\t \n ', [depth; ssp(:,51)'; zeros(length(ssp),1)'; ones(length(ssp),1)'; zeros(length(ssp),1)'; zeros(length(ssp),1)']); %depth, ssp(longitudinal) m/s, ssp(lateral) m/s, denisty(g/cm^3), ssp_s_long, ssp_s_lat 
%note that the ssp long and lat are technically p-waves and s-waves,
%respectively
%last two long and lat ssp values, if they are not included in the vector then Fortran compiles all the vector columns weird
%so even if you dont know what they should be, fill in with zeros and ones
%as above

    options2vec_2 = [max(depth) max(ssp(51,:)) 0.0 1.0 0.5 0.0]; % 1600 0.5 0.0
                             %[maxDepth, p wave speed, shear wave speed,
                             %density of bottom, p wave attenuation, s wave
                             %attenuation]
fprintf(fid, '%s\t %6.2f\t\n', '''R''', 0.0); %eventually we want this to be 'F', 
                                           %the list of reflection coeffs
                                           %in the *.brc --if you dont know
                                           %what this is, check the bellhop
                                           %manual. it is not that long :)
                                           %note when you use R or V you do
                                           %not need nor should you include
                                           %the options2vec_2 line. Just
                                           %delete it
fprintf(fid, '%6.2f\t %6.2f\t %6.2f\t %6.2f\t %6.2f\t %6.2f\t \n', options2vec_2);
    num_sources = 1;%this should also be a prompt later
    depth_source = max(depth)/2; %depth of your source in meters
fprintf(fid, fs, num_sources);
fprintf(fid, fs, depth_source);
    num_receiver = 1;
    depth_receiver = max(depth)/2;
    num_rx_range = 1; %this is how many rxs you have in the x direction. in our case we dont have any
fprintf(fid, fs, num_receiver);
fprintf(fid, fs, depth_receiver);
fprintf(fid, fs, num_rx_range);
    options3vec = '''E'''; %amplitude and travel times is A. Eigen rays are 'E' and show you which rays actually reach your rx from your sx
    launching_angles = 1; %same as your number of receivers in the lateral direction
    launch_degrees = [-14.0 14.0];
fprintf(fid, '%s\n', options3vec);
fprintf(fid, fs, launching_angles);
fprintf(fid, '%6.2f\t %6.2f\t / \n', launch_degrees); 
    %the forward slash is super important. without it the code wont run. 
    rstep_zmax_rmax = [0 max(depth) max(x_axis)]; %deltas(m), box size in depth direction (m) box size in range direction (km)
fprintf(fid, '%6.2f\t %6.2f\t %6.2f\n', rstep_zmax_rmax);

fclose(fid);        
