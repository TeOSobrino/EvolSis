
# Genetic Algorithm for quadcopter Control #

## Motivation ##

Quadricopters are unstable systems, landing, takeoff and maneuverability are 
challenges from the physics point of view. Several mathematical models try to
solve this issue, we have chosen XXX model, that incorporates the YYY matrices
the aplication of genetic algorithm is to find the ideal weights for the matrices
in order to make the drone's control easier.

We make it clear that the choice of drone and the model used were taken from the XXX repository.
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

## How to Run ##

### Requirements ###

The boost library is used for IPC (inter-process comunication), and mutexes to
ensure that each thread will not override other thread individuals.

for Ubuntu:

``` bash
sudo apt-get install -y g++ make cmake
sudo apt install -y libboost-all-dev
```

for compilation and running:

```bash
make all
make run
```
