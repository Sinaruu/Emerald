:: A3 SCRIPT
echo off
echo '--------------------------------------------'
echo '-      ALGONQUIN COLLEGE - COM - 25F       -'
echo '--------------------------------------------'
echo '-                                          -'
echo '-    ====                                  -'
echo '-   =                                      -'
echo '-  =       ==   = =  ==  = =   =   ==  ==  -'
echo '-   =     =  = = = = = = = =   ==  =  ==   -'
echo '-    ====  ==  = = = =   = === === =   ==  -'
echo '-                                          -'
echo '--------------------------------------------'
echo '-  [A3: Scanner - Team: Egor / Hoang]      -'
echo '--------------------------------------------'
set "arg=%1"
set "param=2"
if "%arg%"=="" set "arg=CODED.txt"

gcc compilers.c main1coder.c step1coder.c main2reader.c step2reader.c main3scanner.c step3scanner.c -o compilers

timeout /t 1 /nobreak >nul

compilers 1 1 README.txt %arg%

timeout /t 1 /nobreak >nul

compilers 3 %arg% > out.txt 2> err.txt

timeout /t 1 /nobreak >nul

dir out.txt err.txt
type out.txt
