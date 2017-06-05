
@set NBPath=nb-main.cpp.exe
@set datafold="dataset"

@set theexe=%NBPath%
@for /L %%i in (1,1,10) do @(
@echo fold %%i
@%theexe% %datafold%\adult_cv%%i       -u
@%theexe% %datafold%\car_cv%%i         -u
@%theexe% %datafold%\isolet_cv%%i      -u
@%theexe% %datafold%\page-blocks_cv%%i -u
@%theexe% %datafold%\winequality_cv%%i -u
)

:END
