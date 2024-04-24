#include <iostream>
#include <queue>
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

int main() {
    cout << "hello world" << "\n";

    return 0;
}