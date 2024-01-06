
# Genetic Algorithm for quadcopter Control #

## Motivation ##

Quadricopters are unstable systems, landing, takeoff and maneuverability are 
challenges from the physics point of view. Several mathematical models try to
solve this issue, we have chosen a control strategy based on MPC model, that incorporates the a model of the system in state-space
the aplication of genetic algorithm is to find the ideal inputs for the drone in order to make  stabelize the drone.

We make it clear that the choice of drone and the model used were taken from the [TinyMPC](https://github.com/TinyMPC/TinyMPC) repository.
Therefore, we are not the authors of the modeling. For questions about how the quadcopter was modeled,
consult the TinyMPC staff repository.

## Control Strategy ##

To attempt to control the quadcopter, we chose to use a strategy **inspired** by the model-based predictive controller (MPC)
in discrete time. 

*Caution: What we did is NOT MPC! Just an attempt to create what you could call a "controler", but it doesn't exactly follows the theory.
What we wanted to achieve with this, was a cool and funny project for an undergrad colege discipline, and NOT a serius MPC Controler!*

Therefore, it is worth highlighting some concepts:
- Number of Iterations: Total number of time periods that the simulation lasts.
- Prediction Horizon: Total number of iterations seen by the optimization algorithm in each execution.
- Control Horizon: Total number of control inputs that the algorithm is capable of changing in each execution.
- Admissible states: Set of states (state vector values) that are accepted as a result of the control solution.
- Admissible controls: Set of control input values that are admissible as a solution.
- Chromosome: Basic unit optimized by the Genetic Algorithm.

With that in mind, let's explain what we tried to do:

Starting from a given GA chromosome (a guess to be more direct), a set of control inputs was composed, with a total of H times the dimension of the control input, four (u1, u2, u3, u4), in this case of Crazyflie, where H corresponds to the so-called Control Horizon.

Thus, through a simulation of P iterations (P Prediction Horizon), H commands were applied (one command per iteration) during the first H iterations and a set of commands with null inputs ({0, 0, 0, 0}) in later (P-H) iterations. In the end, an analogous cost function was used, inspired by the cost function used by the **LQR** controller in the P state (x) and control (u) vectors. Thus obtaining the answer (total sum of the cost) and submitting it to the genetic algorithm (GA).

$$ cost = \textbf{x}_{k+1} \cdot Q \cdot \textbf{x}^T_{k+1} + \textbf{u}_k \cdot R \cdot \textbf{u}^T_k $$

The idea is that the genetic algorithm minimizes the calculated cost over generations and thus finds the best control solution for a given instant.
So in this way, as the solution is made as explained, it would become simple to discard solutions that violate the permissible conditions 
of imposed solutions (which in practice may correspond to both desirable characteristics and design 
requirements, such as not colliding the aircraft with the ceiling or floor).

Finally, the idea is that the explained process is carried out for each of the system's iterations, feeding back to the GA (which is called again at each iteration) always with the value of the state that was actually reached after applying just a part of the solution (only the first command) proposed by the algorithm, which in practice corresponds to closing the control loop, making the system robust to instabilities and which in theory can allow a significant reduction in the size of the control and prediction horizon (de facto ideas employed by the MPC controller).

Comments:
- The present work did not close the loop, just carrying out an open-loop optimization and using the single solution in all iterations (Prediction Horizon = Number of Iterations).
- The programmed genetic algorithm does not minimize functions, so the value delivered to the GA was in practice 10^{cte}/(sum of the cost).

## Expected Results ##
It was expected that it would be possible to stabilize the quadcopter around the origin or any other point, as shown below:

### Gif of expected behavior ###

![Gif of expected behavior](https://github.com/TeOSobrino/EvolSis/blob/main/imgs/tiny3dplot.gif)

### Expected graph of position over time ###

![Expected graph of position over time](https://github.com/TeOSobrino/EvolSis/blob/main/imgs/tinyStatePlot.png)

### Expected graph of control inputs over time ###

![Expected graph of control inputs over time](https://github.com/TeOSobrino/EvolSis/blob/main/imgs/tinyControlPlot.png)

*Note: The expected results were obtained using the [TinyMPC](https://github.com/TinyMPC/TinyMPC) algorithm. Obviously we knew it wouldn't be possible to do something as good as theirs, but we hoped to achieve modest results.*

## Achieved Results ##

As mentioned in the observation above, we were unable to achieve these results. Straight up, we’re not even close, as you can see below:

### Gif of the achieved behavior ###

![Gif of the achieved behavior](https://github.com/TeOSobrino/EvolSis/blob/main/imgs/ag3plot.gif)

### Achieved graph of the position over time ###

![Achieved graph of the position over time](https://github.com/TeOSobrino/EvolSis/blob/main/imgs/agxyz.png)

### Achieved graph of control inputs over time ###

![Achieved graph of control inputs over time](https://github.com/TeOSobrino/EvolSis/blob/main/imgs/agcontrol.png)

Several reasons probably led to the non-success (a.k.a failure) of the project, some of them being:
- Very large prediction horizon,
- Use of **double** as the type of genes that make up the chromosome,
- Control Horizon (chromosome size) too large.

These three factors, indicated by Professor Dr. Eduardo do Valle Simoes (@simoesusp), most likely prevented the Genetic Algorithm (detailed below) from evolving properly in a timely manner. Thus, one possibility is to reduce the prediction horizon to something around 5 iterations or less, use a range smaller than that provided by double as a genetic unit and also reduce the control horizon (and thus the size of the chromosome).

Thus, in theory, one can try to successively call the Genetic Algorithm in each of the iterations, letting it deal only with a restricted horizon, but allowing it to function through successive optimizations that together could lead the quadcopter to stability.

*Author's Note: The outlined line of reasoning is based on MPC solutions for power electronics systems (systems in which the control to be effective needs to be extremely responsive), to understand more I suggest reading this [paper](https: //doi.org/10.1007/s00170-021-07682-3), I think it can help clarify ideas.*

*Note: What was discussed above has no guarantee of operation, much less stability, so even if it were possible to control this quadcopter in simulation, a stability analysis would be necessary.*

# Genetic Algorithm #

## What is a genetic algorithm? ##

A genetic algorithm is a program that aims to find the global maxima or minima
of a function, by using randomness and inspiration in nature.

A genetic algorithm starts with an initial population of random individuals,
each individual is a solution to the problem, therefore they all have a function
value associated with them, this value is named fitness, individuals are sorted
based on their fitnes, then they pass through selection and crossover.

Selection will determine if a new individual is fit enough to enter the new
population, doing so by killing one of the individuals of the old population,
after this process new and old population are merged, to ensure that
the number of individuals is constant.

Crossover is a technique to produce offspring using two individuals, this
can be achieved by the central point crossover, in which two individuals
exchange their genes and generate offspring, or an average of all parent's genes
after crossover mutation happens, in one of the offspring's gene, in order to
ensure that the population can explore the solution space.

Then the algorithm returns to selection step, repeating until a satisfactory
answer is found (or time requirements are met).

<p align="center">
   <img src=https://github.com/TeOSobrino/EvolSis/blob/main/imgs/ag.jpg?raw=true/>
</p>

# Island Method #

## Motivation ##

Genetic Algorithms have multiple crossover and selection techniques, some
converge faster, but can be stuck at local maxima or minima, other converge slower
and can take much time to find an accetable solution, for that reason the island
method implements parallelism, running several difernt genetic algorithms, each
with it's own initial population, crossover and selection techniques, and exchanging
their best individuals from time to time, making for a greater genetic diversity
and aiming for a better final solution.

## summary ##

### Theory ###

This program is a genetic algorithm that aims to maximize a function.
It also enables parallelism for calcuting the objective function (parallelism in evaluation), in the file
`` aux.h `` set the `` THREAD NUM `` parameter to the number of threads desired
(if the function being analyzed is very expensive, this can help, but if the
function is simple, the overhead will delay the total execution time).

This program uses the island method, running 4 populations (islands) in
parallel (one thread for each island), each with unique traits, selection and
crossover techniques, at a determined period of time migration between the
islands happens (the islands will exchange the best individuals)

A schematic model of the algorithm:

![alt text](https://github.com/TeOSobrino/EvolSis/blob/main/imgs/Parallel-genetic-algorithm-with-island-model.png?raw=true)

This allows some form of redundancy in finding solutions, and exploits the
difference between selection techniques and how fast they converge, e.g. elitist
crossover (all individuals mate with the best) or entropy-boltzmann, where each
individual gets a probability assigned by the exponential distribution:

$$p(f) = e^{\tfrac{-2\pi f}{T}}$$

where $f$ is the individual fitness, $T$ the temperature, a parameter that
increases the evolutive pressure (increases elitism).

### Practice ###

In pratice, most parameters (number of individuals,
generations, islands, threads, etc)
and are `` #defines `` that can be modified in file `` include/aux.h ``.

The new population always replace the old one
The best solution is always preserved

#### To run your own function ####

The `` GENE_NUM `` parameter represents the number of variables.
In file `` main.cpp `` include your objective function, and follow the pattern adding
`` individual.fitness = calculated_val `` before returning.

## Techniques used ##

### Variable mutation rate ###

If the best individual in the population is the same for a significant amount of
generations, mutation rate will first be decreased (searching for local optima)
then increased (searching for global optima), seeking to leave the current location
as it may be a saddle point.

### Predation by sinthesys ###

In each generation the worst individuals are replaced by the population mean,
excluding thoose individuals.

### Genocide ###

In a determined period genocide will happen, eliminating all individuals and
leaving only the best solution behind, effectivelly resetting the population.

## Conclusions ##

It was not possible to control the quadcopter with the proposed strategy, however, some modifications can be made in order to obtain better results. The project was closed, as it was a project created to be presented in the discipline of Evolutionary Systems Applied to Robotics, taught by professor Eduardo do Vale Simoes (@simoesusp), in the second semester of 2023 (2023.2), thus those involved in the project they delivered it as is, in order to properly take advantage of the end of year break.

### Future Work ###

Regarding the part relating to refining the control solution, it is suggested to read what was detailed in the sub-section (Achieved Results).

Furthermore, at the request of @simoesusp, we recorded a [video](https://drive.google.com/file/d/1Si8IUnitEuL68pLAVMmbWQvhVeX2Zqkk/view?usp=sharing) with explanations about what was done. However, we apologize, as the video was only recorded in Brazilian Portuguese.

![Video with explanations in Brazilian Portuguese](https://drive.google.com/file/d/1Si8IUnitEuL68pLAVMmbWQvhVeX2Zqkk/view?usp=sharing)

## How to Run ##

### Requirements ###

The boost library is used for IPC (inter-process comunication), and mutexes to
ensure that each thread will not override other thread individuals.

for Ubuntu:

``` bash
sudo apt-get install -y g++ make cmake
sudo apt install -y libboost-all-dev
sudo apt install -y eigenlib gnuplot
```

for NixOS:

In the root of the project do
``` bash
nix develop
```

for compilation and running:

```bash
make all
make run
```

to generate the plots at `build/plots`:

```bash
make plot
```

### Final Considerations from those involved ###
@simoesusp: "I'll put your grades on the system. Go enjoy the beach for God's sake!"

@TeOSobrino: "I hope no one undergoes the painful experience of messing with this. Painful!!"

@CarlosCraveiro: "AAAAAAAAA, Hate!! Why doesn't anything work?"

@luana-hartmann "it's working, I'm going to commit it here"
