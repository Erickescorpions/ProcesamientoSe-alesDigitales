set autoscale
set grid
set style data lines
set samples 1000

set multiplot layout 2,1

#------------- Graf 1 ----------------
set title "Funcion"
set xlabel   "muestras"
set ylabel   "x(n)"
plot  "fn.dat"

# Termina la configuración de la primera subparcela
unset title
unset xlabel
unset ylabel

#------------- Graf 2 ----------------
set title "Submuestreo"
set xlabel "muestras"
set ylabel "y(n)"
plot "submuestras.dat" with linespoints

# Termina la configuración de la segunda subparcela
unset title
unset xlabel
unset ylabel

unset multiplot
