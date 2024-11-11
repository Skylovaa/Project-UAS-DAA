#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <limits>

using namespace std;

// Define a structure for state variables
struct State {
    string origin;
    string transit;
    string destination;
    string maskapai;
    string daerahTujuan;
    string trafficCondition;
    string weatherCondition;
    bool vehicleAvailable;
    bool personnelAvailable;
    string additionalNotes;
    int runwayOccupancyTime;  // In minutes
    int estimatedTime;        // In minutes

    bool operator==(const State& other) const {
        return origin == other.origin &&
               destination == other.destination &&
               trafficCondition == other.trafficCondition &&
               weatherCondition == other.weatherCondition &&
               vehicleAvailable == other.vehicleAvailable &&
               personnelAvailable == other.personnelAvailable;
    }
};

// Hash function for unordered_map
struct StateHasher {
    size_t operator()(const State& state) const {
        return hash<string>()(state.origin) ^ hash<string>()(state.destination) ^
               hash<string>()(state.trafficCondition) ^ hash<string>()(state.weatherCondition) ^
               hash<bool>()(state.vehicleAvailable) ^ hash<bool>()(state.personnelAvailable);
    }
};

// Define a structure for action
struct Action {
    int delay;  // Delay in minutes
};

// Reward function
int rewardFunction(const State& state, const Action& action) {
    int penalty = action.delay;  // Penalize based on delay
    if (state.trafficCondition == "Heavy") penalty += 20;
    if (state.weatherCondition == "Rain" || state.weatherCondition == "Fog") penalty += 15;
    if (!state.vehicleAvailable) penalty += 25;
    if (!state.personnelAvailable) penalty += 10;

    return -penalty;  // Minimize penalty (maximize reward)
}

// Policy improvement using ADP
Action policyImprovement(const State& state, unordered_map<State, int, StateHasher>& valueFunction) {
    vector<Action> actions = {{0}, {10}, {20}, {30}};  // Possible delays
    Action bestAction = actions[0];
    int bestValue = rewardFunction(state, bestAction);

    for (const auto& action : actions) {
        int value = rewardFunction(state, action);
        if (value > bestValue) {
            bestValue = value;
            bestAction = action;
        }
    }

    valueFunction[state] = bestValue;  // Update value function
    return bestAction;
}

// Adaptive Dynamic Programming function with iterative policy evaluation
void adaptiveDynamicProgramming(vector<State>& states, int iterations = 10) {
    unordered_map<State, int, StateHasher> valueFunction;

    // Iterate over several rounds to improve policy
    for (int iter = 0; iter < iterations; ++iter) {
        cout << "Iteration: " << iter + 1 << endl;

        for (const auto& state : states) {
            Action bestAction = policyImprovement(state, valueFunction);
            cout << "Optimal delay for flight from " << state.origin << " to "
                 << state.destination << " in iteration " << iter + 1 << " is: "
                 << bestAction.delay << " minutes with reward: " << valueFunction[state] << endl;
        }
        cout << "----------------------------------" << endl;
    }
}

int main() {
    ifstream file("C:\\Users\\Karina Amalia H\\Downloads\\DATA2 (1).csv");  // Adjust the path as necessary
    string line;
    vector<State> states;

    // Check if file is open
    if (!file.is_open()) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    // Skip the header line if needed
    getline(file, line);

    // Read the data line by line
    while (getline(file, line)) {
        stringstream s(line);
        State state;
        string runwayTimeStr, estimatedTimeStr;
        string vehicle, personnel;

        getline(s, state.origin, ',');
        getline(s, state.transit, ',');
        getline(s, state.destination, ',');
        getline(s, state.maskapai, ',');
        getline(s, state.daerahTujuan, ',');
        getline(s, state.trafficCondition, ',');
        getline(s, state.weatherCondition, ',');
        getline(s, vehicle, ',');
        getline(s, personnel, ',');
        getline(s, state.additionalNotes, ',');
        getline(s, runwayTimeStr, ',');
        getline(s, estimatedTimeStr, ',');

        state.vehicleAvailable = (vehicle == "yes");
        state.personnelAvailable = (personnel == "yes");

        // Parse estimated time values
        if (!runwayTimeStr.empty()) {
            state.runwayOccupancyTime = stoi(runwayTimeStr);
        } else {
            state.runwayOccupancyTime = 0;  // Default or handle missing data
        }

        if (!estimatedTimeStr.empty()) {
            size_t hoursPos = estimatedTimeStr.find("jam");
            size_t minutesPos = estimatedTimeStr.find("menit");
            int hours = 0, minutes = 0;
            if (hoursPos != string::npos) {
                hours = stoi(estimatedTimeStr.substr(0, hoursPos));
            }
            if (minutesPos != string::npos) {
                minutes = stoi(estimatedTimeStr.substr(hoursPos + 3, minutesPos - hoursPos - 3));
            }
            state.estimatedTime = hours * 60 + minutes;
        } else {
            state.estimatedTime = 0;  // Default or handle missing data
        }

        // Add to states vector
        states.push_back(state);
    }

    adaptiveDynamicProgramming(states);

    return 0;
}
