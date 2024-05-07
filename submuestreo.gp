set autoscale
set grid
set style data lines
set samples 1000

#------------- Submuestreo ----------------
set title "Submuestreo"
set xlabel "muestras"
set ylabel "y(n)"
plot "submuestras.dat"
