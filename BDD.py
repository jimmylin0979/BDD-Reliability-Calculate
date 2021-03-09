from copy import deepcopy

NODES = 0
GRAPH = []
LINES = {}
PROBS = []
PATHS = []


def ReliabilityFromGraph(reach, target, Graph, curPath):
    if reach[-1] == target:
        # if nodes already reach the 'to' node the last time
        # then dfs done
        PATHS.append(curPath)
        return None

    # dfs main body
    for r in reach:
        for c in range(NODES):
            if Graph[r][c] == '1' and (c not in reach):
                state_RC = Graph[r][c]
                state_CR = Graph[r][c]
                Graph[r][c] = '0'
                Graph[c][r] = '0'

                ReliabilityFromGraph(reach, target, Graph,
                                     curPath + [f'{r}-{c}=0'])
                ReliabilityFromGraph(
                    reach + [c], target, Graph, curPath + [f'{r}-{c}=1'])

                Graph[r][c] = state_RC
                Graph[c][r] = state_CR

    return None


def ReadFromFile(filepath):
    contents = []
    with open(filepath, 'r') as fileObj:
        contents = fileObj.readlines()

    global NODES
    global GRAPH
    global PROBS
    NODES = int(contents[0].strip('\n'))
    for line in range(1, NODES+1):
        rowPath = ((contents[line]).strip('\n')).split()
        GRAPH.append(rowPath)

    PROBS = (contents[-1].strip('\n')).split()
    PROBS = list(map(float, PROBS))
    return None


if __name__ == '__main__':
    #
    ReadFromFile(r"./graph.txt")
    print(NODES)
    print(GRAPH)
    print(PROBS)

    #
    lines = []
    Mapping_GRAPH = deepcopy(GRAPH)
    for r in range(NODES):
        for c in range(NODES):
            if Mapping_GRAPH[r][c] == '1':
                Mapping_GRAPH[r][c] = '0'
                Mapping_GRAPH[c][r] = '0'
                lines.append(f'{r}{c}')
    print(lines)
    LINES = {lines[i]: i for i in range(len(lines))}

    #
    ReliabilityFromGraph([0], 3, GRAPH, [])
    print(f"PATHS: {len(PATHS)} paths")
    for path in PATHS:
        print(path)
