#include <iostream>
#include <queue>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <tuple>
#include <stdlib.h>

using namespace std;

//ALL STANDARD PREP THINGS

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
    int g_n = 0; // cost of each step
    int h_n = 0; // cost of heuristic
    int cost = g_n + h_n;
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

//START OF UCS CODE
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
            new_state.g_n++; 
            neighbors.push_back(new_state);
        }
    }
    return neighbors;
}

vector<string> uniform_cost_search(const vector<vector<int>>& initial_board) { //frontier and implementation of ucs algo
    priority_queue<State> frontier;  // priority queue for UCS
    unordered_set<vector<vector<int>>, BoardHash> visited;  // visited states
    int count = 1;
    int max_frontier = frontier.size();
    // initialize the initial state
    State initial;
    initial.board = initial_board;
    initial.g_n = 0;

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
            cout << "The best state to expand " << count << " with g(n)= " << current.g_n << " and h(n)= " << current.h_n << "\n";
            for (int i = 0; i < current.board.size(); i++) { 
                for (int j = 0; j < current.board[i].size(); j++) {
                    cout << current.board[i][j] << " "; 
                }
                cout << endl; 
            }
            cout << "The maximum number of nodes in the queue at any one time: " << max_frontier << endl;
            return current.path;
        }

        // if not visited, add to visited set and explore neighbors
        if (visited.find(current.board) == visited.end()) {
            visited.insert(current.board);
            /*Below displays every possible board created to reach goal state*/
            cout << "The best state to expand " << count << " with g(n)= " << current.g_n << " and h(n)= " << current.h_n << "\n";
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
        cout << "frontier size = " << frontier.size() << endl;
        if (max_frontier < frontier.size()) {
            max_frontier = frontier.size();
        }
    }

    return {};
}

void UCS(const vector<vector<int>>& initial_board) { //says whether to solution is found or not. if found it prints the trace
    vector<vector<int>> goal_state_board = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 0}
    };
    
    if(initial_board == goal_state_board) {
        cout << "\n" << "Goal!!!" << "\n" << "\n" << "The path of Steps taken: none" << "\n";
        cout << "The depth of the goal node was 1" << "\n";
    }
    else {
        auto path = uniform_cost_search(initial_board);
    
        int count = 0;
        if (!path.empty()) {
            cout << "\n" << "Goal!!!" << "\n" << "\n" << "The path of Steps taken: ";
            for (const auto& step : path) {
                cout << step << " ";
                ++count;
            }
            cout << endl;
            cout << "The depth of the goal node was " << count << "\n";
        } else {
            cout << "No solution found." << endl;
        }
    }   
}
// END OF UCS CODE


//START OF A* EXLUSIVE CODE
priority_queue<State> frontier_init(const vector<vector<int>>& initial_board) { //creates the frontier queue (A* exclusive)
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
//END OF A* EXCLUSIVE CODE


// START OF MISPLACED -- DOUBLE CHECK TO MATCH SOLUTIONS

int total_misplaced_tiles(const State& state) { // calculation for the misplaced tiles
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

vector <State> get_misplaced_neighbors(const State& state) { //finds the different possible boards of each current state
    vector<State> neighbors;
    int row = state.zero_row;
    int col = state.zero_col;
    int g_n = state.g_n;
    int h_n = 0;

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
            g_n++;
            h_n = total_misplaced_tiles(state) - 1;
            new_state.g_n = g_n;
            new_state.h_n = h_n;
            new_state.cost = g_n + h_n;
            neighbors.push_back(new_state);
        }
    }

    return neighbors;
}

vector<string> misplace_tile_search(const vector<vector<int>>& initial_board) { // says whether it found the solution or not. if so print trace
    unordered_set<vector<vector<int>>, BoardHash> visited;  // visited states
    priority_queue<State> frontier = frontier_init(initial_board);
    int count = 1;
    int max_frontier = frontier.size();

    while (!frontier.empty()) {
        State current = frontier.top();
        frontier.pop();
        if (max_frontier < frontier.size()) {
        max_frontier = frontier.size();
        }

        // if the goal is reached, return the path
        if (is_goal(current.board)) {
            cout << "The best state to expand " << count << " with g(n)= " << current.g_n << " and h(n)= " << current.h_n << "\n";
            for (int i = 0; i < current.board.size(); i++) { 
                for (int j = 0; j < current.board[i].size(); j++) {
                    cout << current.board[i][j] << " "; 
                }
                cout << endl; 
            }
            cout << "The maximum number of nodes in the queue at any one time: " << max_frontier << endl;
            return current.path;
        }

        // if not visited, add to visited set and explore neighbors
        if (visited.find(current.board) == visited.end()) {
            visited.insert(current.board);
            
            cout << "The best state to expand " << count << " with g(n)= " << current.g_n << " and h(n)= " << current.h_n << "\n";
            for (int i = 0; i < current.board.size(); i++) { 
                for (int j = 0; j < current.board[i].size(); j++) {
                    cout << current.board[i][j] << " "; 
                }
                cout << endl; 
            }
            ++count;
            auto neighbors = get_misplaced_neighbors(current);
            for (const auto& neighbor : neighbors) {
                if (visited.find(neighbor.board) == visited.end()) {
                    frontier.push(neighbor);
                }
            }
        }
    }

    return {};
}

void misplaced(const vector<vector<int>>& initial_board) { //says whether to solution is found or not. if found it prints the trace
    vector<vector<int>> goal_state_board = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 0}
    };
    
    if(initial_board == goal_state_board) {
        cout << "\n" << "Goal!!!" << "\n" << "\n" << "The path of Steps taken: none" << "\n";
        cout << "The depth of the goal node was 1" << "\n";
    }
    else {
        auto path = misplace_tile_search(initial_board);
    
        int count = 0;
        if (!path.empty()) {
            cout << "\n" << "Goal!!!" << "\n" << "\n" << "The path of Steps taken: ";
            for (const auto& step : path) {
                cout << step << " ";
                ++count;
            }
            cout << endl;
            cout << "The depth of the goal node was " << count << "\n";
        } else {
            cout << "No solution found." << endl;
        }
    }   
}

//END OF MISPLACED




//START OF EUCLIDEANS --DOUBLE CHECK TO MATCH SOLUTIONS

int euclidean_calc (const State& state){ //calculation for euclidean
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

vector<State> get_euclidean_neighbors(const State& state) { //finds the different possible boards of each current state
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
            g_n++;
            h_n = euclidean_calc(state);
            new_state.g_n = g_n;
            new_state.h_n = h_n;
            new_state.cost = g_n + h_n;
            
            neighbors.push_back(new_state);
        }
    }
     
    return neighbors;
}

vector<string> euclidean_distance_search(const vector<vector<int>>& initial_board) { //says whether it found the solution or not. if so print trace
    unordered_set<vector<vector<int>>, BoardHash> visited;
    priority_queue<State> frontier = frontier_init(initial_board);
    int count = 1;
    int max_frontier = frontier.size();
    while(!frontier.empty()) {
        State current = frontier.top();
        frontier.pop();
        if (max_frontier < frontier.size()) {
            max_frontier = frontier.size();
        }

        if(is_goal(current.board)) {
            cout << "The best state to expand " << count << " with g(n)= " << current.g_n << " and h(n)= " << current.h_n << "\n";
            for (int i = 0; i < current.board.size(); i++) { 
                for (int j = 0; j < current.board[i].size(); j++) {
                    cout << current.board[i][j] << " "; 
                }
                cout << endl; 
            }
                cout << "The maximum number of nodes in the queue at any one time: " << max_frontier << endl;

            return current.path;
        }

        if (visited.find(current.board) == visited.end()) {
            visited.insert(current.board);
            cout << "The best state to expand " << count << " with g(n)= " << current.g_n << " and h(n)= " << current.h_n << "\n";
            for (int i = 0; i < current.board.size(); i++) { 
                for (int j = 0; j < current.board[i].size(); j++) {
                    cout << current.board[i][j] << " "; 
                }
                cout << endl; 
            }
            ++count;
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

void euclidean(const vector<vector<int>>& initial_board) { //says whether to solution is found or not. if found it prints the trace
    vector<vector<int>> goal_state_board = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 0}
    };
    
    if(initial_board == goal_state_board) {
        cout << "\n" << "Goal!!!" << "\n" << "\n" << "The path of Steps taken: none" << "\n";
        cout << "The depth of the goal node was 1" << "\n";
    }
    else {
        auto path = euclidean_distance_search(initial_board);
    
        int count = 0;
        if (!path.empty()) {
            cout << "\n" << "Goal!!!" << "\n" << "\n" << "The path of Steps taken: ";
            for (const auto& step : path) {
                cout << step << " ";
                ++count;
            }
            cout << endl;
            cout << "The depth of the goal node was " << count << "\n";
        } else {
            cout << "No solution found." << endl;
        }
    }   
}

//END OF EUCLIDEANS

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
        if (choice == 2) {
            misplaced(initial_state_board);
        }

        if (choice == 3) {
            euclidean(initial_state_board);
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

        if (choice == 2) {
            misplaced(initial_state_board);
        }

        if (choice == 3) {
            euclidean(initial_state_board);
        }
    }

    return 0;
}
