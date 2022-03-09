Echo "Erase APROM area"
"%ProgramFiles(x86)%\Nuvoton Tools\NuLink Command Tool\NuLink.exe" -e APROM
Echo "Flash your program to APROM"
"%ProgramFiles(x86)%\Nuvoton Tools\NuLink Command Tool\NuLink.exe" -w APROM %1
