@echo off

attrib -s -h .git
ren .git P.git
attrib +s +h P.git

ren .gitignore P.gitignore
ren M.gitignore .gitignore

git init

exit /b 0