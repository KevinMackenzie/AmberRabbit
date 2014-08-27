IF "%2" == "Debug" (
copy "%~dp0\packages\sfml.redist.2.1.0.0\build\native\bin\%1\v110\%2\Desktop\sfml-audio-d-2.dll" "%~dp0\..\Bin\%1\%2\" /Y
copy "%~dp0\packages\sfml.redist.2.1.0.0\build\native\bin\%1\v110\%2\Desktop\sfml-system-d-2.dll" "%~dp0\..\Bin\%1\%2\" /Y
) ELSE (
copy "%~dp0\packages\sfml.redist.2.1.0.0\build\native\bin\%1\v110\%2\Desktop\sfml-audio-2.dll" "%~dp0\..\Bin\%1\%2\" /Y
copy "%~dp0\packages\sfml.redist.2.1.0.0\build\native\bin\%1\v110\%2\Desktop\sfml-system-2.dll" "%~dp0\..\Bin\%1\%2\" /Y
)

copy "%~dp0\packages\sfml.redist.2.1.0.0\build\native\bin\%1\v110\%2\Desktop\openal32.dll" "%~dp0\..\Bin\%1\%2\" /Y
copy "%~dp0\packages\sfml.redist.2.1.0.0\build\native\bin\%1\v110\%2\Desktop\libsndfile-1.dll" "%~dp0\..\Bin\%1\%2\" /Y
copy "%~dp0\packages\libogg.redist.1.3.0.1\build\native\bin\v110\%1\%2\dynamic\libogg.dll" "%~dp0\..\Bin\%1\%2\" /Y
::copy "%~dp0\packages\zlib.redist.1.2.8.6\build\native\bin\v110\%1\%2\dynamic\cdecl\zlib.dll" "%~dp0\..\Bin\%1\%2\" /Y