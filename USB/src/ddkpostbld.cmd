@echo off
:: $Id$
setlocal
:: Perform post-build steps
:: An example follows on the next two lines ...
:: xcopy /y ".\obj%BUILD_ALT_DIR%\i386\*.sys" "..\"
:: xcopy /y ".\obj%BUILD_ALT_DIR%\i386\*.pdb" "..\"
xcopy WdfCoInstaller01005.dll ".\obj%BUILD_ALT_DIR%\i386\"
xcopy usbjk.inf ".\obj%BUILD_ALT_DIR%\i386\"

xcopy /y WdfCoInstaller01005.dll "..\install\"
xcopy /y usbjk.inf "..\install\"
xcopy /y ".\obj%BUILD_ALT_DIR%\i386\usbjk.sys" "..\install\"

endlocal
