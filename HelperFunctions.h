#pragma once
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <regex>
#include <vector>

using namespace std;

struct ParsedData {
    map<string, double> metadata;
    vector<pair<int, int>> nodes;
    vector<tuple<int, int, int, int>> items;
};

vector<vector<double>> create_distance_matrix(const vector<pair<int, int>>& coordinates);

double RandomFloat(double a, double b);

ParsedData parse_bttp_file(const string& file_path);