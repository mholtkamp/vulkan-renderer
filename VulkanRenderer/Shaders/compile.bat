#%VULKAN_SDK%/Bin32/glslangValidator.exe -V shader.vert
#%VULKAN_SDK%/Bin32/glslangValidator.exe -V shader.frag
#pause

for %%f in (.\src\*) do (

  %VULKAN_SDK%/Bin32/glslangValidator.exe -V %%f -o .\bin\%%~nxf
)

pause