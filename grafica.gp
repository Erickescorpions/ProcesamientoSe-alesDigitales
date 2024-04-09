set autoscale
set grid
set style data lines
set samples 1000

set multiplot layout 2,2

#------------- Graf 1 ----------------
set title "Funcion"
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

# Establece el valor que se interpretará como datos faltantes solo para la segunda gráfica
#set datafile missing "0.0000"
plot "submuestras.dat" with linespoints pointtype 7

unset title
unset xlabel
unset ylabel

#------------- Graf 3 ----------------
set title "Redondeo"
set xlabel "muestras"
set ylabel "y(n)"

plot "redondeo.dat"

unset title
unset xlabel
unset ylabel

#------------- Graf 4 ----------------
set title "Truncamiento"
set xlabel "muestras"
set ylabel "y(n)"

plot "truncamiento.dat" with linespoints pointtype 7

unset title
unset xlabel
unset ylabel

unset multiplot

