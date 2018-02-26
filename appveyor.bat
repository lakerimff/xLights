echo on

rem dir "\program files (x86)\windows kits\10\bin\"

dir c:\

dir c:\mingw

dir C:\mingw-w64

set "xlightsdir=%cd%"

rem git clone -q --branch=master https://github.com/wxWidgets/wxWidgets.git \projects\wxWidgets
7z x -o\projects\wxWidgets wxWidgets-3.1.1.zip
if %ERRORLEVEL% NEQ 0 exit 1

rem dir \projects\wxWidgets

sed -i '#define wxUSE_STD_STRING_CONV_IN_WXSTRING wxUSE_STL/#define wxUSE_STD_STRING_CONV_IN_WXSTRING 1/g' \projects\wxWidgets\include\wx\msw\setup.h
if %ERRORLEVEL% NEQ 0 exit 1

sed -i '#define wxUSE_STD_CONTAINERS 0/#define wxUSE_STD_CONTAINERS 1/g' \projects\wxWidgets\include\wx\msw\setup.h
if %ERRORLEVEL% NEQ 0 exit 1

is.exe /LOG=inno.log /SUPPRESSMSGBOXES /SP- /NORESTART /NOCLOSEAPPLICATIONS /DIR=\projects\inno /NOICONS /SILENT
if %ERRORLEVEL% NEQ 0 exit 1

type inno.log

dir \projects\inno

if [%BUILDTYPE%] EQU [X64DEBUGVS] goto x64DebugVS 
if [%BUILDTYPE%] EQU [X64RELEASEGCC] goto x64ReleaseGCC 
if [%BUILDTYPE%] EQU [X86RELEASEGCC] goto x86ReleaseGCC 

echo "Unknown build type " %BUILDTYPE%

exit 1

rem =========================================== 64 BIT MSVC ===========================================
:x64DebugVS

rem prepare the header files for wxWidgets
cd ..\wxWidgets\build\msw
msbuild /m wx_custom_build.vcxproj /p:PlatformToolset=v%PLATFORMTOOLSET% /p:Configuration="Release"
if %ERRORLEVEL% NEQ 0 exit 1

rem Build wxWidgets
msbuild /m wx_vc14.sln /p:PlatformToolset=v%PLATFORMTOOLSET%
if %ERRORLEVEL% NEQ 0 exit 1

cd %xlightsdir%

cd xLights

sed -i "s/10.0.15063.0/10.0.14393.0/g" Xlights.vcxproj

msbuild /m xLights.sln /p:PlatformToolset=v%PLATFORMTOOLSET%
if %ERRORLEVEL% NEQ 0 exit 1

cd ..

cd xSchedule

sed -i "s/10.0.15063.0/10.0.14393.0/g" xSchedule.vcxproj

msbuild /m xSchedule.sln /p:PlatformToolset=v%PLATFORMTOOLSET%
if %ERRORLEVEL% NEQ 0 exit 1

exit 0

rem =========================================== 32 BIT GCC ===========================================
:x86ReleaseGCC

rem This has to be cmd.exe because the makefile uses \ in paths and bash and sh dont understand them
set COMSPEC=cmd.exe
set MINGWPATH=C:\mingw\i686-7.2.0-posix-seh-rt_v5-rev1\mingw32\bin
set PATH=%MINGWPATH%;%PATH%

rem set

cd ..\wxWidgets\build\msw

rem make the header files
mingw32-make setup_h -f makefile.gcc --debug MONOLITHIC=1 SHARED=1 UNICODE=1 CXXFLAGS="-std=gnu++14" BUILD=release SHELL=%COMSPEC%

cd %xlightsdir%

7z e cbp2make-stl-rev147-all.tar.7z -o. cbp2make-stl-rev147-all\bin\Release\cbp2make.exe
if %ERRORLEVEL% NEQ 0 exit 1

cd xlights

..\cbp2make.exe -in xLights.cbp -out xLights.cbp.mak --with-deps --keep-outdir --keep-objdir -windows -targets MinGW_Release
if %ERRORLEVEL% NEQ 0 exit 1

rem sed -i "s/-l\.\./../g" xLights.cbp.mak
rem if %ERRORLEVEL% NEQ 0 exit 1

mingw32-make -f xLights.cbp.mak CXXFLAGS="-std=gnu++14" WX=\projects\wxWidgets INC=-I\projects\wxWidgets\include -j 10 mingw_release
if %ERRORLEVEL% NEQ 0 exit 1

cd ..\xSchedule

..\cbp2make.exe -in xSchedule.cbp -out xSchedule.cbp.mak --with-deps --keep-outdir --keep-objdir -windows -targets MinGW_Release
if %ERRORLEVEL% NEQ 0 exit 1

mingw32-make -f xSchedule.cbp.mak CXXFLAGS="-std=gnu++14" WX=\projects\wxWidgets INC=-I\projects\wxWidgets\include -j 10 mingw_release
if %ERRORLEVEL% NEQ 0 exit 1

cd ..

rem build the install file
\projects\inno\iscc.exe "xLights_4_32bit.iss"

exit 0

rem =========================================== 64 BIT GCC ===========================================
:x64ReleaseGCC

set COMSPEC=C:\Windows\system32\cmd.exe
set MINGWPATH=C:\mingw-w64\x86_64-6.3.0-posix-seh-rt_v5-rev1\mingw64\bin
set PATH=%PATH%;%MINGWPATH%

cd ..\wxWidgets\build\msw

rem make the header files
mingw32-make setup_h -f makefile.gcc --debug TARGET_CPU=X64 MONOLITHIC=1 SHARED=1 UNICODE=1 CXXFLAGS="-std=gnu++14" BUILD=release SHELL=%COMSPEC%

rem build wxWidgets
mingw32-make -f makefile.gcc --debug TARGET_CPU=X64 MONOLITHIC=1 SHARED=1 UNICODE=1 CXXFLAGS="-std=gnu++14" BUILD=release -j 10 SHELL=%COMSPEC%

if %ERRORLEVEL% NEQ 0 exit 1

cd %xlightsdir%

7z e cbp2make-stl-rev147-all.tar.7z -o. cbp2make-stl-rev147-all\bin\Release\cbp2make.exe
if %ERRORLEVEL% NEQ 0 exit 1

cd xlights

sed -i "s/gnu_gcc_64bit_compiler/gcc/g" xLights.cbp

..\cbp2make.exe -in xLights.cbp -out xLights.cbp.mak --with-deps --keep-outdir --keep-objdir -windows -targets "64bit  MinGW_Release"
if %ERRORLEVEL% NEQ 0 exit 1

sed -i "s/-l\.\./../g" xLights.cbp.mak
if %ERRORLEVEL% NEQ 0 exit 1

mingw32-make -f xLights.cbp.mak CXXFLAGS="-std=gnu++14" WX=\projects\wxWidgets INC=-I\projects\wxWidgets\include -j 10 64bit__mingw_release
if %ERRORLEVEL% NEQ 0 exit 1

cd ..\xSchedule

..\cbp2make.exe -in xSchedule.cbp -out xSchedule.cbp.mak --with-deps --keep-outdir --keep-objdir -windows -targets "64bit  MinGW_Release"
if %ERRORLEVEL% NEQ 0 exit 1

mingw32-make -f xSchedule.cbp.mak CXXFLAGS="-std=gnu++14" WX=\projects\wxWidgets INC=-I\projects\wxWidgets\include -j 10 64bit__mingw_release
if %ERRORLEVEL% NEQ 0 exit 1

cd ..

rem build the install file
\projects\inno\iscc.exe "xLights_4_64bit.iss"

dir

exit 0
