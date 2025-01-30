#include "HelperFunctions.h"

// Function to create distance matrix
vector<vector<double>> create_distance_matrix(const vector<pair<int, int>>& coordinates)
{
    size_t num_cities = coordinates.size();
    vector<vector<double>> distances(num_cities, vector<double>(num_cities, 0.0));

    for (size_t i = 0; i < num_cities; ++i)
    {
        for (size_t j = i + 1; j < num_cities; ++j)
        {
            double x1 = coordinates[i].first;
            double y1 = coordinates[i].second;
            double x2 = coordinates[j].first;
            double y2 = coordinates[j].second;
            double distance = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
            distances[i][j] = distance;
            distances[j][i] = distance; // Matrix is symmetric
        }
    }

    return distances;
}


double RandomFloat(double a, double b)
{
    double random = ((double)rand()) / (double)RAND_MAX;
    double diff = b - a;
    double r = random * diff;
    return a + r;
}


ParsedData parse_bttp_file(const string& file_path)
{
    ParsedData parsed_data;
    parsed_data.metadata = {
        {"DIMENSION", 0},
        {"CAPACITY", 0},
        {"MIN_SPEED", 0.0},
        {"MAX_SPEED", 0.0},
        {"RENTING_RATIO", 0.0}
    };

    parsed_data.items.emplace_back(0, 0, 0, 0);

    ifstream file(file_path);
    if (!file.is_open())
    {
        cerr << "Error opening file: " << file_path << endl;
        exit(1);
    }

    string line;
    bool reading_nodes = false, reading_items = false;

    while (getline(file, line))
    {
        line = regex_replace(line, regex("^ +| +$|( ) +"), "$1");

        // Parse metadata
        if (line.find("DIMENSION") != string::npos)
        {
            parsed_data.metadata["DIMENSION"] = stoi(line.substr(line.find(":\t") + 1));
        }
        else if (line.find("CAPACITY") != string::npos)
        {
            smatch match;
            if (regex_search(line, match, regex("\\d+")))
            {
                parsed_data.metadata["CAPACITY"] = stoi(match.str());
            }
        }
        else if (line.find("MIN SPEED") != string::npos)
        {
            smatch match;
            if (regex_search(line, match, regex("\\d+\\.\\d+")))
            {
                parsed_data.metadata["MIN_SPEED"] = stod(match.str());
            }
        }
        else if (line.find("MAX SPEED") != string::npos)
        {
            smatch match;
            if (regex_search(line, match, regex("\\d+")))
            {
                parsed_data.metadata["MAX_SPEED"] = stod(match.str());
            }
        }
        else if (line.find("NUMBER OF ITEMS") != string::npos)
        {
            smatch match;
            if (regex_search(line, match, regex("\\d+")))
            {
                parsed_data.metadata["NUMBER_OF_ITEMS"] = stod(match.str());
            }
        }
        else if (line.find("RENTING RATIO") != string::npos)
        {
            smatch match;
            if (regex_search(line, match, regex("\\d+\\.\\d+")))
            {
                parsed_data.metadata["RENTING_RATIO"] = stod(match.str());
            }
        }
        else if (line.find("EDGE_WEIGHT_TYPE") != string::npos)
        {
            parsed_data.metadata["EDGE_WEIGHT_TYPE"] = 0; // Placeholder for EDGE_WEIGHT_TYPE
        }
        else if (line.find("NODE_COORD_SECTION") != string::npos)
        {
            reading_nodes = true;
            reading_items = false;
            continue;
        }
        else if (line.find("ITEMS SECTION") != string::npos)
        {
            reading_nodes = false;
            reading_items = true;
            continue;
        }
        else if (line.find("EOF") != string::npos)
        {
            break;
        }

        // Parse nodes (INDEX, X, Y)
        if (reading_nodes)
        {
            istringstream iss(line);
            int index, x, y;

            smatch match;
            if (regex_match(line, match, regex("^(\\d+)\\s+(\\d+)\\s+(\\d+)$")))
            {
                index = stoi(match[1].str());
                x = stoi(match[2].str());
                y = stoi(match[3].str());
            }

            iss >> index >> x >> y;
            parsed_data.nodes.emplace_back(x, y);
        }
        else if (reading_items)
        {
            int index, profit, weight, assigned_node;

            smatch match;
            if (regex_match(line, match, regex("^(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)$")))
            {
                index = stoi(match[1].str());
                profit = stoi(match[2].str());
                weight = stoi(match[3].str());
                assigned_node = stoi(match[4].str()) - 1;
            }

            parsed_data.items.emplace_back(index, profit, weight, assigned_node);
        }
    }

    file.close();
    return parsed_data;
}