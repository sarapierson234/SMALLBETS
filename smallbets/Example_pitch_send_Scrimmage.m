%function answer
% Respond to SEND using memmapfile class.

disp('ANSWER server is awaiting message');

filename = fullfile('~/.scrimmage/logs/someText.txt');
% Create the communications file if it is not already there.
if ~exist(filename, 'file')
    [f, msg] = fopen(filename, 'wb');
    if f ~= -1
         fwrite(f, zeros(1,256), 'uint8');
        fclose(f);
    else
        error('MATLAB:demo:answer:cannotOpenFile', ...
              'Cannot open file "%s": %s.', filename, msg);
    end
end

% Memory map the file.
 m = memmapfile(filename, 'Writable', true, 'Format', 'int8');
tmp_vars=[];
tmp_t=[];
loop=1;
while loop
    % Wait until the first byte is not zero.
    tmp = char(m.Data);
    
    while tmp(1) == '0'
        %pause(.1);
        tmp = char(m.Data);
    end
    
    % The first byte now contains the length of the message.
    % Get it from m.
    %msg = char(m.Data(2:1+double(m.Data(1))))';
    
    % Display the message.
    %disp('Received message from SEND:')
    tmp_str=convertCharsToStrings(tmp(1:end));
    %disp(tmp_str);
    tmp_new=split(tmp_str,' ');
    tmp_new(1)=[];
    time=str2double(tmp_new(1));
    turn_rate=str2double(tmp_new(2));
    pitch_rate=str2double(tmp_new(3));
    roll_rate=str2double(tmp_new(4));
    speed=str2double(tmp_new(5));
    x=str2double(tmp_new(6));
    y=str2double(tmp_new(7));
    z=str2double(tmp_new(8));
    U=str2double(tmp_new(9));
    V=str2double(tmp_new(10));
    W=str2double(tmp_new(11));
    P=str2double(tmp_new(12));
    Q=str2double(tmp_new(13));
    R=str2double(tmp_new(14));
    roll=str2double(tmp_new(15));
    pitch=str2double(tmp_new(16));
    yaw=str2double(tmp_new(17));
    dxdt=str2double(tmp_new(18));
    dydt=str2double(tmp_new(19));
    dzdt=str2double(tmp_new(20));
    tmp_t=[tmp_t, time];
    tmp_vars=[tmp_vars;roll,roll_rate,pitch,pitch_rate,speed,x,y,z];
    figure(101);
    subplot(1,2,1)
    plot(tmp_t,tmp_vars(:,1),tmp_t,tmp_vars(:,2),tmp_t,tmp_vars(:,3),tmp_t,tmp_vars(:,4),tmp_t,tmp_vars(:,5));
    subplot(1,2,2)
    plot(tmp_t,tmp_vars(:,6),tmp_t,tmp_vars(:,7),tmp_t,tmp_vars(:,8))
    drawnow
    %text = std::to_string(turn_rate_) + "," + std::to_string(pitch_rate_) + "," + std::to_string(roll_rate_idx_) + "," + std::to_string(speed_)
    %    + "," + std::to_string(x_[Xw]) + "," + std::to_string(x_[Yw]) + "," + std::to_string(x_[Zw])
    %    + "," + std::to_string(x_[U]) + "," + std::to_string(x_[V]) + "," + std::to_string(x_[W])
    %    + "," + std::to_string(x_[P]) + "," + std::to_string(x_[Q]) + "," + std::to_string(x_[R])
    %    + "," + std::to_string(state_->quat().roll()) + "," + std::to_string(state_->quat().pitch()) + "," + std::to_string(state_->quat().yaw())
    %    + "," + std::to_string(state_->vel()(0)) + "," + std::to_string(state_->vel()(1)) + "," + std::to_string(state_->vel()(2));
    
    % Transform the message to send back to scrimmage (PSU Loop should go replace this).
    tmp_NEW=[num2str(time,'%.6f')];
    tmp_NEW=[tmp_NEW,' ',num2str(turn_rate,'%.6f')];
    tmp_NEW=[tmp_NEW,' ',num2str(pitch_rate,'%.6f')];
    tmp_NEW=[tmp_NEW,' ',num2str(roll_rate,'%.6f')];
    tmp_NEW=[tmp_NEW,' ',num2str(speed,'%.6f')];
    tmp_NEW=[tmp_NEW,' ',num2str(x,'%.6f')];
    tmp_NEW=[tmp_NEW,' ',num2str(y,'%.6f')];
    tmp_NEW=[tmp_NEW,' ',num2str(z,'%.6f')];
    tmp_NEW=[tmp_NEW,' ',num2str(U,'%.6f')];
    tmp_NEW=[tmp_NEW,' ',num2str(V,'%.6f')];
    tmp_NEW=[tmp_NEW,' ',num2str(W,'%.6f')];
    tmp_NEW=[tmp_NEW,' ',num2str(P,'%.6f')];
    tmp_NEW=[tmp_NEW,' ',num2str(Q,'%.6f')];
    tmp_NEW=[tmp_NEW,' ',num2str(R,'%.6f')];
    tmp_NEW=[tmp_NEW,' ',num2str(roll+45,'%.6f')];%Add 45 Degrees to Vehicle Pitch
    tmp_NEW=[tmp_NEW,' ',num2str(pitch,'%.6f')];
    tmp_NEW=[tmp_NEW,' ',num2str(yaw,'%.6f')];
    tmp_NEW=[tmp_NEW,' ',num2str(dxdt,'%.6f')];
    tmp_NEW=[tmp_NEW,' ',num2str(dydt,'%.6f')];
    tmp_NEW=[tmp_NEW,' ',num2str(dzdt,'%.6f')];
    
    for ii=1:length(tmp_NEW)
        m.Data(ii+2) = int8(tmp_NEW(ii));%Write to memMapped File
    end
   
    % Signal to SEND that the response is ready.
    m.Data(1) = int8('0');
    
    if strcmpi(tmp(1),'9') %%% Send stop command
        loop =0;
    end
end