# GUI / Console Subsystem application

This project showcases how a program can both run as a console project and as
a Windows native application.

The program is just a Windows application in reality, but it calls
[AttachConsole()](https://docs.microsoft.com/en-us/windows/console/attachconsole)
to get a hold of the current console and act as a valid Console Subsystem
program.

`AttachConsole()` returns immediately so the trick to block the console is
to wrap the executable in a launcher script - `mm.cmd` here.

## TODO

Deal with redirects - `<`, `>` and `2>` currently don't work.
