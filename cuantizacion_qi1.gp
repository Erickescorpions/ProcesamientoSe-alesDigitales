set autoscale
set grid
set style data lines
set samples 1000

set multiplot layout 2,1

#------------- Graf 1 ----------------
set title "Redondeo 5 bits"
set xlabel "muestras"
set ylabel "y(n)"

plot "redondeo_qi1.dat"

unset title
unset xlabel
unset ylabel

#------------- Graf 2 ----------------
set title "Truncamiento 5 bits"
set xlabel "muestras"
set ylabel "y(n)"

plot "truncamiento_qi1.dat"

unset title
unset xlabel
unset ylabel