@echo off
setlocal EnableDelayedExpansion
chcp 65001 >nul
title MessageBoxEx - manualny test

rem =============================================================
rem  MessageBoxEx.exe - kompletny manualny testovaci beh
rem =============================================================
rem  Ako to funguje:
rem   1. Pre kazdy test sa najprv vypise, co sa ma objavit / stat.
rem   2. Stlacenim ENTER sa test spusti (spusti sa MessageBoxEx.exe
rem      s danymi parametrami).
rem   3. Over dialog / konzolu podla popisu a zavri ho.
rem   4. Potvrd vysledok: Y = spravne, N = chyba, S = preskocit.
rem   5. Vysledky sa priebezne zapisuju do test-results.txt.
rem
rem  Pouzitie:
rem   test-manual.bat                 - najde exe sam (Release/Debug)
rem   test-manual.bat "cesta\k.exe"   - pouzije zadanu cestu k exe
rem =============================================================

set "SCRIPT_DIR=%~dp0"
set "EXE="

if not "%~1"=="" set "EXE=%~1"
if not defined EXE if exist "%SCRIPT_DIR%Release\MessageBoxEx.exe" set "EXE=%SCRIPT_DIR%Release\MessageBoxEx.exe"
if not defined EXE if exist "%SCRIPT_DIR%Debug\MessageBoxEx.exe" set "EXE=%SCRIPT_DIR%Debug\MessageBoxEx.exe"
if not defined EXE if exist "%SCRIPT_DIR%MessageBoxEx.exe" set "EXE=%SCRIPT_DIR%MessageBoxEx.exe"

if not defined EXE (
    echo [CHYBA] MessageBoxEx.exe sa nenasiel v Release\, Debug\ ani v priecinku skriptu.
    echo Zostav projekt vo Visual Studiu, alebo zadaj cestu k exe ako parameter:
    echo   %~nx0 "C:\cesta\MessageBoxEx.exe"
    exit /b 1
)
for %%F in ("%EXE%") do set "EXE=%%~fF"

set "LOG=%SCRIPT_DIR%test-results.txt"
set "TRIGGER_FILE=%SCRIPT_DIR%trigger.txt"

rem Testovacie ikony patria do projektu, nie niekam do profilu pouzivatela - ulozene su
rem v podpriecinku test\ vedla tohto skriptu.
set "TEST_DIR=%SCRIPT_DIR%test\"
if not exist "%TEST_DIR%" mkdir "%TEST_DIR%"
set "ICON_BMP=%TEST_DIR%test-icon.bmp"
set "ICONAPP_DISK=%TEST_DIR%test-icon-disk.ico"

rem MessageBoxEx.exe posiela hodnoty -icon/-iconapp/-file cez ParseEscapeString,
rem ktora interpretuje \n \r \t \x \u ako escape sekvencie - v beznej Windows
rem ceste (napr. ...\test-icon.bmp obsahuje \t) to cestu poskodi. Pre argumenty
rem programu preto pouzivame verziu s dopredným lomitkom (Windows API ju prijme
rem rovnako ako spatne lomitko), interne v skripte ostavaju povodne cesty.
set "ICON_BMP_ARG=%ICON_BMP:\=/%"
set "TRIGGER_FILE_ARG=%TRIGGER_FILE:\=/%"
set "ICONAPP_ARG=%SCRIPT_DIR:\=/%Resources/icon1.ico"
set "ICONAPP_DISK_ARG=%ICONAPP_DISK:\=/%"

rem Testovacia ikona pre -iconapp z inej cesty ako Resources (test\ namiesto Resources\).
if not exist "%ICONAPP_DISK%" (
    powershell -NoProfile -Command "Add-Type -AssemblyName System.Drawing; $b=New-Object System.Drawing.Bitmap(32,32); $g=[System.Drawing.Graphics]::FromImage($b); $g.Clear([System.Drawing.Color]::DodgerBlue); $ic=[System.Drawing.Icon]::FromHandle($b.GetHicon()); $fs=[System.IO.File]::Create('%ICONAPP_DISK%'); $ic.Save($fs); $fs.Close()" >nul 2>&1
)

rem Pripravit testovaci BMP obrazok pre testy s ikonou, ak este neexistuje.
if not exist "%ICON_BMP%" (
    powershell -NoProfile -Command "Add-Type -AssemblyName System.Drawing; $b=New-Object System.Drawing.Bitmap(48,48); $g=[System.Drawing.Graphics]::FromImage($b); $g.Clear([System.Drawing.Color]::OrangeRed); $b.Save('%ICON_BMP%',[System.Drawing.Imaging.ImageFormat]::Bmp)" >nul 2>&1
)

set /a TOTAL=0
set /a PASS=0
set /a FAIL=0
set /a SKIP=0

echo. >> "%LOG%"
echo ============================================================ >> "%LOG%"
echo Test beh: %DATE% %TIME% >> "%LOG%"
echo Exe: %EXE% >> "%LOG%"
echo ============================================================ >> "%LOG%"

cls
echo ============================================================
echo   MessageBoxEx.exe - KOMPLETNY MANUALNY TEST
echo ============================================================
echo Testovany subor: %EXE%
echo.
echo Postup pre kazdy test:
echo   1. Precitaj si, co sa ma stat / objavit.
echo   2. Stlac ENTER - test sa spusti.
echo   3. Over dialog / konzolu podla popisu, potom ho zavri.
echo   4. Potvrd vysledok: Y = spravne, N = chyba, S = preskocit.
echo.
echo Vysledky sa priebezne zapisuju do: %LOG%
echo Kedykolvek mozes cely beh ukoncit cez CTRL+C.
echo ============================================================
pause

rem =============================================================
rem A. ZAKLADNE PARAMETRE A CHYBOVE STAVY
rem =============================================================
echo.
echo ############################################################
echo # A. ZAKLADNE PARAMETRE A CHYBOVE STAVY
echo ############################################################

set "ID=A1-help"
set "DESC=Vypise sa kompletny zoznam parametrov a priklad pouzitia do konzoly. Nema sa otvorit ziadne okno. Errorlevel ma byt 0."
call :runtest -help

set "ID=A2-basic"
set "DESC=Objavi sa okno s titulkom Test a textom Zakladny dialog, s jednym tlacidlom Yes. Po kliknuti na Yes ma byt errorlevel 1."
call :runtest -t Test -m "Zakladny dialog"

set "ID=A3-missing-message"
set "DESC=V konzole sa vypise Error - message is empty. Ziadne okno sa neotvori, errorlevel 0."
call :runtest -t BezSpravy -m ""

set "ID=A4-unknown-param"
set "DESC=V konzole sa vypise Unknown parameter a strucna sprava Run with -help for usage information - NIE cela napoveda. Ziadne okno sa neotvori."
call :runtest -neexistujuciParameter hodnota

set "ID=A5-invalid-enum"
set "DESC=V konzole sa vypise Invalid enum value a strucna sprava Run with -help for usage information - NIE cela napoveda. Ziadne okno sa neotvori."
call :runtest -m "test" -button 9

set "ID=A6-invalid-color"
set "DESC=V konzole sa vypise chyba o neplatnom znaku vo farbe a strucna sprava Run with -help for usage information - NIE cela napoveda. Ziadne okno sa neotvori."
call :runtest -m "test" -p ZZZZZZ -button 1

rem =============================================================
rem B. VZHLAD TEXTU
rem =============================================================
echo.
echo ############################################################
echo # B. VZHLAD TEXTU
echo ############################################################

set "ID=B1-notitle"
set "DESC=Objavi sa okno BEZ titulku a bez ramceka okolo - borderless, s jednym tlacidlom."
call :runtest -m "Okno bez titulku a ramika" -nt -button 1

set "ID=B2-multiline"
set "DESC=Text sa zobrazi na troch riadkoch pod sebou, nie na jednom riadku."
call :runtest -m "Riadok jedna\nRiadok dva\nRiadok tri" -l 3

set "ID=B3-unicode"
set "DESC=Za slovom Hotovo sa zobrazi emoji palec hore."
call :runtest -m "Hotovo \u{1F44D}"

set "ID=B4-font"
set "DESC=Text je vo fonte Arial a viditelne vacsi ako v predchadzajucich testoch."
call :runtest -m "Ina pisma a velkost" -f Arial -fs 28

set "ID=B5-width"
set "DESC=Okno je znacne sirsie nez predchadzajuce, priblizne 1000 px."
call :runtest -m "Siroke okno" -w 1000

rem =============================================================
rem C. FARBY
rem =============================================================
echo.
echo ############################################################
echo # C. FARBY
echo ############################################################

set "ID=C1-pen-color"
set "DESC=Text spravy je cerveny."
call :runtest -m "Cerveny text" -p FF0000 -button 1

set "ID=C2-background-color"
set "DESC=Priestor za textom - pozadie textu - je zeleny."
call :runtest -m "Zelene pozadie za textom" -b 00CC00 -button 1

set "ID=C3-brush-color"
set "DESC=Pozadie celeho okna, aj mimo textu, je modre."
call :runtest -m "Modre pozadie okna" -br 0000CC -button 1

set "ID=C4-default-colors-regression"
set "DESC=Bez zadanych farieb ma okno vyzerat standardne - biely/systemovy text na normalnom pozadi, NIE cierne, rozbite ci blikajuce pozadie. Overuje opravu bodu 1 z todo.md."
call :runtest -m "Ziadne farby nezadane" -button 1

rem =============================================================
rem D. IKONY
rem =============================================================
echo.
echo ############################################################
echo # D. IKONY
echo ############################################################

set "ID=D1-icon"
set "DESC=Vlavo od textu sa zobrazi maly oranzovy stvorec - ikona z BMP suboru. Ak sa neobjavi, over ci existuje subor test-icon.bmp v priecinku skriptu."
call :runtest -i "%ICON_BMP_ARG%" -m "S BMP ikonou vlavo" -button 1

set "ID=D2-icon-border"
set "DESC=Ikona je vacsia, 64 px, a ma viditelny ramik okolo seba."
call :runtest -i "%ICON_BMP_ARG%" -iconsize 64 -iconborder -m "Vacsia ikona s ramikom" -button 1

set "ID=D3-iconapp"
set "DESC=V titulku okna a na paneli uloh je vlastna ikona z Resources\icon1.ico namiesto predvolenej."
call :runtest -iconapp "%ICONAPP_ARG%" -m "Vlastna ikona aplikacie" -button 1

set "ID=D4-iconapp-from-disk"
set "DESC=V titulku okna a na paneli uloh je INA, modra ikona, nacitana z .ico suboru v priecinku test\ - nie z Resources\ ako v predchadzajucom teste. Ma sa lisit od ikony z testu D3, nie byt rovnaka."
call :runtest -iconapp "%ICONAPP_DISK_ARG%" -m "Ikona nacitana z disku mimo Resources" -button 1

rem =============================================================
rem E. TLACIDLA A KLAVESNICA
rem =============================================================
echo.
echo ############################################################
echo # E. TLACIDLA A KLAVESNICA
echo ############################################################

set "ID=E1-buttons-0-timeout"
set "DESC=Nezobrazi sa ziadne tlacidlo. Po cca 3 sekundach sa okno samo zavrie. Errorlevel ma byt 0. Overuje opravu bodu 4 z todo.md."
call :runtest -m "Ziadne tlacidlo. Po 3 sekundach sa zavrie samo" -button 0 -ttc 3000

set "ID=E2-buttons-2"
set "DESC=Zobrazia sa dve tlacidla s textami Ano a Nie. Klikni Ano - errorlevel ma byt 1."
call :runtest -m "Dve tlacidla s vlastnymi textami" -button 2 -b1 Ano -b2 Nie

set "ID=E3-buttons-3-center"
set "DESC=Zobrazia sa tri tlacidla - Ano, Nie, Zrusit - vodorovne vycentrovane a sirsie ako standardne."
call :runtest -m "Tri vycentrovane a sirsie tlacidla" -button 3 -b1 Ano -b2 Nie -b3 Zrusit -center -widthbuttons 20

set "ID=E4-default-button"
set "DESC=Pri otvoreni je fokusovane druhe tlacidlo Nie. Stlac ENTER bez klikania mysou - potvrdi sa Nie, errorlevel ma byt 2."
call :runtest -m "Fokus by mal byt na druhom tlacidle" -button 3 -b1 Ano -b2 Nie -b3 Zrusit -default 2

set "ID=E5-windows-return-code"
set "DESC=Klikni na Ano. V konzole sa vypise cislo vysledku - ma byt 6, teda IDYES, nie 1."
call :runtest -m "Klikni na Ano" -button 3 -b1 Ano -b2 Nie -b3 Zrusit -wrc

set "ID=E6-escape"
set "DESC=Stlac klavesu ESC namiesto klikania na tlacidlo. Vysledny errorlevel ma byt 0."
call :runtest -m "Stlac klavesu ESC" -button 3

set "ID=E7-enter-default"
set "DESC=Stlac ENTER bez klikania mysou. Pouzije sa predvolene druhe tlacidlo, errorlevel ma byt 2."
call :runtest -m "Stlac ENTER - pouzije sa predvolene 2. tlacidlo" -button 2 -default 2

set "ID=E8-tab-cycle"
set "DESC=Opakovanym stlacanim TAB sa fokus cykli postupne 1, 2, 3, 1. So SHIFT+TAB v opacnom poradi."
call :runtest -m "Klavesou TAB prepinaj medzi tlacidlami" -button 3

rem =============================================================
rem F. POZICOVANIE A MONITORY
rem =============================================================
echo.
echo ############################################################
echo # F. POZICOVANIE A MONITORY
echo ############################################################

set "ID=F1-position-top"
set "DESC=Okno sa zobrazi pri hornom okraji obrazovky, vodorovne v strede."
call :runtest -m "Hore v strede obrazovky" -pos top -button 1

set "ID=F2-position-bottom"
set "DESC=Okno sa zobrazi pri spodnom okraji obrazovky, vodorovne v strede."
call :runtest -m "Dole v strede obrazovky" -pos bottom -button 1

set "ID=F3-position-left"
set "DESC=Okno sa zobrazi pri lavom okraji obrazovky, zvisle v strede."
call :runtest -m "Vlavo, zvisle v strede" -pos left -button 1

set "ID=F4-position-right"
set "DESC=Okno sa zobrazi pri pravom okraji obrazovky, zvisle v strede."
call :runtest -m "Vpravo, zvisle v strede" -pos right -button 1

echo.
echo Pred dalsim testom presun kurzor mysi tam, kde chces, aby sa okno objavilo.
pause
set "ID=F5-position-pointer"
set "DESC=Okno sa zobrazi presne tam, kde bol kurzor mysi v momente spustenia."
call :runtest -m "Pri kurzore mysi" -pos pointer -button 1

set "ID=F6-position-xy-offset"
set "DESC=Okno sa zobrazi 100 px od laveho a 100 px od horneho okraja obrazovky."
call :runtest -m "Posunute o 100,100 od laveho horneho rohu" -pos xy -x 100 -y 100 -button 1

set "ID=F7-monitor-invalid"
set "DESC=V konzole sa vypise Error - invalid monitor: abc. Ziadne okno sa neotvori, errorlevel 0. Overuje opravu poznamky o monitore z todo.md."
call :runtest -mon abc -m "Neplatny monitor"

set "ID=F8-monitor-primary"
set "DESC=Okno sa zobrazi v strede primarneho monitora."
call :runtest -mon primary -pos center -m "Na primarnom monitore" -button 1

set "ID=F9-monitor-index-1"
set "DESC=Ak mas druhy monitor s indexom 1, okno sa zobrazi v strede na nom. Ak druhy monitor nemas, over ci sa aspon vypise zmysluplna chyba alebo fallback - napr. spadne na monitor 0 - a nie pad programu."
call :runtest -mon 1 -pos center -m "Monitor s indexom 1" -button 1

set "ID=F10-monitor-index-2"
set "DESC=Ak mas tretiu obrazovku s indexom 2, okno sa zobrazi v strede na nej. Ak nie, over rovnako ako pri predchadzajucom teste - zmysluplny fallback, ziadny pad."
call :runtest -mon 2 -pos center -m "Monitor s indexom 2" -button 1

rem =============================================================
rem G. SPRAVANIE A CASOVANIE
rem =============================================================
echo.
echo ############################################################
echo # G. SPRAVANIE A CASOVANIE
echo ############################################################

set "ID=G1-topmost"
set "DESC=Po otvoreni klikni na plochu alebo inu aplikaciu - toto okno ma zostat viditelne navrchu vsetkych ostatnych."
call :runtest -m "Mal by zostat navrchu" -topmost -button 1

set "ID=G2-quiet"
set "DESC=Po zatvoreni okna sa do konzoly NEMA vypisat ziadne cislo vysledku - bez -quiet by sa vypisalo."
call :runtest -m "Po kliknuti sa nic nevypise do konzoly" -button 1 -quiet

set "ID=G3-min-display-time"
set "DESC=Skus kliknut na Yes hned po otvoreni - okno sa napriek tomu zavrie az po uplynuti cca 3 sekund."
call :runtest -m "Skus zavriet hned, pojde to az po 3 sekundach" -mdt 3000 -button 1

if exist "%TRIGGER_FILE%" del "%TRIGGER_FILE%"
set "ID=G4-file-required"
set "DESC=V inom okne CMD alebo v Prieskumnikovi vytvor subor s nazvom trigger.txt priamo v priecinku tohto skriptu, s lubovolnym obsahom. Okno sa po jeho objaveni samo zavrie a subor sa nasledne vymaze vdaka -dfrfc."
call :runtest -m "Zavrie sa, ked sa objavi trigger.txt v priecinku skriptu" -file "%TRIGGER_FILE_ARG%" -dfrfc -button 1

goto :summary

rem =============================================================
rem Pomocna rutina: spusti jeden test podla premennych ID a DESC,
rem argumenty pre MessageBoxEx.exe su prevzate z parametrov call-u.
rem =============================================================
:runtest
set ARGS=%*
set /a TOTAL+=1
cls
echo ============================================================
echo TEST !ID!   (%TOTAL%. v poradi)
echo ------------------------------------------------------------
echo Ocakavane:
echo   !DESC!
echo.
echo Prikaz: "%EXE%" !ARGS!
echo ============================================================
echo Stlac ENTER pre spustenie testu...
pause >nul

"%EXE%" !ARGS!
set "RC=%ERRORLEVEL%"

echo.
echo Dialog sa zatvoril. Navratovy kod (errorlevel): %RC%
choice /c YNS /n /m "Zodpoveda vysledok popisu vyssie? [Y]ano [N]ie [S]kip: "
if errorlevel 3 goto :runtest_skip
if errorlevel 2 goto :runtest_fail

echo [ OK ] !ID! - !DESC! ^| errorlevel=!RC! >> "%LOG%"
set /a PASS+=1
goto :runtest_done

:runtest_fail
echo [FAIL] !ID! - !DESC! ^| errorlevel=!RC! >> "%LOG%"
set /a FAIL+=1
goto :runtest_done

:runtest_skip
echo [SKIP] !ID! - !DESC! ^| errorlevel=!RC! >> "%LOG%"
set /a SKIP+=1

:runtest_done
exit /b 0

:summary
echo.
echo ============================================================
echo VYSLEDOK: %PASS% OK, %FAIL% chyba, %SKIP% preskocenych, spolu %TOTAL%
echo Podrobny log: %LOG%
echo ============================================================
echo Zhrnutie: %PASS% OK, %FAIL% chyba, %SKIP% preskocenych, spolu %TOTAL% >> "%LOG%"
endlocal
pause
exit /b 0
