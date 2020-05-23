@echo off
REM Settings-------------------------------------------------------------------------------------------------
set ExeName=glfw_touch_win
set MainCode=glfw_touch_win.cpp

set Architecture=x64
REM ---------------------------------------------------------------------------------------------------------

REM * Linking *
REM /MTd	Creates a debug multithreaded executable file using LIBCMTD.lib.
set FlagsLinking=/MTd

REM * Optimization *
REM /Od	Disables optimization.
REM /Oi	Generates intrinsic functions.
set FlagsOptimization=/Od /Oi

REM * Miscellaneous *
REM /FC	Display full path of source code files passed to cl.exe in diagnostic text.
REM /nologo	Suppresses display of sign-on banner.
set FlagsMiscellaneous=/FC /nologo

REM * Warnings *
REM /WX	Treats warnings as errors.
REM /W0, /W1, /W2, /W3, /W4 Sets output warning level.
REM /WL	Enables one-line diagnostics for error and warning messages when compiling C++ source code from the command line.
set FlagsWarnings=/WX /W4

REM * Language *
REM /Zi	Generates complete debugging information.
REM /Z7	Generates C 7.0-compatible debugging information.
REM /Zp n	Packs structure members.
set FlagsLanguage= /Z7

REM * Output files * 
REM /Fp	Specifies a precompiled header file name.
REM -fp:fast -fp:except- ?
set FlagsOutputFiles=

REM * Code generation *
REM /Gm	Deprecated. Enables minimal rebuild.
REM /GR[-]	Enables run-time type information (RTTI).
REM /EH	Specifies the model of exception handling.
REM     a - Enables standard C++ stack unwinding
REM /GS	Checks buffer security.
REM /Gs[size]	Controls stack probes.
set FlagsCodeGeneration= /Gm- /GR- /EHa

REM * Warning Disable * -wd[warning]
REM 4201 nonstandard extension used : nameless struct/union
REM 4100 identifier' : unreferenced formal parameter
REM 4189 identifier' : local variable is initialized but not referenced
REM 4800 type' : forcing value to bool 'true' or 'false' (performance warning)
REM 4505 function' : unreferenced local function has been removed
REM 4456 declaration of 'identifier' hides previous local declaration+
REM 4127 conditional Expression is Constant
REM 4530 C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
set FlagsWarningDisable= /wd4100 /wd4201 /wd4189

REM set FlagsDefines=-DDEFINE=1

REM * Linker options *
REM /OPT	Controls LINK optimizations
REM /INCREMENTAL	Controls incremental linking.
REM /STACK:reserve[,commit]	Sets the size of the stack in bytes.
REM /LD	Creates a dynamic-link library.

set CommonCompilerFlags=%FlagsLinking% %FlagsOptimization% %FlagsMiscellaneous% %FlagsWarnings% %FlagsLanguage% %FlagsOutputFiles% %FlagsCodeGeneration% %FlagsWarningDisable% %FlagsDefines%

set CommonLinkerFlags= -opt:ref ^
    -incremental:no User32.lib gdi32.lib Shell32.lib opengl32.lib msvcrt.lib ..\glfw3.lib

REM Setup vcvars if they are not defined
if not defined DevEnvDir (
  echo DevEnvDir Not Found
  call vcvarsall.bat %Architecture%
)

IF NOT EXIST build mkdir build
pushd build

cl %CommonCompilerFlags% ..\%MainCode% -Fe%ExeName%.exe /link /SUBSYSTEM:CONSOLE /NODEFAULTLIB:MSVCRTD /NODEFAULTLIB:LIBCMTD %CommonLinkerFlags%  

popd