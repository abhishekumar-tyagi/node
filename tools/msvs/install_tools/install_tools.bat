@echo off

setlocal

cls
echo ====================================================
echo Tools for Node.js Native Modules Installation Script
echo ====================================================
echo.
echo This Boxstarter script will install Python and the Visual Studio Build
echo Tools, needed to compile Node.js native modules. Note that Boxstarter,
echo Chocolatey and required Windows updates will also be installed.
echo.
echo This will require about 3 Gb of free disk space, plus any space
echo necessary to install Windows updates.
echo.
echo This will take a while to run. Please close all open programs for the
echo duration of the installation as your computer may reboot multiple times
echo while the applicable installations are proceeding.
echo.
echo.
echo You can close this window to stop now. This script can be invoked from the
echo Start menu. Detailed instructions to install these tools manually are
echo available at https://github.com/nodejs/node-gyp#on-windows
echo.
pause

cls
REM Adapted from https://github.com/Microsoft/windows-dev-box-setup-scripts/blob/79bbe5bdc4867088b3e074f9610932f8e4e192c2/README.md#legal
echo Using this script downloads third party software
echo ------------------------------------------------
echo This script will direct to Chocolatey to install packages. By using
echo Chocolatey to install a package, you are accepting the license for the
echo application, executable(s), or other artifacts delivered to your machine
echo as a result of the install(s). This acceptance occurs whether you know
echo the license terms or not. Read and understand the license terms of the
echo packages being installed and their dependencies prior to installation:
echo - https://chocolatey.org/packages/chocolatey
echo - https://chocolatey.org/packages/boxstarter
echo - https://chocolatey.org/packages/python2
echo - https://chocolatey.org/packages/visualstudio2017buildtools
echo - https://chocolatey.org/packages/visualstudio2017-workload-vctools
echo.
echo This script is provided AS-IS without any warranties of any kind
echo ----------------------------------------------------------------
echo Chocolatey has implemented security safeguards in their process to help
echo protect the community from malicious or pirated software, but any use of
echo this script is at your own risk. Please read the Chocolatey's legal
echo terms of use and the Boxstarter project license as well as how the
echo community repository for Chocolatey.org is maintained.
echo.
pause

cls
echo !!!!!WARNING!!!!!
echo -----------------
echo Use of Boxstarter may reboot your computer automatically multiple times.
echo When performing a reboot, Boxstarter will need to disable User Account
echo Control (UAC) to allow the script to run immediately after the reboot.
echo If you are installing Node.js under an alternate user account from your
echo GUI (i.e. you elevate to a separate admin account to perform installs),
echo you may need to log in directly to the GUI interface of that user after
echo reboots. When the scripts have completed, Boxstarter will re-enable UAC
echo and you can log in under your current user account again.
echo.
echo Sometimes the scripts may install all necessary Windows Updates which
echo could cause a high number of reboots that appear to be a reboot loop
echo when in fact it is just a normal Windows Updates reboot cycle.
:acceptretry
echo.
echo Your computer may REBOOT SEVERAL TIMES WITHOUT FURTHER WARNING.
echo Please type YES followed by enter to confirm that you have saved all
set /p "ACCEPT_PROMPT=your open documents and other work and closed all open programs: "
if /i not "%ACCEPT_PROMPT%"=="yes" (
  echo Please type YES to confirm, or close the window to exit.
  goto acceptretry
)

"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" -NoProfile -InputFormat None -ExecutionPolicy Bypass -Command Start-Process '%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe' -ArgumentList '-NoProfile -InputFormat None -ExecutionPolicy Bypass -Command iex ((New-Object System.Net.WebClient).DownloadString(''https://boxstarter.org/bootstrapper.ps1'')); get-boxstarter -Force; Install-BoxstarterPackage -PackageName ''%~dp0\install_tools.txt''; Read-Host ''Type ENTER to exit'' ' -Verb RunAs
