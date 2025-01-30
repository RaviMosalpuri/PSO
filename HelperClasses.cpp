#include "HelperClasses.h"

// Calculate the total distance of the TSP tour
double PSOParticle::calculateTSPDistance(const vector<int> &new_tour)
{
    double totalDistance = 0.0;

    for (size_t i = 0; i < new_tour.size() - 1; i++)
    {
        totalDistance += distances[new_tour[i]][new_tour[i + 1]];
    }

    // Add the distance to return to the starting city
    totalDistance += distances[new_tour.back()][new_tour.front()];
    return totalDistance;
}


// Calculate the total weight of the picking plan
double PSOParticle::calculateTotalWeight(const vector<double> &new_plan)
{
    double totalWeight = 0.0;

    for (size_t i = 0; i < new_plan.size(); i++)
    {
        if (new_plan[i] == 1)
        {
            totalWeight += get<2>(items[i]); // Assuming second is weight
        }
    }

    return totalWeight;
}


// Calculate the total profit of the picking plan
double PSOParticle::calculateKnapsackProfit(const vector<double>& new_plan)
{
    double totalProfit = 0.0;

    for (size_t i = 0; i < new_plan.size(); i++)
    {
        if (new_plan[i] == 1)
        {
            totalProfit += get<1>(items[i]); // Assuming first is profit
        }
    }

    return totalProfit;
}


// 2-OPT local search for TSP optimization
void PSOParticle::twoOpt()
{
    vector<int> bestTour = tour;
    double bestDistance = calculateTSPDistance(bestTour);
    bool improved = true;
    int iteration = 0;
    mutex mtx;

    while (improved) {
        improved = false;
        iteration++;
        //cout << "Iteration " << iteration << ": Current Distance = " << bestDistance << endl;

        vector<thread> threads;

        for (size_t i = 1; i < tour.size() - 2; i++)
        {
            threads.emplace_back([&, i]() {
                for (size_t j = i + 1; j < tour.size(); j++) 
                {
                    if (j - i == 1)
                    {
                        continue; // Skip consecutive nodes
                    }

                    vector<int> newTour = bestTour;
                    reverse(newTour.begin() + i, newTour.begin() + j);
                    double newDistance = calculateTSPDistance(newTour);

                    lock_guard<mutex> lock(mtx);
                    if (newDistance < bestDistance)
                    {
                        //cout << "Improvement found! Distance reduced from " << bestDistance << " to " << newDistance << endl;
                        bestTour = newTour;
                        bestDistance = newDistance;
                        improved = true;
                    }
                }
            });
        }

        for (auto& t : threads)
        {
            if (t.joinable())
            {
                t.join();
            }
        }
    }
    tour = bestTour;
}

// Bit-flip local search for knapsack optimization
void PSOParticle::bitFlipSearch()
{
    vector<double> bestPlan = picking_plan;
    double bestProfit = calculateKnapsackProfit(bestPlan);
    mutex mtx;

    vector<thread> threads;

    for (size_t i = 0; i < picking_plan.size(); i++)
    {
        threads.emplace_back([&, i]() {
            vector<double> newPlan = bestPlan;
            newPlan[i] = 1 - newPlan[i]; // Flip the bit

            if (calculateTotalWeight(newPlan) <= capacity)
            {
                double newProfit = calculateKnapsackProfit(newPlan);

                lock_guard<mutex> lock(mtx);
                if (newProfit > bestProfit)
                {
                    //cout << "Improvement found! Profit increased from " << bestProfit << " to " << newProfit << endl;
                    bestPlan = newPlan;
                    bestProfit = newProfit;
                }
            }
        });
    }

    for (auto& t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }

    picking_plan = bestPlan;
}

// Restrictive local search combining 2-OPT and bit-flip search
void PSOParticle::restrictiveLocalSearch(int maxIterations)
{
    for (int iteration = 0; iteration < maxIterations; iteration++)
    {
        //cout << "Restrictive Local Search Iteration " << iteration + 1 << endl;
        twoOpt(); // Optimize the TSP tour
        bitFlipSearch(); // Optimize the picking plan
    }
}

void PSOParticle::prioritise_items()
{
    for (auto& [city, item_list] : items_dict)
    {
        // Sort items in a temporary vector
        vector<item> sorted_items = item_list;
        sort(sorted_items.begin(), sorted_items.end(), 
            [](const item& a, const item& b) {
            return ((a.profit) / static_cast<double>((a.weight))) >
                ((b.profit) / static_cast<double>((b.weight)));
            });

        // Replace the unordered_map value with the sorted vector
        items_dict[city] = sorted_items;
    }
}



//------------------------------------------------------------------------------------------------------------------------
// PSOParticle class
//------------------------------------------------------------------------------------------------------------------------
PSOParticle::PSOParticle(const vector<vector<double>>& distances, const vector<tuple<int, int, int, int>>& items,
    int num_cities, int num_items, double capacity, double v_max, double v_min, double rent_rate)
    :distances(distances), items(items), num_cities(num_cities), num_items(num_items), capacity(capacity), v_max(v_max),
    v_min(v_min), rent_rate(rent_rate)
{
    // Instantiate the items dictionary, with key as assigned node or city
    for (const auto &i : items)
    {
        items_dict[get<3>(i)].push_back(item{ get<0>(i), get<1>(i), get<2>(i) });
        //items_dict[get<3>(item)].push_back({ get<0>(item), get<1>(item), get<2>(item) });
    }

    prioritise_items();

    // Initialise the tour vector
    tour = vector<int>(num_cities, 0);
    iota(tour.begin(), tour.end(), 0);
    // Use a random number generator
    random_device rd;
    mt19937 g(rd()); // Mersenne Twister engine
    shuffle(tour.begin(), tour.end(), g); // Shuffle tour starting from the first element
    
    // Initialise the picking plan
    picking_plan.resize(num_items, 0);
    generate_valid_picking_plan();

    restrictiveLocalSearch();

    // Initialise the velocity that is the tour and picking plan
    velocity = vector<double>(num_cities + num_items, 0.0);

    // Set best position to tour and picking plan
    best_position = { tour, picking_plan };
    best_fitness = -1e9;
}


void PSOParticle::generate_valid_picking_plan()
{
    // Generate a random number using uniform distribution
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    
    double current_weight = 0;

    // Loop over the tour
    //for (auto city = tour.rbegin(); city != tour.rend(); ++city)
    //{
    //    // Loop over the items in the current city
    //    for (const auto &item : items_dict[*city])
    //    {
    //        // Random value generator
    //        double r = dis(gen);
    //        const double r_check = 0.3;

    //        // Pick the item if the weight of current item and knapsack weight is less than the capacity
    //        if (r > r_check && current_weight + item.weight <= capacity)
    //        {
    //            // Add the item to picking plan and add to current weight
    //            picking_plan[item.index] = 1;
    //            current_weight += item.weight;
    //        }
    //    }
    //}

    vector<tuple<int, int, int, int>> items_sort = items;

    sort(items_sort.begin() + 1, items_sort.end(),
        [](const tuple<int, int, int, int>& a, const tuple<int, int, int, int>& b) {
        return (static_cast<double>(get<1>(a)) / static_cast<double>(get<2>(a))) >
            (static_cast<double>(get<1>(b)) / static_cast<double>(get<2>(b)));
        });

    for (auto& it : items_sort)
    {
        // Random value generator
        double r = dis(gen);
        const double r_check = 0.3;
        if (r > r_check && current_weight + get<2>(it) <= capacity)
        {
            picking_plan[get<0>(it)] = 1;
            current_weight += get<2>(it);
        }
    }

}


double PSOParticle::calculate_speed(double current_weight) const
{
    // Calculate the speed using the current weight, if the weight is more than capacity then the speed will be minimum
    // otherwise, the speed will be calculated using the formula
    return (current_weight <= capacity) ? (v_max - current_weight * (v_max - v_min) / capacity) : v_min;
}


return_values PSOParticle::evaluate_fitness(const vector<vector<double>>& distances, const vector<tuple<int, int, int, int>>& items,
    double capacity, double rent_rate, double v_max, double v_min)
{
    // Initialise the total profit, travel time and current weight for the tour
    double total_profit = 0;
    double travel_time = 0;
    double current_weight = 0;

    for (size_t i = 0; i < tour.size(); ++i)
    {
        // Get the current city
        size_t current_city = tour[i];

        // Get the next city in tour
        size_t next_city = tour[(i + 1) % tour.size()];

        // Calculate distance to next city
        double distance = distances[current_city][next_city];

        // Update weight and profit based on picking plan
        for (const auto& item : items_dict[current_city])
        {
            // Check if item is in the picking plan and if the current weight of the knapsack is less than the capacity
            if (picking_plan[item.index] == 1 && current_weight + item.weight <= capacity)
            {
                // Add the current weight of item to current knapsack weight
                current_weight += item.weight;

                // Add the value of item to the total profit of knapsack
                total_profit += item.profit;
            }
        }

        // Calculate the speed from the current knapsack weight
        double speed = calculate_speed(current_weight);

        // Add the travel time between cities to the total travel time
        travel_time += distance / speed;
    }

    // Calculate fitness using total profit, rent rate and travel time
    double fitness = total_profit - rent_rate * travel_time;

    // Update the fitness and position, if the current fitness is greater than the best fitness
    if (fitness > best_fitness)
    {
        best_fitness = fitness;
        best_position = { tour, picking_plan };
    }

    // Return the best fitness, profit, current weight of knapsack and travel time
    return return_values{ best_fitness, total_profit, current_weight, travel_time };
}


void PSOParticle::update_position(const pair<vector<int>, vector<double>>& global_best, double w, double c1, double c2)
{
    // Generate a random number using uniform distribution
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    for (size_t i = 0; i < tour.size(); ++i)
    {
        // Generate random numbers
        double r1 = RandomFloat(0,1);
        double r2 = RandomFloat(0, 1);

        // Calculate the first part of velocity that is the tour using the personal best and global best positions
        velocity[i] = w * velocity[i] +
                    c1 * r1 * (best_position.first[i] - tour[i]) +
                    c2 * r2 * (global_best.first[i] - tour[i]);

        tour[i] = (tour[i] + static_cast<int>(velocity[i])) % tour.size();
    }

    for (size_t i = 0; i < picking_plan.size(); ++i)
    {
        double r1 = RandomFloat(0, 1);
        double r2 = RandomFloat(0, 1);

        // Calculate the second part of velocity that is the picking plan using the personal best and global best positions
        velocity[tour.size()-1 + i] = w * velocity[tour.size() + i] +
                                    c1 * r1 * (best_position.second[i] - picking_plan[i]) +
                                    c2 * r2 * (global_best.second[i] - picking_plan[i]);

        picking_plan[i] = (1 / (1 + exp(-velocity[tour.size() + i]))) > 0.5 ? 1 : 0;
    }
}

//------------------------------------------------------------------------------------------------------------------------
// PSO class
//------------------------------------------------------------------------------------------------------------------------

PSO::PSO(size_t num_particles, const vector<vector<double>>& distances, const vector<tuple<int, int, int, int>>& items, 
    size_t num_cities, size_t num_items, double capacity, double v_max, double v_min, double rent_rate)
    :num_particles(num_particles), distances(distances), items(items), num_cities(num_cities), num_items(num_items), 
    capacity(capacity), v_max(v_max), v_min(v_min), rent_rate(rent_rate)
{
    // Initialise the global best fitness, profit and time
    global_best_fitness = -1e9;
    global_best_profit = -1e9;
    global_best_time = -1e9;

    // Initialise the PSOParticles
    for (size_t i = 0; i < num_particles; ++i)
    {
        particles.emplace_back(distances, items, num_cities, num_items, capacity, v_max, v_min, rent_rate);
    }

    // Initialise the global best
    global_best = {vector<int>(num_cities, 0), vector<double>(num_items, 0.0)};
}


void PSO::update_particle_position(double w, double c1, double c2)
{
    // Vector of threads
    vector<thread> threads;

    // Loop over all the particles
    for (auto& particle : particles)
    {
        // Initialise the threads to update the particle position
        threads.emplace_back([&]() { particle.update_position(global_best, w, c1, c2); });
    }

    // Loop over all the threads
    for (auto& thread : threads)
    {
        // Check if the thread is joinable
        if (thread.joinable())
        {
            // Join or wait for the thread execution
            thread.join();
        }
    }
}


void PSO::evaluate_particle_fitness(double w, double c1, double c2)
{
    // Vector of threads
    vector<thread> threads;

    // Loop over all the particles
    for (auto& particle : particles)
    {
        // Initialise the threads to evaluate the fitness of particles
        threads.emplace_back([&]() {
            auto values = particle.evaluate_fitness(distances, items, capacity, rent_rate, v_max, v_min);

            // Add lock guard to protect the shared data, travel time and profit list
            lock_guard<mutex> lock(mtx);
            
            if (values.fitness > global_best_fitness)
            {
                global_best_fitness = values.fitness;
                global_best = particle.get_best_position();
                
                // Push the values in the list after execution
                travel_time_list.push_back(values.time);
                profit_list.push_back(values.profit);
            }

        });
    }

    // Loop over all the threads
    for (auto& thread : threads)
    {
        // Check if the thread is joinable
        if (thread.joinable())
        {
            // Join or wait for the thread execution
            thread.join();
        }
    }
}


tuple<vector<double>, vector<double>> PSO::run(size_t iterations, double w, double c1, double c2)
{
    // Loop for number of 'iterations'
    for (size_t iter = 0; iter < iterations; ++iter)
    {
        // Evaluate the particle fitness
        evaluate_particle_fitness(w, c1, c2);

        // Update the particle position
        update_particle_position(w, c1, c2);        
    }

    // Return the travel time and profit lists
    return { travel_time_list, profit_list };
}