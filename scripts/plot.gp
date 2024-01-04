# Set terminal
set terminal png size 800,600
set datafile separator ','

# Plot 1: Triad (x, y, z)
set output 'plot1.png'
set title 'State Vector: x, y, z'
set xlabel 'Iteration'
set ylabel 'Value'
plot 'state.csv' using 0:1 title 'x' with lines, \
     'state.csv' using 0:2 title 'y' with lines, \
     'state.csv' using 0:3 title 'z' with lines
       
# Plot 2: Triad (phi, theta, psi)
set output 'plot2.png'
set title 'State Vector: phi, theta, psi'
set xlabel 'Iteration'
set ylabel 'Value'
plot 'state.csv' using 0:4 title 'phi' with lines, \
     'state.csv' using 0:5 title 'theta' with lines, \
     'state.csv' using 0:6 title 'psi' with lines

# Plot 3: Triad (dx, dy, dz)
set output 'plot3.png'
set title 'State Vector: dx, dy, dz'
set xlabel 'Iteration'
set ylabel 'Value'
plot 'state.csv' using 0:7 title 'dx' with lines, \
     'state.csv' using 0:8 title 'dy' with lines, \
     'state.csv' using 0:9 title 'dz' with lines

# Plot 4: Triad (dphi, dtheta, dpsi)
set output 'plot4.png'
set title 'State Vector: dphi, dtheta, dpsi'
set xlabel 'Iteration'
set ylabel 'Value'
plot 'state.csv' using 0:10 title 'dphi' with lines, \
     'state.csv' using 0:11 title 'dtheta' with lines, \
     'state.csv' using 0:12 title 'dpsi' with lines

# Plot 5: Control Inputs (u1, u2, u3, u4)
set output 'plot5.png'
set title 'Control Inputs: u1, u2, u3, u4'
set xlabel 'Iteration'
set ylabel 'Value'
plot 'control.csv' using 0:1 title 'u1' with lines, \
     'control.csv' using 0:2 title 'u2' with lines, \
     'control.csv' using 0:3 title 'u3' with lines, \
     'control.csv' using 0:4 title 'u4' with lines

# Plot 1.5: Triad (x, y, z) 3D Plot
set terminal x11
set title 'State Vector: x, y, z'
set xlabel 'x'
set ylabel 'y'
set zlabel 'z'
set view equal xyz
splot 'state.csv' using 1:2:3 title 'drone' with lines
pause -1 "Press any key to continue"
