START /WAIT atmelstudio.exe RenderDisplay.cproj /build debug /out build_log.txt
type build_log.txt
avrdude.exe -c usbasp -p m32 -B 0.5 -U flash:w:"C:\Users\Monmoy\Desktop\RenderDisplay\RenderDisplay\Debug\RenderDisplay.hex":a 