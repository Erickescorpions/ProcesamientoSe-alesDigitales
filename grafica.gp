set autoscale
set grid
set style data lines
set samples 1000

set multiplot layout 2,1

#------------- Graf 1 ----------------
set title "Funcion original con ruido"
set xlabel   "muestras"
set ylabel   "x(n)"
plot  "fn.dat"

unset title
unset xlabel
unset ylabel

#------------- Graf 2 ----------------
set title "Submuestreo"
set xlabel "muestras"
set ylabel "y(n)"
plot "submuestras.dat"

unset title
unset xlabel
unset ylabel

unset samples  # Restablece la configuración de muestras

unset multiplot