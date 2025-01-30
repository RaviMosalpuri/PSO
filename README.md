# Particle Swarm Optimization (PSO)

This repository contains the implementation of Particle Swarm Optimization (PSO) algorithm. PSO is a computational method used for optimizing a wide range of problems by simulating the social behavior of birds flocking or fish schooling.

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Implementation](#implementation)
- [Contributing](#contributing)
- [License](#license)

## Introduction
Particle Swarm Optimization (PSO) is a population-based optimization technique inspired by the social behavior of birds and fish. It is widely used for solving optimization problems in various fields such as engineering, computer science, and economics.

## Features
- Implementation of PSO algorithm
- Helper classes and functions for easy customization

## Installation
To build and run the PSO algorithm, follow these steps:

1. **Clone the repository**:
   ```bash
   git clone https://github.com/yourusername/PSO.git
   cd PSO
   ```

2. **Create a build directory**:
   ```bash
   mkdir build
   cd build
   ```

3. **Run CMake to configure the project**:
   ```bash
   cmake ..
   ```

4. **Build the project**:
   ```bash
   make
   ```

## Usage
To run the PSO algorithm, use the following command:
```bash
./PSO
```

## Implementation
The `PSO.cpp` file contains the main implementation of the PSO algorithm. Here are the key components:

- **Input and Output Directories**: The input directory for test files is specified as `tests/`, and the output directory for results is specified as `results/`. The output directory is created if it doesn't exist.
- **File Processing**: The algorithm loops over all test files in the input directory, parses the data, and extracts relevant information such as the number of cities and items.
- **Distance Matrix**: A distance matrix is created to store the distances between each pair of cities.
- **PSO Initialization**: The PSO algorithm is initialized with parameters such as the number of particles, inertia weight, and acceleration coefficients.
- **PSO Execution**: The PSO algorithm is run for a specified number of iterations, and the travel time and profit are recorded.
- **Output**: The results, including travel time and profit, are written to the output files.

The `HelperClasses.cpp` file contains several important functions and classes used in the PSO algorithm:

- **PSOParticle Class**: This class represents a particle in the PSO algorithm. It includes methods for calculating the total distance of a TSP tour, the total weight and profit of a picking plan, and performing local search optimizations (2-OPT and bit-flip search).
  - `calculateTSPDistance`: Calculates the total distance of the TSP tour.
  - `calculateTotalWeight`: Calculates the total weight of the picking plan.
  - `calculateKnapsackProfit`: Calculates the total profit of the picking plan.
  - `twoOpt`: Performs 2-OPT local search for TSP optimization.
  - `bitFlipSearch`: Performs bit-flip local search for knapsack optimization.
  - `restrictiveLocalSearch`: Combines 2-OPT and bit-flip search for optimization.
  - `prioritise_items`: Prioritizes items based on profit-to-weight ratio.
  - `generate_valid_picking_plan`: Generates a valid picking plan for the knapsack problem.
  - `calculate_speed`: Calculates the speed based on the current weight.
  - `evaluate_fitness`: Evaluates the fitness of the particle based on profit, travel time, and current weight.
  - `update_position`: Updates the position of the particle based on personal and global best positions.

- **PSO Class**: This class represents the PSO algorithm and manages a swarm of particles.
  - `update_particle_position`: Updates the position of all particles in the swarm.
  - `evaluate_particle_fitness`: Evaluates the fitness of all particles in the swarm.
  - `run`: Runs the PSO algorithm for a specified number of iterations.

The `HelperFunctions.cpp` file contains several utility functions used in the PSO algorithm:

- **create_distance_matrix**: This function creates a distance matrix from a list of coordinates. It calculates the Euclidean distance between each pair of cities and stores the distances in a symmetric matrix.
- **RandomFloat**: This function generates a random floating-point number between two specified values.
- **parse_bttp_file**: This function parses a file containing metadata, node coordinates, and item information for the PSO algorithm. It extracts the relevant data and stores it in a structured format.

## Contributing
Contributions are welcome! Please fork the repository and submit a pull request with your changes.

## License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.
