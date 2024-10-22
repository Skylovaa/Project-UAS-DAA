#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// Function to solve the Knapsack problem using Dynamic Programming
int knapsack(int W, const vector<int>& weights, const vector<int>& values, int n) {
    // Create a DP table to store results of subproblems
    vector<vector<int>> dp(n + 1, vector<int>(W + 1));

    // Build the table dp[][] in bottom-up manner
    for (int i = 0; i <= n; i++) {
        for (int w = 0; w <= W; w++) {
            if (i == 0 || w == 0)
                dp[i][w] = 0;
            else if (weights[i - 1] <= w)
                dp[i][w] = max(values[i - 1] + dp[i - 1][w - weights[i - 1]], dp[i - 1][w]);
            else
                dp[i][w] = dp[i - 1][w];
        }
    }

    return dp[n][W]; // The maximum value that can be put in a knapsack of capacity W
}

int main() {
    int n, W;
    cout << "Enter number of items: ";
    cin >> n;
    
    vector<int> weights(n), values(n);
    
    cout << "Enter the capacity of the knapsack: ";
    cin >> W;
    
    cout << "Enter the weights of the items:\n";
    for (int i = 0; i < n; i++) {
        cin >> weights[i];
    }
    
    cout << "Enter the values of the items:\n";
    for (int i = 0; i < n; i++) {
        cin >> values[i];
    }

    // Call knapsack function and output the result
    cout << "Maximum value in Knapsack = " << knapsack(W, weights, values, n) << endl;
    
    return 0;
}
