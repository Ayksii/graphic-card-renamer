@echo off
echo Dodawanie .vs do .gitignore...
echo .vs/>> .gitignore

echo Usuwanie folderu .vs z indeksu Gita...
git rm -r --cached .vs

echo Ponowne dodawanie wszystkich plików...
git add .

echo Gotowe.
pause
