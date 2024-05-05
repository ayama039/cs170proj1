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
    vector<vector<int>> board; // The actual 8-puzzle Board
    int zero_row, zero_col;
    int cost; // Records how many moves it takes to reach final State
    vector<string> path;  // directions taken to reach this state

    bool operator<(const State& other) const {
        return cost > other.cost; 
    }
};


bool is_goal(vector<vector<int>>& board) { //Checks if the current state of the board is equal to the Goal State

    vector<vector<int>> goal = 
    {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 0}
    };

    return board == goal;
}


vector<State> get_neighbors(const State& state) { //Creates the different possible boards after each current state
    vector<State> neighbors;
    int row = state.zero_row;
    int col = state.zero_col;

    vector<tuple<int, int, string>> moves = {  //Possible moves for the 8-puzzle board 
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
    int count = 1;
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
            /*Displays Goal State Board*/
            cout << "Board " << count << " with cost: " << current.cost <<"\n";
            for (int i = 0; i < current.board.size(); i++) { 
                for (int j = 0; j < current.board[i].size(); j++) {
                    cout << current.board[i][j] << " "; 
                }
                cout << endl; 
            }
            return current.path;
        }

        // if not visited, add to visited set and explore neighbors
        if (visited.find(current.board) == visited.end()) {
            visited.insert(current.board);
            /*Below displays every possible board created to reach goal state*/
            cout << "Board " << count << " with cost: " << current.cost <<"\n";
            for (int i = 0; i < current.board.size(); i++) { 
                for (int j = 0; j < current.board[i].size(); j++) {
                    cout << current.board[i][j] << " "; 
                }
                cout << endl; 
            }
            ++count;
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

void UCS(const vector<vector<int>>& initial_board) {
    vector<vector<int>> goal_state_board = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 0}
    };
    
    if(initial_board == goal_state_board) {
        cout << "Solution found!" << "\n" << "The path of Steps taken: none" << "\n";
        cout << "Numnber of steps taken to reach Goal state: 1" << "\n";
    }
    else {
        auto path = uniform_cost_search(initial_board);
    
        int count = 0;
        if (!path.empty()) {
            cout << "Solution found!" << "\n" << "The path of Steps taken: ";
            for (const auto& step : path) {
                cout << step << " ";
                ++count;
            }
            cout << endl;
            cout << "Numnber of steps taken to reach Goal state: " << count << "\n";
        } else {
            cout << "No solution found." << endl;
        }
    }   
}

int main() {
    cout << "Welcome to Our 8-puzzle solver" << "\n";
    cout << "Type 1 to use the Default 8-puzzle, or Type 2 to enter your own 8-puzzle" << "\n";

    int choice = 0;

    cin >> choice;

    if(choice == 1) {
        choice = 0;
        vector<vector<int>> initial_state_board = {
            {1, 0, 3},
            {4, 2, 6},
            {7, 5, 8}
        };

        cout << "Choose the algorithm to use:" << "\n";
        cout << "(1) Uniform Cost Search" << "\n";
        cout << "(2) A* search with the Misplaced Tile heuristic" << "\n";
        cout << "(3) A* search with the Euclidean distance heuristic" << "\n";
        
        cin >> choice;
        
        if(choice == 1) {
            UCS(initial_state_board);
        }
    }
    else if (choice == 2) {
        vector<vector<int>> initial_state_board;
        int one, two, three = 0;
        
        cout << "Enter your puzzle, use a zero to represent the blank" << "\n";
        cout << "Enter the first row, use space or tabs between numbers ";
        
        cin >> one >> two >> three;
        vector<int> row1;
        row1.push_back(one);
        row1.push_back(two);
        row1.push_back(three);
        
        cout << "\n";
        cout << "Enter the second row, use space or tabs between numbers ";

        cin >> one >> two >> three;
        vector<int> row2;
        row2.push_back(one);
        row2.push_back(two);
        row2.push_back(three);

        cout << "\n";
        cout << "Enter the third row, use space or tabs between numbers ";

        cin >> one >> two >> three;
        vector<int> row3;
        row3.push_back(one);
        row3.push_back(two);
        row3.push_back(three);
        cout << "\n";

        initial_state_board.push_back(row1);
        initial_state_board.push_back(row2);
        initial_state_board.push_back(row3);

        for (int i = 0; i < initial_state_board.size(); i++) { 
            for (int j = 0; j < initial_state_board[i].size(); j++) {
                cout << initial_state_board[i][j] << " "; 
            }
            cout << endl; 
        }
        cout << "\n";

        cout << "Choose the algorithm to use:" << "\n";
        cout << "(1) Uniform Cost Search" << "\n";
        cout << "(2) A* search with the Misplaced Tile heuristic" << "\n";
        cout << "(3) A* search with the Euclidean distance heuristic" << "\n";
        
        cin >> choice;
        
        if(choice == 1) {
            UCS(initial_state_board);
        }
    }

    return 0;
}
