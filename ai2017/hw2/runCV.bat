
@set CVPath=cv-main.cpp.exe
@set datafolder="dataset"

@echo cutting data
@set theexe=%CVPath%
@%theexe% %datafolder%\adult
@%theexe% %datafolder%\car
@%theexe% %datafolder%\isolet
@%theexe% %datafolder%\page-blocks
@%theexe% %datafolder%\winequality

:END
