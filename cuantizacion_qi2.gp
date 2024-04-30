set autoscale
set grid
set style data lines
set samples 1000

set multiplot layout 2,1


#------------- Graf 1 ----------------
set title "Redondeo 12 bits"
set xlabel "muestras"
set ylabel "y(n)"

plot "redondeo_qi2.dat"

unset title
unset xlabel
unset ylabel

#------------- Graf 2 ----------------
set title "Truncamiento 12 bits"
set xlabel "muestras"
set ylabel "y(n)"

plot "truncamiento_qi2.dat"

unset title
unset xlabel
unset ylabel

unset multiplot