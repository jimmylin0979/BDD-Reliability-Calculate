#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

// the info reads from txt
int NODES = 0;
vector<vector<bool>> GRAPHS;
vector<double> PROBS;

vector<vector<string>> paths;   // state {0: disconnected, 1: connected, -1: don't care}
void ReliabilityFromGraph(vector<int>& from, int target, vector<vector<bool>>& Graph, vector<string> path) {
    if (from.back() == target) {
        // if nodes already reach the 'to' node the last time
        // then dfs done
        paths.push_back(path);
        return;
    }

    for (int r : from) {
        for (int c = 0; c < NODES; c++) {
            if (Graph[r][c]) {
                // dfs the new state
                bool state_RC = Graph[r][c];
                bool state_CR = Graph[c][r];
                Graph[r][c] = false;
                Graph[c][r] = false;

                from.push_back(c);

                // set curPath to path
                char buff[100];
                snprintf(buff, sizeof(buff), "%2d %2d %d", r, c, 1);
                string curPathTure = buff;
                snprintf(buff, sizeof(buff), "%2d %2d %d", r, c, 0);
                string curPathFalse = buff;

                vector<string> path_True = path;
                vector<string> path_False = path;
                path_True.push_back(curPathTure);
                path_False.push_back(curPathFalse);

                // dfs
                ReliabilityFromGraph(from, target, Graph, path_True);
                ReliabilityFromGraph(from, target, Graph, path_False);

                // restore the state
                Graph[r][c] = state_RC;
                Graph[c][r] = state_CR;

                from.pop_back();
            }
        }
    }
}

void ReadFromFile(string filepath) {
    ifstream infile;
    infile.open(filepath);

    // 1st line => NODES
    infile >> NODES;

    // // 2nd line => PROBS, probability of each nodes
    // PROBS = vector<double>(NODES, 0.0f);
    // for (int i = 0; i < NODES; i++)
    //     infile >> PROBS[i];

    GRAPHS = vector<vector<bool>>(NODES, vector<bool>(NODES));
    for (int r = 0; r < NODES; r++) {
        for (int c = 0; c < NODES; c++) {
            int tmp = 0;
            infile >> tmp;
            if (tmp)
                GRAPHS[r][c] = true;
            else
                GRAPHS[r][c] = false;
        }
    }

    infile.close();
}

int main() {
    /* read graph with adj-matrix */
    ReadFromFile("./graph.txt");
    // debug for ReadFromFile
    cout << NODES << endl;
    for (int r = 0; r < NODES; r++) {
        for (int c = 0; c < NODES; c++) {
            if (GRAPHS[r][c])
                cout << "1 ";
            else
                cout << "0 ";
        }
        cout << endl;
    }

    /* calculate for the Reliability */
    vector<int> from;
    ReliabilityFromGraph(from, 3, GRAPHS, {});

    cout << "DONE\n";
    for (vector<string> p : paths) {
        for (string s : p) {
            cout << s << "-";
        }
        cout << "----------------------------------------" << endl;
    }
}