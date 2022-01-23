%Sara Pierson, 
%Georgia Institute of Technology, Spring 2021
%the purpose of this file is to create a model .ssp type file of the sound
%speed profile (based on the temperature profile)
%We want to be as resolute as possible, but to begin will start with very
%simple models. 

%% Building BELLHOP .ssp file
%%the two lines below import a xlsx file (excel file) with columns 1 and 2
%%filled with depth and temperature data of interest. type in the name of
%%the file in the xlsread() function

orig_depth_data = xlsread('europa_vars.xlsx', 'A:A');
orig_temp_k_data = xlsread('europa_vars.xlsx', 'B:B');

x_step = 5; %meters (resolution in your lateral direction)
x_end = 500; %meters (total length of lateral direction)
    x_vec = 0:x_step:x_end;
y_step = 250; %meters (resolution in your longitudinal/depth direction)
y_end = 15000; %meters (total depth)

depth = 0:y_step:y_end;

fs = '%6.2f\t'; %format spec
fid = fopen('sspenv_ex', 'w');
fprintf(fid, '%6f', length(x_vec)); %for the bellhop ssp this must come first, this is the number of columns in the ssp
    %NOTE it is WILDLY IMPORTANT that the above value not have any decimal
    %places in it--which is why it is not using the 'fs' variable listed above.
    %otherwise you will get a fortran error
fprintf(fid, '\n');
fprintf(fid, fs, x_vec); %second mandatory line in ssp specifying the x vector
fprintf(fid, '\n');

temp_k = [];
ssp_vec = [];
for i = 1:length(depth)
    
    T_ambient_K = interp1(orig_depth_data, orig_temp_k_data, depth(i));
    %ambient temp profile in KELVIN
    temp_k = [temp_k T_ambient_K];
    speedofsound = -2.3*(T_ambient_K-273) + 3795; %this is speed of sound in ICE. 
        %CHANGE THIS if you are look at the speed of sound in anther medium
    ssp_vec = [ssp_vec speedofsound];
    ssp = speedofsound*ones(1, length(x_vec));
    fprintf(fid, fs, ssp);
    fprintf(fid, '\n');
end 
fclose(fid);

%% Building BELLHOP .env 
%a code for writing the bellhop .env file since otherwise it would take
%forever to manually input all of the vectors. 

fid = fopen('sspenv_ex.env', 'w')
fs = '%6.2f\n';
%fprintf(fid, '%s\n', '''EXAMPLE NAME: ssp ray trace'''); %line 1 in
%bellhop .env file describing the purpose of your data reduction. 

    prompt = 'What frequency is your source?';
    frequency = input(prompt)
fprintf(fid, fs, frequency); %line 2 
fprintf(fid, '%1f', 1); %line 3 dummy variable -- it is very important that this is just an integer with no decimals
                        %otherwise it just doesnt work for some reason
    
    options1vec = '''QVM''';
    %eventually we will want this to be an input prompt
fprintf(fid, '%s\n', options1vec) %line 4, stands for: quadratic interpoloation (uses the ssp), v:vaccum above surface, m: dB/m. you can change this to suit your needs, see bellhop manual
                                  % bellhop manual: https://oalib-acoustics.org/AcousticsToolbox/Bellhop-2010-1.pdf
    
    top_vec = [length(ssp_vec) x_vec(1) depth(end)]; %line5 topline vector: length of ssp_vector, starting depth, ending depth
fprintf(fid, '%6f\t %6f\t %6.2f\t \n', top_vec);    
%important that the first two numbers printed in the line above are
%INTEGERS (I know, this is annoying. Idk why this is the case but if there
%are decimals there, fortran wont run. the last number in that list can
%have decimals. dont ask me why. The internet doesnt seem to know either)
fprintf(fid, '%6.2f\t %6.2f\t %6.2f\t %6.2f\t %6.2f\t %6.2f\t \n ', [depth; ssp_vec; zeros(61,1)'; ones(61,1)'; zeros(61,1)'; zeros(61,1)']); %depth, ssp(longitudinal) m/s, ssp(lateral) m/s, denisty(g/cm^3), ssp_s_long, ssp_s_lat 
%note that the ssp long and lat are technically p-waves and s-waves,
%respectively
%last two long and lat ssp values, if they are not included in the vector then Fortran compiles all the vector columns weird
%so even if you dont know what they should be, fill in with zeros and ones
%as above

    options2vec_2 = [depth(end) max(ssp) 0.0 1.0 0.5 0.0]; % 1600 0.5 0.0
                             %[maxDepth, p wave speed, shear wave speed,
                             %density of bottom, p wave attenuation, s wave
                             %attenuation]
fprintf(fid, '%s\t %6.2f\t\n', '''R''', 0.0); %eventually we want this to be 'F', 
                                           %the list of reflection coeffs
                                           %in the *.brc --if you dont know
                                           %what this is, check the bellhop
                                           %manual. it is not that long :)
fprintf(fid, '%6.2f\t %6.2f\t %6.2f\t %6.2f\t %6.2f\t %6.2f\t \n', options2vec_2);
    num_sources = 1;%this should also be a prompt later
    depth_source = 5000; %depth of your source in meters
fprintf(fid, fs, num_sources);
fprintf(fid, fs, depth_source);
    num_receiver = 1;
    depth_receiver = 1000;
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
    rstep_zmax_rmax = [0 15000 4]; %deltas(m) box size in depth direction (m) box size in range direction (km)
fprintf(fid, '%6.2f\t %6.2f\t %6.2f\n', rstep_zmax_rmax);

fclose(fid);