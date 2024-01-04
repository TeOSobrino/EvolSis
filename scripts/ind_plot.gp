# Set terminal
set terminal png size 800,600
set datafile separator ','
#set logscale y 10

# Plot 1: Triad (x, y, z)
set output 'bestfit1.png'
set title 'Best Fit Over Time'
set xlabel 'Time'
set ylabel 'Best Fit'
plot '1bestfit.csv' using 0:2 title 'x' with lines, \

# Plot 2: Triad (x, y, z)
set output 'bestfit2.png'
set title 'Best Fit Over Time'
set xlabel 'Time'
set ylabel 'Best Fit'
plot '2bestfit.csv' using 0:2 title 'x' with lines, \

# Plot 3
set output 'bestfit3.png'
set title 'Best Fit Over Time'
set xlabel 'Time'
set ylabel 'Best Fit'
plot '3bestfit.csv' using 0:2 title 'x' with lines, \

# Plot 4
set output 'bestfit4.png'
set title 'Best Fit Over Time'
set xlabel 'Time'
set ylabel 'Best Fit'
plot '4bestfit.csv' using 0:2 title 'x' with lines, \