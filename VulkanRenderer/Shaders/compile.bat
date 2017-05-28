for %%f in (.\src\*) do (

  (%VULKAN_SDK%/Bin32/glslangValidator.exe -V %%f -o .\bin\%%~nxf) && (
     echo Compile Successful
  ) || (
  pause )
)
