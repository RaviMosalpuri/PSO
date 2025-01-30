#pragma once
#include <iostream>
#include <mutex>
#include <numeric>
#include <random>
#include <thread>
#include <unordered_map>
#include <vector>

#include "HelperFunctions.h"

using namespace std;

struct return_values {
    double fitness, profit, weight, time, best_profit;
};

struct item {
    int index, profit, weight;
};

// Class for a PSO Particle
class PSOParticle {
public:
    PSOParticle(const vector<vector<double>>& distances, const vector<tuple<int, int, int, int>>& items,
        int num_cities, int num_items, double capacity, double v_max, double v_min, double rent_rate);

    /// <summary>
    /// Evaluate the fitness of the particle or solution based on objective function that includes total profit, rent rate and travel time.
    /// </summary>
    /// <param name="distances"></param>
    /// <param name="items"></param>
    /// <param name="capacity"></param>
    /// <param name="rent_rate"></param>
    /// <param name="v_max"></param>
    /// <param name="v_min"></param>
    /// <returns></returns>
    return_values evaluate_fitness(const vector<vector<double>>& distances, const vector<tuple<int, int, int, int>>& items,
        double capacity, double rent_rate, double v_max, double v_min);

    /// <summary>
    /// Updates the position of the particle based on the personal and global best.
    /// c1 and c2 are the acceleration coefficients to determine the influence of personal and global best solutions.
    /// </summary>
    /// <param name="global_best"></param>
    /// <param name="w"></param>
    /// <param name="c1"></param>
    /// <param name="c2"></param>
    void update_position(const pair<vector<int>, vector<double>>& global_best, double w, double c1, double c2);

    /// <summary>
    /// Returns the best position, that is the tour and picking plan
    /// </summary>
    /// <returns></returns>
    inline pair<vector<int>, vector<double>> get_best_position() const { return best_position; }

private:

    /// <summary>
    /// Prioritise the items with higher profit/weight in any city
    /// </summary>
    void prioritise_items();

    void twoOpt();

    void bitFlipSearch();

    void restrictiveLocalSearch(int maxIterations = 2);

    double calculateTSPDistance(const vector<int> &new_tour);

    double calculateTotalWeight(const vector<double> &new_plan);

    double calculateKnapsackProfit(const vector<double> &new_plan);

    /// <summary>
    /// Generates a valid picking plan based on the knapsack capacity.
    /// </summary>
    void generate_valid_picking_plan();

    /// <summary>
    /// Calculates the speed based on the current weight of knapsack.
    /// </summary>
    /// <param name="current_weight"></param>
    /// <returns>double</returns>
    double calculate_speed(double current_weight) const;

    // Distances matrix, contains distances between all the cities
    const vector<vector<double>>& distances;

    //Items vector, contains index, profit, weight and assigned node
    const vector<tuple<int, int, int, int>>& items;

    // Items dictionary, with key as the node and values as index, profit and weight
    unordered_map<size_t, vector<item>> items_dict;
    
    // Best position of the particle, that is the tour and the picking plan
    pair<vector<int>, vector<double>> best_position;

    // Number of cities
    int num_cities;

    // Number of items
    size_t num_items;

    // Current tour
    vector<int> tour;

    // Picking plan for items
    vector<double> picking_plan;

    // Velocity, that is tour and picking plan
    vector<double> velocity;

    // Best fitness of particle
    double best_fitness;

    // Knapsack Capacity
    double capacity;

    // Maximum velocity
    double v_max;

    // Minimum velocity
    double v_min;

    // Rent rate
    double rent_rate;
};


/// <summary>
/// Class for Particle Swarm Optimisation
/// </summary>
class PSO {
public:
    PSO(size_t num_particles, const vector<vector<double>>& distances,
        const vector<tuple<int, int, int, int>>& items, size_t num_cities, size_t num_items, double capacity,
        double v_max, double v_min, double rent_rate);

    /// <summary>
    /// Runs Particle Swarm Optimisation algorithm
    /// </summary>
    /// <param name="iterations"></param>
    /// <param name="w"></param>
    /// <param name="c1"></param>
    /// <param name="c2"></param>
    /// <param name="distances"></param>
    /// <param name="items"></param>
    /// <param name="capacity"></param>
    /// <param name="rent_rate"></param>
    /// <param name="v_max"></param>
    /// <param name="v_min"></param>
    /// <returns></returns>
    tuple<vector<double>, vector<double>> run(size_t iterations, double w, double c1, double c2);

private:

    /// <summary>
    /// Evaluate the particle fitness
    /// </summary>
    /// <param name="w"></param>
    /// <param name="c1"></param>
    /// <param name="c2"></param>
    void evaluate_particle_fitness(double w, double c1, double c2);

    /// <summary>
    /// Update the particle position
    /// </summary>
    /// <param name="w"></param>
    /// <param name="c1"></param>
    /// <param name="c2"></param>
    void update_particle_position(double w, double c1, double c2);

    // Vector for travel times
    vector<double> travel_time_list;

    // Vector for profits
    vector<double> profit_list;

    // Mutex
    mutex mtx;
    
    // Distance matrix
    const vector<vector<double>>& distances;

    // Items
    const vector<tuple<int, int, int, int>>& items;

    // Global best tour and picking plan
    pair<vector<int>, vector<double>> global_best;
    double global_best_fitness;
    double global_best_profit;
    double global_best_time;
    vector<PSOParticle> particles;

    // Number of particles
    size_t num_particles;

    // Number of cities
    size_t num_cities;

    // Number of items
    size_t num_items;

    // Current tour
    vector<int> tour;

    // Picking plan for items
    vector<double> picking_plan;

    // Velocity, that is tour and picking plan
    vector<double> velocity;

    // Knapsack Capacity
    double capacity;

    // Maximum velocity
    double v_max;

    // Minimum velocity
    double v_min;

    // Rent rate
    double rent_rate;
};

