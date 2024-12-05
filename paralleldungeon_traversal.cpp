#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <omp.h>
#include <thread>

struct Node {
    int value;
    std::vector<Node*> children;
    Node(int val) : value(val) {}
};

// Clones the tree to ensure DFS and BFS operate on separate trees
Node* cloneTree(Node* root) {
    if (!root) return nullptr;
    Node* newRoot = new Node(root->value);
    for (Node* child : root->children) {
        newRoot->children.push_back(cloneTree(child));
    }
    return newRoot;
}

// DFS Traversal
bool dfs(Node* node, int escapeValue, int& totalMoves, bool& escapeFound) {
    if (!node || escapeFound) return false;

    totalMoves++;

    if (node->value == escapeValue) {
        #pragma omp atomic write
        escapeFound = true;
        return true;
    }

    for (Node* child : node->children) {
        if (dfs(child, escapeValue, totalMoves, escapeFound)) {
            return true;
        }
    }
    return false;
}

// BFS Traversal
bool bfs(Node* root, int escapeValue, int& totalMoves, bool& escapeFound) {
    std::queue<Node*> q;
    q.push(root);

    while (!q.empty() && !escapeFound) {
        Node* node = q.front();
        q.pop();
        totalMoves++;

        if (node->value == escapeValue) {
            #pragma omp atomic write
            escapeFound = true;
            return true;
        }

        for (Node* child : node->children) {
            q.push(child);
        }
    }
    return false;
}

// Generate a random tree with a specific escape node value
Node* generateRandomTree(int nodeCount, int escapeValue) {
    srand(time(0));
    Node* root = new Node(rand() % 100 + 1);
    std::vector<Node*> nodes{root};

    for (int i = 1; i < nodeCount; i++) {
        Node* newNode = new Node(rand() % 100 + 1);
        nodes[rand() % nodes.size()]->children.push_back(newNode);
        nodes.push_back(newNode);
    }

    nodes[rand() % nodes.size()]->value = escapeValue;  // Assign escape node
    return root;
}

int main() {
    int nodeCount = 1000;
    int escapeValue = 5;
    Node* originalTree = generateRandomTree(nodeCount, escapeValue);

    Node* dfsTree = cloneTree(originalTree);
    Node* bfsTree = cloneTree(originalTree);

    int totalMovesDFS = 0, totalMovesBFS = 0;
    bool escapeFound = false;
    std::string finder = "None";

    std::cout << "\nStarting Parallel DFS and BFS...\n";

    auto start = std::chrono::high_resolution_clock::now();

    int localTotalMovesDFS = 0, localTotalMovesBFS = 0;

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            if (bfs(bfsTree, escapeValue, localTotalMovesBFS, escapeFound)) {
                finder = "BFS";
            }
        }

        #pragma omp section
        {
            if (dfs(dfsTree, escapeValue, localTotalMovesDFS, escapeFound)) {
                finder = "DFS";
            }
        }
    }

    // Combine the local total moves after both sections finish
    totalMovesDFS += localTotalMovesDFS;
    totalMovesBFS += localTotalMovesBFS;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Escape node found by: " << finder << '\n';
    std::cout << "Total moves in DFS: " << totalMovesDFS << '\n';
    std::cout << "Total moves in BFS: " << totalMovesBFS << '\n';
    std::cout << "Total time taken: " << elapsed.count() << " seconds.\n";

    return 0;
}
