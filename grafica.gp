set autoscale
set grid
set style data lines
set samples 1000

set multiplot layout 2,1


#------------- Graf 3 ----------------
set title "Redondeo"
set xlabel "muestras"
set ylabel "y(n)"

plot "redondeo.dat"

# Termina la configuración de la segunda subparcela
unset title
unset xlabel
unset ylabel

#------------- Graf 4 ----------------
set title "Truncamiento"
set xlabel "muestras"
set ylabel "y(n)"

plot "truncamiento.dat"

# Termina la configuración de la segunda subparcela
unset title
unset xlabel
unset ylabel

unset multiplot

