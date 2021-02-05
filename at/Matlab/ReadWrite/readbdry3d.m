function [ xBot, yBot, zBot, NbdryPtsx, NbdryPtsy ] = readbdry3d( bdryfil )

% Read a boundary file (3D case)

%if ( strcmp( bdryfil, 'BTYFIL' ) == 0 && isempty( strfind( bdryfil, '.bty' ) ) )
%    bdryfil = [ bdryfil '.bty' ]; % append extension
%end

fid = fopen( bdryfil, 'rt' );
if ( fid == -1 )
    error( 'Bathymetry file does not exist' )
end

bdryType = fgetl( fid );

% Extract option letter between the quotes
nchars = strfind( bdryType, '''' );   % find quotes
bdryType = [ bdryType( nchars( 1 ) + 1 : nchars( 2 ) - 1 ) blanks( 2 - ( nchars( 2 ) - nchars( 1 ) ) ) ];

switch ( bdryType )
    case ( 'R' )
        disp( 'Piecewise-linear approximation to boundary' )
    case ( 'C' )
        disp( 'Curvilinear approximation to boundary' )
    otherwise
        fclose all;
        disp( bdryType )
        error( 'Fatal error: Unknown option for boundary type' )
end

%% x values
[ xBot, NbdryPtsx ] = readvector( fid )

% NbtyPtsx = fscanf( fid, '%i', 1 );
fprintf( 'Number of boundary points in x = %i \n\n', NbdryPtsx )
fprintf( ' x (km) \n' )
% 
% xBot = zeros( 1, NbtyPtsx );
% 
for ii = 1 : NbdryPtsx
%     xBot( ii ) = fscanf( fid, '%f', 1 );
   if ( ii == NbdryPtsx && ii > 51 )
      disp( '   ...' )
   end
   
   if ( ii < 51 || ii == NbdryPtsx  )   % echo up to 51 values
      fprintf( '%9.5g \n', xBot( ii ) );
   end
end

%xlimits = fscanf( fid, '%f ', 2 )
%x = linspace( xlimits( 1 ), xlimits( 2 ), NbtyPtsx );
%fgetl( fid );

%% y values
[ yBot, NbdryPtsy ] = readvector( fid );

% NbtyPtsy = fscanf( fid, '%i', 1 );
fprintf( 'Number of boundary points in y = %i \n\n', NbdryPtsy )
fprintf( ' y (km) \n' )
% 
% yBot = zeros( 1, NbtyPtsy );
% 
for ii = 1 : NbdryPtsy
%     yBot( ii ) = fscanf( fid, '%f', 1 );
   if ( ii < 50 || ii == NbdryPtsy )   % echo up to 51 values
         fprintf( '%9.5g \n', yBot( ii ) );
   end
end

%ylimits = fscanf( fid, '%f ', 2 );
%y = linspace( ylimits( 1 ), ylimits( 2 ), NbtyPtsy );
%fgetl( fid );

zBot = fscanf( fid, '%f ', [ NbdryPtsx, NbdryPtsy ] );
zBot = zBot';

fclose( fid );  % close the boundary file
