# III.3: mide la velocidad real del procesador mientras corre el programa
# con 1, 2, 4 y 8 hilos (promedio de 7 muestras de 1 segundo).
# Windows la da como % de la velocidad base (2100 MHz).
$contador = "\Información del procesador(_Total)\% de rendimiento del procesador"
$base = (Get-CimInstance Win32_Processor).MaxClockSpeed
$bin = (wsl -d Ubuntu --exec wslpath -a ($PSScriptRoot -replace "\\", "/")).Trim() + "/bin"
$corridas = @{ 1 = 9; 2 = 16; 4 = 28; 8 = 38 }   # unos 11 s de calculo por caso

"Frecuencia base: $base MHz"
"p   % rendimiento   frecuencia aprox.   tiempo por corrida"
$reposo = ((Get-Counter $contador -SampleInterval 1 -MaxSamples 3).CounterSamples | Measure-Object CookedValue -Average).Average
"-   {0,10:N1} %   {1,12:N0} MHz       (en reposo)" -f $reposo, ($reposo * $base / 100)
foreach ($p in 1, 2, 4, 8) {
    $proc = Start-Process wsl -ArgumentList "-d Ubuntu --exec bash `"$bin/../carga.sh`" $p $($corridas[$p])" -PassThru -WindowStyle Hidden
    Start-Sleep -Milliseconds 1500
    $m = ((Get-Counter $contador -SampleInterval 1 -MaxSamples 7).CounterSamples | Measure-Object CookedValue -Average).Average
    $proc.WaitForExit()
    $t = (wsl -d Ubuntu --exec cat "/tmp/frec_$p.med").Trim()
    "{0}   {1,10:N1} %   {2,12:N0} MHz       {3} s (mediana)" -f $p, $m, ($m * $base / 100), $t
    Start-Sleep -Seconds 2
}
