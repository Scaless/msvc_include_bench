@echo off

cd /d %~dp0

echo Calling vcvarsall
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

SET SKIPTESTS=0

FOR %%A IN (%*) DO (
    IF "%%A"=="/analyzeonly" (
		ECHO /analyzeonly set, skipping tests
		SET SKIPTESTS=1
	)
)

IF %SKIPTESTS%==1 GOTO analyze

echo Setting Up Test Directories
rmdir /s /q build
mkdir build
rmdir /s /q temp
mkdir temp

echo Building Test Framework
del /s test_builder.exe
call cl /nologo /EHsc /O2 /std:c++latest /Fo:build/ test_builder.cpp

echo Creating Test Files
call test_builder

echo Running Tests
pushd temp
FOR /D %%G in ("*") DO (
    echo %%G
    
    REM Run the compiler once to warm up the file cache
    call cl /nologo /MD /EHsc /std:c++latest /Fo:.\..\build\ /Fe:.\%%G\test_cpplatest.exe /d1reportTime %%G\test.cpp > NUL
    
    REM Run d1reportTime
    call cl /nologo /MD /EHsc /std:c++latest /Fo:.\..\build\ /Fe:.\%%G\test_cpplatest.exe /d1reportTime %%G\test.cpp > %%G\d1report_cpplatest.txt
    call cl /nologo /MD /EHsc /std:c++20     /Fo:.\..\build\ /Fe:.\%%G\test_cpp20.exe     /d1reportTime %%G\test.cpp > %%G\d1report_cpp20.txt
    call cl /nologo /MD /EHsc /std:c++17     /Fo:.\..\build\ /Fe:.\%%G\test_cpp17.exe     /d1reportTime %%G\test.cpp > %%G\d1report_cpp17.txt
    call cl /nologo /MD /EHsc /std:c++14     /Fo:.\..\build\ /Fe:.\%%G\test_cpp14.exe     /d1reportTime %%G\test.cpp > %%G\d1report_cpp14.txt
    
    REM Expand headers into compilation unit - with and without comments
    call cl /nologo /MD /EHsc /std:c++latest /EP /C %%G\test.cpp > %%G\expanded_comments_cpplatest.cpp
    call cl /nologo /MD /EHsc /std:c++20     /EP /C %%G\test.cpp > %%G\expanded_comments_cpp20.cpp
    call cl /nologo /MD /EHsc /std:c++17     /EP /C %%G\test.cpp > %%G\expanded_comments_cpp17.cpp
    call cl /nologo /MD /EHsc /std:c++14     /EP /C %%G\test.cpp > %%G\expanded_comments_cpp14.cpp
    
    call cl /nologo /MD /EHsc /std:c++latest /EP %%G\test.cpp > %%G\expanded_no_comments_cpplatest.cpp
    call cl /nologo /MD /EHsc /std:c++20     /EP %%G\test.cpp > %%G\expanded_no_comments_cpp20.cpp
    call cl /nologo /MD /EHsc /std:c++17     /EP %%G\test.cpp > %%G\expanded_no_comments_cpp17.cpp
    call cl /nologo /MD /EHsc /std:c++14     /EP %%G\test.cpp > %%G\expanded_no_comments_cpp14.cpp
    
    REM /Bt+ provides the full frontend/backend timings, but isn't that useful
    REM call cl /nologo /EHsc /std:c++latest /Fo:.\..\build\ /Fe:.\%%G\test.exe /Bt+ %%G\test.cpp > %%G\compile_log.txt
    
    REM d2cgsummary does not provide any relevant info for our purposes
    REM call cl /nologo /EHsc /std:c++latest /Fo:.\..\build\ /Fe:.\%%G\test.exe /d2cgsummary %%G\test.cpp > %%G\d2report.txt
)
popd

:analyze

echo Setting Up Results Directory
rmdir /s /q results
mkdir results

echo Building Test Analyzer
del /s test_analyze.exe
call cl /nologo /EHsc /O2 /std:c++latest /Fo:build/ test_analyze.cpp

echo Analyzing Tests
call test_analyze

echo Done
pause
