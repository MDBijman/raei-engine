set /p name="Enter shader name: "
glslangvalidator -V %name%.vert -o compiled/%name%.vert.spv
glslangvalidator -V %name%.frag -o compiled/%name%.frag.spv
pause
