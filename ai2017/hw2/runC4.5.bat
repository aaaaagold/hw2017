
@set C4_5Path="Windows Version\C4.5\bin\Debug\C4.exe"
@set datafold="dataset"

@echo cutting data
@set theexe=cv-main.cpp.exe
@%theexe% %datafold%\adult
@%theexe% %datafold%\car
@%theexe% %datafold%\isolet
@%theexe% %datafold%\page-blocks
@%theexe% %datafold%\winequality

@set theexe=%C4_5Path%
@for /L %%i in (1,1,10) do @(
@echo fold %%i
@%theexe% -f %datafold%\adult_cv%%i       -u
@%theexe% -f %datafold%\car_cv%%i         -u
@%theexe% -f %datafold%\isolet_cv%%i      -u
@%theexe% -f %datafold%\page-blocks_cv%%i -u
@%theexe% -f %datafold%\winequality_cv%%i -u
)

:END
