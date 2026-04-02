:: A3 SCRIPT
echo off
echo '--------------------------------------------'
echo '-      ALGONQUIN COLLEGE - COM - 25S       -'
echo '--------------------------------------------'
echo '-                                          -'
echo '-    ====                                  -'
echo '-   =                                      -'
echo '-  =       ==   = =  ==  = =   =   ==  ==  -'
echo '-   =     =  = = = = = = = =   ==  =  ==   -'
echo '-    ====  ==  = = = =   = === === =   ==  -'
echo '-                                          -'
echo '--------------------------------------------'
echo '-  [A2: Parser - Team: Paulo Sousa / God]  -'
echo '--------------------------------------------'
set "arg=%1"
set "arg=%1"
if "%arg%"=="" set "arg=CODED.txt"
del compilers.exe CODED.txt
gcc compilers.c main1coder.c step1coder.c main2reader.c step2reader.c main3scanner.c step3scanner.c main4parser.c step4parser.c -o compilers
timeout /t 1 /nobreak >nul 
compilers 1 1 README.txt %arg%
timeout /t 1 /nobreak >nul
compilers 4 %arg% > out.txt 2> err.txt
timeout /t 1 /nobreak >nul
dir out.txt err.txt
type out.txt
