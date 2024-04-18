set autoscale
set grid
set style data lines
set samples 1000

set multiplot layout 2,2

#------------- Graf 1 ----------------
set title "Redondeo 5 bits"
set xlabel "muestras"
set ylabel "y(n)"

plot "redondeo5.dat"

unset title
unset xlabel
unset ylabel

#------------- Graf 2 ----------------
set title "Truncamiento 5 bits"
set xlabel "muestras"
set ylabel "y(n)"

plot "truncamiento5.dat"

unset title
unset xlabel
unset ylabel

#------------- Graf 3 ----------------
set title "Redondeo 12 bits"
set xlabel "muestras"
set ylabel "y(n)"

plot "redondeo12.dat"

unset title
unset xlabel
unset ylabel

#------------- Graf 4 ----------------
set title "Truncamiento 12 bits"
set xlabel "muestras"
set ylabel "y(n)"

plot "truncamiento12.dat"

unset title
unset xlabel
unset ylabel

unset multiplot