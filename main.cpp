#include <iostream>
#include <queue>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <tuple>
#include <stdlib.h>

using namespace std;

class Tree {
    struct Node {
        Node *left;
        Node *right;
        int cost;
    };

    public:
        Tree() {
            root = nullptr;
        }
        void insert(int value) {
            cout << "HELPME";
        }
    private:
        Node *root;
};


struct BoardHash {
    size_t operator()(const vector<vector<int>>& board) const {
        size_t hash = 0;
        for (const auto& row : board) {
            for (int val : row) {
                hash ^= std::hash<int>()(val) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
        }
        return hash;
    }
};

// state representation
struct State {
    vector<vector<int>> board;
    int zero_row, zero_col;
    int cost;
    vector<string> path;  // directions taken to reach this state

    bool operator<(const State& other) const {
        return cost > other.cost; 
    }
};


bool is_goal(vector<vector<int>>& board)
{
    vector<vector<int>> goal = 
    {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 0}
    };

    return board == goal;
}


int total_misplaced_tiles(const State& state)
{
    State curr_state = state;
    vector<vector<int>> goal = 
    {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 0}
    };
    int heuristic_total = 0;
    int i = 0;
    int j = 0;
    while (j < 3) {
        i = 0;
        while (i < 3) {
            if (curr_state.board[i][j] != goal[i][j]) {
            heuristic_total++;
            }
            i++;
        }
        j++;
    }
    return heuristic_total;
}

vector<State> get_neighbors(const State& state) {
    vector<State> neighbors;
    int row = state.zero_row;
    int col = state.zero_col;

    vector<tuple<int, int, string>> moves = {
        {-1, 0, "Up"},
        {1, 0, "Down"},
        {0, -1, "Left"},
        {0, 1, "Right"}
    };

    for (const auto& move : moves) {
        int new_row = row + get<0>(move);
        int new_col = col + get<1>(move);

        if (new_row >= 0 && new_row < 3 && new_col >= 0 && new_col < 3) {
            State new_state = state;
            new_state.board[row][col] = new_state.board[new_row][new_col];
            new_state.board[new_row][new_col] = 0;
            new_state.zero_row = new_row;
            new_state.zero_col = new_col;
            new_state.path.push_back(get<2>(move));
            new_state.cost++;
            neighbors.push_back(new_state);
        }
    }

    return neighbors;
}


vector<string> uniform_cost_search(const vector<vector<int>>& initial_board) {
    priority_queue<State> frontier;  // priority queue for UCS
    unordered_set<vector<vector<int>>, BoardHash> visited;  // visited states

    // initialize the initial state
    State initial;
    initial.board = initial_board;
    initial.cost = 0;

    // find the position of zero
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (initial.board[i][j] == 0) {
                initial.zero_row = i;
                initial.zero_col = j;
            }
        }
    }

    frontier.push(initial);

    while (!frontier.empty()) {
        State current = frontier.top();
        frontier.pop();

        // if the goal is reached, return the path
        if (is_goal(current.board)) {
            return current.path;
        }

        // if not visited, add to visited set and explore neighbors
        if (visited.find(current.board) == visited.end()) {
            visited.insert(current.board);

            auto neighbors = get_neighbors(current);
            for (const auto& neighbor : neighbors) {
                if (visited.find(neighbor.board) == visited.end()) {
                    frontier.push(neighbor);
                }
            }
        }
    }

    return {};
}

int main() {

    vector<vector<int>> initial_board = {
        {6, 8, 4},
        {3, 7, 1},
        {5, 0, 2}
    };

    auto path = uniform_cost_search(initial_board);

    if (!path.empty()) {
        cout << "Solution found! Steps: ";
        for (const auto& step : path) {
            cout << step << " ";
        }
        cout << endl;
    } else {
        cout << "No solution found." << endl;
    }

    return 0;
}