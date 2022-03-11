mkdir x64\Debug
mkdir x64\Release
xcopy /y /d %WX_LIB64%\lib\vc_x64_dll\wxbase316u_vc_custom.dll x64\Release
xcopy /y /d %WX_LIB64%\lib\vc_x64_dll\wxbase316ud_vc_custom.dll x64\Debug
xcopy /y /d %WX_LIB64%\lib\vc_x64_dll\wxmsw316u_core_vc_custom.dll x64\Release
xcopy /y /d %WX_LIB64%\lib\vc_x64_dll\wxmsw316ud_core_vc_custom.dll x64\Debug
xcopy /y /d %WX_LIB64%\lib\vc_x64_dll\wxbase316u_net_vc_custom.dll x64\Release
xcopy /y /d %WX_LIB64%\lib\vc_x64_dll\wxbase316ud_net_vc_custom.dll x64\Debug

