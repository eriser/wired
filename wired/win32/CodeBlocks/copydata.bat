mkdir .\bin\data\
mkdir .\bin\conf
echo Makefile.am > templist.txt
xcopy ..\..\src\data\* .\bin\data\ /S /Y /EXCLUDE:templist.txt
del templist.txt
copy /Y ..\*.conf .\bin\conf
