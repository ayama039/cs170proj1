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

int euclidean (const State& state){
    vector<vector<int>> g_board = 
    {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 0}
    };
    State goal;
    goal.board = g_board;
    goal.cost = 0;
    double distance = 0;
    int n = state.board.size();

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (state.board[i][j] != 0) {
                int goal_i = (state.board[i][j] - 1) / n;
                int goal_j = (state.board[i][j] - 1) % n;
                distance += sqrt(pow(i - goal_i, 2) + pow(j - goal_j, 2));
            }
        }
    }
    return distance;
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


vector<State> get_euclidean_neighbors(const State& state) {
    // cout << "in get_neighbors" << endl;
    vector<State> neighbors;
    int row = state.zero_row;
    int col = state.zero_col;
    int g_n = state.cost;
    int h_n = 0;

    vector<tuple<int, int, string>> moves = {
        {-1, 0, "Up"},
        {1, 0, "Down"},
        {0, -1, "Left"},
        {0, 1, "Right"}
    };
    //won't create copies and won't let modifications happen
    //element : initial list
    for (const auto& move : moves) {
        // cout << "row: " << row << "    column: " << col << endl;
        int new_row = row + get<0>(move);
        int new_col = col + get<1>(move);

        // cout << "new row:   " << new_row << "   column: " << new_col << endl;

        if (new_row >= 0 && new_row < 3 && new_col >= 0 && new_col < 3) {
            State new_state = state;
            new_state.board[row][col] = new_state.board[new_row][new_col];
            new_state.board[new_row][new_col] = 0;
            new_state.zero_row = new_row;
            new_state.zero_col = new_col;
            new_state.path.push_back(get<2>(move));
            g_n++;
            h_n = euclidean(state);
            new_state.cost = g_n + h_n;
            


            // cout << "new state cost: " << new_state.cost << endl;

            neighbors.push_back(new_state);
            /*for (int i = 0; i < new_state.board.size(); i ++) {
                cout << "row" << i << ": ";
                for (int j= 0; j < new_state.board[i].size(); j++) {
                    cout << new_state.board[i][j] << " ";
                }
                cout << endl;
            }
            cout << endl << endl; */
           
        }
    }
     
    return neighbors;
}




priority_queue<State> frontier_init(const vector<vector<int>>& initial_board) {
    priority_queue<State> frontier;  // priority queue for UCS

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
    return frontier;
}

vector<string> uniform_cost_search(const vector<vector<int>>& initial_board) {
    unordered_set<vector<vector<int>>, BoardHash> visited;  // visited states
    priority_queue<State> frontier = frontier_init(initial_board);

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

vector<string> euclidean_distance_search(const vector<vector<int>>& initial_board) {
    unordered_set<vector<vector<int>>, BoardHash> visited;
    priority_queue<State> frontier = frontier_init(initial_board);

    while(!frontier.empty()) {
        State current = frontier.top();
        frontier.pop();

        if(is_goal(current.board)) {
            return current.path;
        }

        if (visited.find(current.board) == visited.end()) {
            visited.insert(current.board);

            auto neighbors = get_euclidean_neighbors(current);
            for(const auto& neighbor : neighbors) {
                if(visited.find(neighbor.board) == visited.end()){
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

    auto path = euclidean_distance_search(initial_board);

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