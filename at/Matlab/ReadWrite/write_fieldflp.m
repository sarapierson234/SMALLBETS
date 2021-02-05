function write_fieldflp( flpfil, Option, Pos )

% Write a field-parameters file

if ( ~strcmp( flpfil( end - 3 : end ), '.flp' ) )
  flpfil = [ flpfil '.flp' ]; % append extension
end

fid = fopen( flpfil, 'wt' );

fprintf( fid, '/ ! Title' );
fprintf( fid, '''%4s'' ! Option', Option );
fprintf( fid, '999999   ! Mlimit (number of modes to include)' );
fprintf( fid, '1   ! NProf ' );
fprintf( fid, '0.0 /  ! rProf (km)' );

% receiver ranges
fprintf( fid, '%5i \t \t \t \t ! NRR', length( Pos.r.range ) );

if ( length( Pos.r.range ) > 2 && equally_spaced( Pos.r.range ) )
    fprintf( fid, '    %6f  ', Pos.r.range( 1 ), Pos.r.range( end ) );
else
    fprintf( fid, '    %6f  ', Pos.r.range );
end
fprintf( fid, '/ \t ! RR(1)  ... (km)' );

% source depths

fprintf( fid, '%5i \t \t \t \t ! NSD', length( Pos.s.depth ) );

if ( length( Pos.s.depth ) > 2 && equally_spaced( Pos.s.depth ) )
    fprintf( fid, '    %6f  ', Pos.s.depth( 1 ), Pos.s.depth( end ) );
else
    fprintf( fid, '    %6f  ', Pos.s.depth );
end

fprintf( fid, '/ \t ! SD(1)  ... (m)' );

% receiver depths

fprintf( fid, '%5i \t \t \t \t ! NRD', length( Pos.r.depth ) );

if ( length( Pos.r.depth ) > 2 && equally_spaced( Pos.r.depth ) )
    fprintf( fid, '    %6f  ', Pos.r.depth( 1 ), Pos.r.depth( end ) );
else
    fprintf( fid, '    %6f  ', Pos.r.depth );
end

fprintf( fid, '/ \t ! RD(1)  ... (m)' );

% receiver range offsets

fprintf( fid, '%5i \t \t ! NRR', length( Pos.r.depth ) );
fprintf( fid, '    %6.2f  ', zeros( 1, 2 ) );
fprintf( fid, '/ \t \t ! RR(1)  ... (m)' );

fclose( fid );
