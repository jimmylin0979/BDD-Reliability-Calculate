#include <time.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;

// the info reads from txt
int NODES = 0;
vector<vector<bool>> GRAPH;
map<string, int> LINES;
vector<double> PROBS;
vector<string> SOLS_PATH;
vector<double> SOLS_RELI;
set<pair<string, double>> SOLS;

// state {0: disconnected, 1: connected}
void ReliabilityFromGraph(vector<int> reach, int target, vector<vector<bool>> Graph, string curPath, double prob = 1.0f) {
    if (reach.back() == target) {
        // if nodes already reach the 'to' node the last time
        // then dfs done
        int dontCareCnt = 0;
        for (char p : curPath)
            if (p == '-') dontCareCnt++;

        int iterCnt = pow(2, dontCareCnt);
        string iterPoss = string(dontCareCnt, '0');
        for (int iter = 0; iter < iterCnt; iter++) {
            // iterPoss, '000' -> '001' -> '010'
            if (iter != 0) {
                for (int i = dontCareCnt - 1; i >= 0; i--) {
                    if (iterPoss[i] == '0') {
                        iterPoss[i] = '1';
                        break;
                    } else {
                        iterPoss[i] = '0';
                    }
                }
            }

            int cnt = 0;
            string nowPath = curPath;
            double nowProb = prob;
            for (int i = 0; i < nowPath.length(); i++) {
                if (nowPath[i] == '-') {
                    int state = iterPoss[cnt++] - '0';
                    nowPath[i] = (state + '0');
                    nowProb *= (state == 1 ? PROBS[i] : (1 - PROBS[i]));
                }

                if (cnt == dontCareCnt)
                    break;
            }

            // SOLS.insert({nowPath, nowProb});
            if (SOLS_PATH.size() == 0 || find(SOLS_PATH.begin(), SOLS_PATH.end(), nowPath) == SOLS_PATH.end()) {
                SOLS_PATH.push_back(nowPath);
                SOLS_RELI.push_back(nowProb);
            }
        }
        return;
    }

    // dfs main body
    for (int ridx = 0; ridx < reach.size(); ridx++) {
        int r = reach[ridx];
        for (int c = 0; c < NODES; c++) {
            if (Graph[r][c]) {
                // the path choose currently in Graph
                char buff[100];
                sprintf(buff, "%d%d", (r < c) ? r : c, (r > c) ? r : c);
                string PathInStr = buff;
                int choosePath = LINES[PathInStr];

                // dfs the new state
                bool state_RC = Graph[r][c];
                bool state_CR = Graph[c][r];
                Graph[r][c] = false;
                Graph[c][r] = false;

                curPath[choosePath] = '0';
                ReliabilityFromGraph(reach, target, Graph, curPath, prob * (1 - PROBS[choosePath]));

                curPath[choosePath] = '1';
                reach.push_back(c);
                ReliabilityFromGraph(reach, target, Graph, curPath, prob * PROBS[choosePath]);
                reach.pop_back();

                curPath[choosePath] = '-';

                // restore the state
                Graph[r][c] = state_RC;
                Graph[c][r] = state_CR;
            }
        }
    }
}

void ReadFromFile(string filepath) {
    ifstream infile;
    infile.open(filepath);

    // 1st line => NODES
    infile >> NODES;

    // adjacency matrix => GRAPH
    GRAPH = vector<vector<bool>>(NODES, vector<bool>(NODES));
    for (int r = 0; r < NODES; r++) {
        for (int c = 0; c < NODES; c++) {
            int tmp = 0;
            infile >> tmp;
            if (tmp)
                GRAPH[r][c] = true;
            else
                GRAPH[r][c] = false;
        }
    }

    // last line, probability of each nodes => PROBS
    double prob;
    while (infile >> prob) {
        PROBS.push_back(prob);
    }

    infile.close();
}

int main(int argc, char* argv[]) {
    // read filepath as argv
    ReadFromFile(argv[1]);

    // print out HyperParameter -> for debug use
    // cout << NODES << endl;
    // for (int r = 0; r < NODES; r++) {
    //     for (int c = 0; c < NODES; c++) {
    //         if (GRAPH[r][c])
    //             cout << "1 ";
    //         else
    //             cout << "0 ";
    //     }
    //     cout << endl;
    // }
    // for (double prob : PROBS)
    //     cout << prob << " ";
    // cout << endl;

    //
    vector<vector<bool>> copyGRAPH = GRAPH;
    char buff[100];
    int cnt = 0;
    for (int r = 0; r < NODES; r++) {
        for (int c = 0; c < NODES; c++) {
            if (copyGRAPH[r][c]) {
                copyGRAPH[r][c] = false;
                copyGRAPH[c][r] = false;

                sprintf(buff, "%d%d", r, c);
                string line = buff;
                LINES[line] = cnt++;
            }
        }
    }

    //
    string INIT_PATH = string(cnt, '-');
    vector<int> reach = {0};
    clock_t start = clock();
    ReliabilityFromGraph(reach, NODES - 1, GRAPH, INIT_PATH);
    cout << "Total cost " << (float)(clock() - start) / CLOCKS_PER_SEC << " seconds\n"
         << endl;

    //
    cout << "SOLS_PATH: " << SOLS_PATH.size() << endl;
    cout << "State   Reliability" << endl;
    cout << "----------------------" << endl;
    for (int i = 0; i < SOLS_PATH.size(); i++) {
        cout << SOLS_PATH[i] << "     " << SOLS_RELI[i] << endl;
    }
}