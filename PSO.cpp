#include <chrono>
#include <ctime>
#include <filesystem>
#include "HelperClasses.h"
#include "HelperFunctions.h"

using namespace std;

int main()
{
    // Input directory for the test files
    const string input_directory = "tests/";

    // Output directory for the results
    const string output_directory = "results/";

    // Create output directory if it doesn't exists
    filesystem::create_directory(output_directory);

    // Loop over all the test files
    for (const auto& entry : filesystem::directory_iterator(input_directory))
    {
        // Start the timer for checking execution time of algorithm
        auto start = chrono::system_clock::now();

        // Test file path
        string file_path = entry.path().string();

        cout << "-----------------------------------------------------------------------------------------------" << endl;
        cout << entry.path().filename().string() << endl;
        cout << "-----------------------------------------------------------------------------------------------" << endl;
        
        // Write the start time to output stream
        time_t in_time_t = chrono::system_clock::to_time_t(start);
        struct tm buf;
        localtime_s(&buf, &in_time_t);
        cout << "Start time: " << put_time(&buf, "%Y-%m-%d %X") << endl;
        
        // Parse the data from test file, and get all the relevant information
        ParsedData parsed_data = parse_bttp_file(file_path);

        // Store the number of cities and items
        size_t num_cities = (size_t)parsed_data.metadata["DIMENSION"];
        size_t num_items = (size_t)parsed_data.metadata["NUMBER_OF_ITEMS"]+1;
        
        // Create the distance matrix, containing distance of each node from other
        vector<vector<double>> distances = create_distance_matrix(parsed_data.nodes);

        // Define all the constants
        const size_t num_iterations = 2;
        const size_t num_particles = 20;
        const double w = 0.9; // Inertia weight
        const double c1 = 1.4; // Acceleration coefficient for personal best
        const double c2 = 1.5; // Acceleration coefficient for global best

        // Initialise the PSO
        PSO pso(num_particles, distances, parsed_data.items, num_cities, num_items, parsed_data.metadata["CAPACITY"],
            parsed_data.metadata["MAX_SPEED"], parsed_data.metadata["MIN_SPEED"],
            parsed_data.metadata["RENTING_RATIO"]);

        // Run the PSO algorithm
        tuple<vector<double>, vector<double>> ret_values = pso.run(num_iterations, w, c1, c2);

        // Store the outputs, travel time and profit list
        vector<double> travel_time_list = get<0>(ret_values), profit_list = get<1>(ret_values);

        // Open the output file for append
        ofstream output_file(output_directory + entry.path().filename().string(), ios::app);
        if (output_file.is_open())
        {
            // Loop over the output lists
            for (size_t i = 0; i < profit_list.size(); ++i)
            {
                if (profit_list[i] != 0)
                {
                    // Write the travel time and profit list in output file
                    output_file << travel_time_list[i] << ' ' << profit_list[i] << endl;
                    cout << "Travel time: " << travel_time_list[i] << " , " << "Profit: " << profit_list[i] << endl;
                }
            }
        }

        // End time after completing the execution
        auto end = chrono::system_clock::now();
        auto elapsed = end - start;
        cout << endl << "Execution time: " << chrono::duration_cast<chrono::duration<double>>(elapsed).count() << '\n';
    }

    return 0;
}