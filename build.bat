@rem ECHO off

@rem Build Process:
@rem 1. Clean the package directory
@rem 2. Make package directory (Package)
@rem 3. Build the target. Release x64 by default.
@rem 4. From output Directory:
@rem     Copy .dll files
@rem     Copy RetroGraph.exe
@rem 5. Copy resources to output dir.
@rem     Rename default_config.ini to config.ini

@SET PKGDIR=Package\
@SET RSRCDIR=%PKGDIR%resources\
@SET BINDIR=%PKGDIR%bin\x64\
@SET DEVENVCOM="C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.com"

@IF [%1]==[rebuild] GOTO :build
@SET ACTION=-build
GOTO :start_build

:build
@SET ACTION=-rebuild
GOTO :start_build

:start_build
@ECHO.
@ECHO ========================================================================
@ECHO Compiling
@ECHO ========================================================================

@rem Compile project.
%DEVENVCOM% RetroGraph.sln %ACTION% "Release|x64" || goto :error

@ECHO.
@ECHO ========================================================================
@ECHO Copying bin directory files
@ECHO ========================================================================

@RMDIR /S /Q "Package"
@MKDIR "Package" || goto :error

@COPY "RetroGraph\InstallNotes.txt" %PKGDIR% >NUL || goto :error

@XCOPY /Q "bin\Releasex64\freeglut.dll" %BINDIR% >NUL || goto :error
@XCOPY /Q "bin\Releasex64\glew32.dll" %BINDIR% >NUL || goto :error
@XCOPY /Q "bin\Releasex64\GetCoreTempInfo.dll" %BINDIR% >NUL || goto :error
@XCOPY /Q "bin\Releasex64\RetroGraph.exe" %BINDIR% >NUL || goto :error
@XCOPY /Q "bin\Releasex64\RetroGraphDLL.dll" %BINDIR% >NUL || goto :error

@ECHO.
@ECHO ========================================================================
@ECHO Copying resource directory files
@ECHO ========================================================================

@XCOPY /Q /E /I "RetroGraph\resources\fonts" "Package\RetroGraph\resources\fonts" >NUL || goto :error
@XCOPY /Q /E /I "RetroGraph\resources\shaders" "Package\RetroGraph\resources\shaders" >NUL || goto :error
@COPY  "RetroGraph\resources\default_config.ini" "Package\RetroGraph\resources\config.ini" >NUL || goto :error

:success
@ECHO.
@ECHO ========================================================================
@ECHO Successfully built package.
@ECHO ========================================================================
EXIT /B 0

:error
@ECHO.
@ECHO ========================================================================
ECHO "Failed to package. Exiting."
@ECHO ========================================================================
EXIT /B %errorlevel%

PAUSE
