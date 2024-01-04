# Set terminal
set terminal png size 800,600
set datafile separator ','
#set logscale y 10

# Plot 1: Triad (x, y, z)
set output 'build/plots/bestfit1.png'
set title 'Best Fit Over Time'
set xlabel 'Time'
set ylabel 'Best Fit'
plot 'build/bestfit1.csv' using 0:2 title 'x' with lines, \

# Plot 2: Triad (x, y, z)
set output 'build/plots/bestfit2.png'
set title 'Best Fit Over Time'
set xlabel 'Time'
set ylabel 'Best Fit'
plot 'build/bestfit2.csv' using 0:2 title 'x' with lines, \

# Plot 3
set output 'build/plots/bestfit3.png'
set title 'Best Fit Over Time'
set xlabel 'Time'
set ylabel 'Best Fit'
plot 'build/bestfit3.csv' using 0:2 title 'x' with lines, \

# Plot 4
set output 'build/plots/bestfit4.png'
set title 'Best Fit Over Time'
set xlabel 'Time'
set ylabel 'Best Fit'
plot 'build/bestfit4.csv' using 0:2 title 'x' with lines, \
