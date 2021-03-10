from copy import deepcopy
import sys

NODES = 0
GRAPH = []
LINES = {}
PROBS = []
SOLS_PATH = []
SOLS_RELI = []


def ReliabilityFromGraph(reach, target, Graph, curPath, prob=1):
    if reach[-1] == target:
        # if nodes already reach the 'to' node the last time
        # then dfs done

        # whether there are '-' no cares in curPath
        dontCareCnt = 0
        for c in curPath:
            if c == '-':
                dontCareCnt += 1

        iterCnt = 2 ** dontCareCnt
        for iter in range(iterCnt):
            # iterable possible path state, in binary state
            iterPoss = bin(iter)[2:]
            iterPoss = '0' * (dontCareCnt - len(iterPoss)) + iterPoss

            cnt = 0
            nowPath = curPath[:]
            nowProb = prob
            for i in range(len(nowPath)):
                if nowPath[i] == '-':
                    state = int(iterPoss[cnt])
                    nowPath[i] = state
                    nowProb = nowProb * \
                        (PROBS[i] if state == 1 else (1 - PROBS[i]))
                    cnt += 1

                if cnt == iterCnt:
                    break

            if nowPath not in SOLS_PATH:
                SOLS_PATH.append(nowPath)
                SOLS_RELI.append(nowProb)
        return None

    # dfs main body
    for r in reach:
        for c in range(NODES):
            if Graph[r][c] == '1':

                # the path choose currently in Graph
                choosePath = f'{r if r < c else c}{r if r > c else c}'
                choosePath = LINES[choosePath]

                state_RC = Graph[r][c]
                state_CR = Graph[c][r]
                Graph[r][c] = '0'
                Graph[c][r] = '0'

                curPath[choosePath] = 0
                ReliabilityFromGraph(reach, target, Graph,
                                     curPath, prob * (1 - PROBS[choosePath]))
                curPath[choosePath] = 1
                ReliabilityFromGraph(
                    reach + [c], target, Graph, curPath, prob * PROBS[choosePath])

                curPath[choosePath] = '-'

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

    # read filepath as argv
    try:
        ReadFromFile(sys.argv[1])
    except Exception as e:
        print(e)
        sys.exit(1)

    # print out HyperParameter
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
    INIT_PATH = ['-' for _ in range(len(LINES))]
    ReliabilityFromGraph([0], NODES-1, GRAPH, INIT_PATH)

    # OUTPUT SOLUTIONS
    print(f"SOLS_PATH: {len(SOLS_PATH)} SOLS_PATH")

    # sort
    Z = zip(SOLS_PATH, SOLS_RELI)
    Z = sorted(Z)
    SOLS_PATH, SOLS_RELI = zip(*Z)

    for i in range(len(SOLS_PATH)):
        print(f'{SOLS_PATH[i]}, {SOLS_RELI[i]}')
