
@set CVPath=cv-main.cpp.exe
@set datafold="dataset"

@echo cutting data
@set theexe=%CVPath%
@%theexe% %datafold%\adult
@%theexe% %datafold%\car
@%theexe% %datafold%\isolet
@%theexe% %datafold%\page-blocks
@%theexe% %datafold%\winequality

:END
