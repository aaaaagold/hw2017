
@set C4_5Path="Windows Version\C4.5\bin\Debug\C4.exe"
@set datafolder="dataset"

@set theexe=%C4_5Path%

@set data=%datafolder%\adult
@echo %data%
@for /L %%i in (1,1,10) do @(
@echo fold %%i
@%theexe% -f %data%_cv%%i -u
)

@set data=%datafolder%\car
@echo %data%
@for /L %%i in (1,1,10) do @(
@echo fold %%i
@%theexe% -f %data%_cv%%i -u
)

@set data=%datafolder%\isolet
@echo %data%
@for /L %%i in (1,1,10) do @(
@echo fold %%i
@%theexe% -f %data%_cv%%i -u
)

@set data=%datafolder%\page-blocks
@echo %data%
@for /L %%i in (1,1,10) do @(
@echo fold %%i
@%theexe% -f %data%_cv%%i -u
)

@set data=%datafolder%\winequality
@echo %data%
@for /L %%i in (1,1,10) do @(
@echo fold %%i
@%theexe% -f %data%_cv%%i -u
)

:END
