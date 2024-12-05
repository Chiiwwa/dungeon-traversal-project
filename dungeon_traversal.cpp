#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <chrono>

// Node structure representing a dungeon room
struct Node {
    int value;
    std::vector<Node*> children;
    Node(int val) : value(val) {}
};

// Traverse the tree using DFS to find the escape node
bool dfs(Node* node, int escapeValue, std::vector<int>& path, int& totalMoves) {
    if (!node) return false;

    path.push_back(node->value);
    totalMoves++;

    if (node->value == escapeValue) {
        std::cout << "Escape node found!\n";
        return true;
    }

    for (Node* child : node->children) {
        if (dfs(child, escapeValue, path, totalMoves)) {
            return true;
        }
        totalMoves++;
    }
    path.pop_back();
    return false;
}

// Generate a random tree with a specific escape node value
Node* generateRandomTree(int nodeCount, int escapeValue, bool& escapeNodePresent) {
    srand(time(0));
    Node* root = new Node(rand() % 100 + 1);
    std::vector<Node*> nodes{root};

    for (int i = 1; i < nodeCount; i++) {
        Node* newNode = new Node(rand() % 100 + 1);
        nodes[rand() % nodes.size()]->children.push_back(newNode);
        nodes.push_back(newNode);
    }

    if (rand() % 2 == 0) {
        escapeNodePresent = false;
        std::cout << "No escape node selected.\n";
    } else {
        nodes[rand() % nodes.size()]->value = escapeValue;
        escapeNodePresent = true;
        std::cout << "Escape node with value " << escapeValue << " added.\n";
    }
    return root;
}

// Read a custom tree from a file
Node* readCustomTreeFromFile(const std::string& filename, int escapeValue) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file.\n";
        return nullptr;
    }

    std::vector<Node*> nodes;
    std::string line;
    Node* root = nullptr;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int parentValue, childValue;
        iss >> parentValue;

        if (nodes.empty()) {
            root = new Node(parentValue);
            nodes.push_back(root);
        }

        Node* parent = nullptr;
        for (Node* node : nodes) {
            if (node->value == parentValue) {
                parent = node;
                break;
            }
        }

        while (iss >> childValue) {
            Node* child = new Node(childValue);
            if (childValue == escapeValue) {
                std::cout << "Escape node found in the custom tree: " << childValue << "\n";
            }
            parent->children.push_back(child);
            nodes.push_back(child);
        }
    }

    file.close();
    return root;
}

// Print the tree
void printTree(Node* node, int depth = 0) {
    if (!node) return;
    std::cout << std::string(depth * 2, '-') << "Node Value: " << node->value << '\n';
    for (Node* child : node->children) {
        printTree(child, depth + 1);
    }
}

int main() {
    int escapeValue = 1000; 
    Node* tree = nullptr;
    bool escapeNodePresent = true;

    std::cout << "Choose an option:\n";
    std::cout << "1. Generate random tree\n";
    std::cout << "2. Use custom tree from file\n";
    int choice;
    std::cin >> choice;

    switch (choice) {
        case 1: {
            int nodeCount = 1000;
            tree = generateRandomTree(nodeCount, escapeValue, escapeNodePresent);
            break;
        }
        case 2: {
            std::string filename;
            std::cout << "Enter the filename: ";
            std::cin >> filename;
            tree = readCustomTreeFromFile(filename, escapeValue);
            if (!tree) {
                std::cerr << "Error loading custom tree.\n";
                return 1;
            }
            break;
        }
        default:
            std::cerr << "Invalid choice.\n";
            return 1;
    }

    int totalMoves = 0;
    std::vector<int> path;

    printTree(tree);
    std::cout << "\nStarting DFS...\n";

    auto start = std::chrono::high_resolution_clock::now();
    bool found = dfs(tree, escapeValue, path, totalMoves);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Total moves: " << totalMoves << '\n';
    std::cout << "Total time taken: " << elapsed.count() << " seconds.\n";

    if (!found) {
        std::cout << "Escape node not found.\n";
    }

    return 0;
}
