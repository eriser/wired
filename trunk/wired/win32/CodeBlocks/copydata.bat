mkdir .\bin\data\wired
mkdir .\bin\conf
echo Makefile.am > templist.txt
xcopy ..\..\src\data\* .\bin\data\wired /S /Y /EXCLUDE:templist.txt
del templist.txt
copy /Y ..\*.conf .\bin\conf
