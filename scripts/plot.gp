# Set terminal
set terminal png size 800,600
set datafile separator ','

# Plot 1: Triad (x, y, z)
set output 'build/plots/plot1.png'
set title 'State Vector: x, y, z'
set xlabel 'Iteration'
set ylabel 'Value'
plot 'build/state.csv' using 0:1 title 'x' with lines, \
     'build/state.csv' using 0:2 title 'y' with lines, \
     'build/state.csv' using 0:3 title 'z' with lines

# Plot TinyMPC 1: Triad (x, y, z)
set output 'plotTinyState.png'
set title 'State Vector: x, y, z'
set xlabel 'Iteration'
set ylabel 'Value'
plot 'tinystate.csv' using 0:1 title 'x' with lines, \
     'tinystate.csv' using 0:2 title 'y' with lines, \
     'tinystate.csv' using 0:3 title 'z' with lines
       
# Plot 2: Triad (phi, theta, psi)
set output 'build/plots/plot2.png'
set title 'State Vector: phi, theta, psi'
set xlabel 'Iteration'
set ylabel 'Value'
plot 'build/state.csv' using 0:4 title 'phi' with lines, \
     'build/state.csv' using 0:5 title 'theta' with lines, \
     'build/state.csv' using 0:6 title 'psi' with lines

# Plot 3: Triad (dx, dy, dz)
set output 'build/plots/plot3.png'
set title 'State Vector: dx, dy, dz'
set xlabel 'Iteration'
set ylabel 'Value'
plot 'build/state.csv' using 0:7 title 'dx' with lines, \
     'build/state.csv' using 0:8 title 'dy' with lines, \
     'build/state.csv' using 0:9 title 'dz' with lines

# Plot 4: Triad (dphi, dtheta, dpsi)
set output 'build/plots/plot4.png'
set title 'State Vector: dphi, dtheta, dpsi'
set xlabel 'Iteration'
set ylabel 'Value'
plot 'build/state.csv' using 0:10 title 'dphi' with lines, \
     'build/state.csv' using 0:11 title 'dtheta' with lines, \
     'build/state.csv' using 0:12 title 'dpsi' with lines

# Plot 5: Control Inputs (u1, u2, u3, u4)
set output 'build/plots/plot5.png'
set title 'Control Inputs: u1, u2, u3, u4'
set xlabel 'Iteration'
set ylabel 'Value'
plot 'build/control.csv' using 0:1 title 'u1' with lines, \
     'build/control.csv' using 0:2 title 'u2' with lines, \
     'build/control.csv' using 0:3 title 'u3' with lines, \
     'build/control.csv' using 0:4 title 'u4' with lines

# Plot 5 TinyControl: Control Inputs (u1, u2, u3, u4)
set output 'controlplot.png'
set title 'Control Inputs: u1, u2, u3, u4'
set xlabel 'Iteration'
set ylabel 'Value'
plot 'tinycontrol.csv' using 0:1 title 'u1' with lines, \
     'tinycontrol.csv' using 0:2 title 'u2' with lines, \
     'tinycontrol.csv' using 0:3 title 'u3' with lines, \
     'tinycontrol.csv' using 0:4 title 'u4' with lines


# Plot 1.5: Triad (x, y, z) 3D Plot
set terminal x11
set title 'State Vector: x, y, z'
set xlabel 'x'
set ylabel 'y'
set zlabel 'z'
set view equal xyz
splot 'build/state.csv' using 1:2:3 title 'drone' with lines

# pause -1 "Press any key to continue"

#set terminal gif animate delay 100
#set output 'foobar.gif'
set terminal x11
#stats 'datafile' nooutput


#set xrange [-10:10]
#set yrange [-10:10]
#set zrange [-10:10]

set title 'State Vector: x, y, z'
set xlabel 'x'
set ylabel 'y'
set zlabel 'z'
set view equal xyz
set view 60, 30, 1, 1

set ticslevel 0

set style data points
set pointsize 7
#set pointtype 7  # or any other point type you prefer

$Mydata << EOD
    0, 0, 0
EOD

do for [i=1:int(100)] {
    splot 'build/state.csv' every ::0::i using 1:2:3 title 'drone' with lines, \
            $Mydata title 'origin' with points
    pause 1
}
