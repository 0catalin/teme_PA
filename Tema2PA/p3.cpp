#include <fstream>
#include <iostream>
#include <list>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;

#define INFINITY 999999999

// a log structure consisting of the initial
// positions the moves, and the index
struct Log {
  int index;
  int x_start, y_start, x_end, y_end;
  vector<char> moves;

  // function checking if a log is vertical
  bool isVerticalLog() { return x_start == x_end; }
};

// a graphNode structure
struct GraphNode {
  int logIndex, logSegment, timestamp;
  int x, y;

  bool operator==(const GraphNode &other) const {
    return logIndex == other.logIndex && logSegment == other.logSegment &&
           timestamp == other.timestamp && x == other.x && y == other.y;
  }

  bool operator!=(const GraphNode &other) const {
    // returns the negation of the == operator
    return !(*this == other);
  }
};

// hash for a graph node efficiently chosen
struct GraphNodeHash {
  size_t operator()(const GraphNode &k) const {
    size_t h1 = hash<int>()(k.logIndex);
    size_t h2 = hash<int>()(k.logSegment);
    size_t h3 = hash<int>()(k.timestamp);
    size_t h4 = hash<int>()(k.x);
    size_t h5 = hash<int>()(k.y);

    size_t seed = h1;
    seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= h4 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= h5 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
  }
};

// hash for a pair efficiently chosen
struct pair_hash {
  size_t operator()(const std::pair<int, int> &p) const {
    size_t h1 = std::hash<int>()(p.first);
    size_t h2 = std::hash<int>()(p.second);
    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
  }
};

// comparator used for the pq in Dijkstra, we only compare
// based on the cost, which is the first element,
// otherwise we choose the node by timestamp
struct GraphNodeComparator {
  bool operator()(const pair<int, GraphNode> &p1,
                  const pair<int, GraphNode> &p2) const {
    if (p1.first != p2.first) {
      return p1.first > p2.first;
    }
    return p1.second.timestamp > p2.second.timestamp;
  }
};

// returns a string computed via the parent map
// and the number of moves which were made to get to Marian
pair<string, int>
getString(GraphNode node,
          const unordered_map<GraphNode, GraphNode, GraphNodeHash> &parents,
          const unordered_map<GraphNode, string, GraphNodeHash> &best_move) {
  vector<string> moves;

  // we go from parent to parent
  // and push the string into our list
  // until the node that we started from
  while (parents.at(node) != node) {
    moves.push_back(best_move.at(node));
    node = parents.at(node);
  }

  int resInt = moves.size();
  // compute the returned string in reverse
  string res = "";
  for (int i = moves.size() - 1; i >= 0; i--) {
    res += moves[i];
  }

  return {res, resInt};
}

int main() {
  int totalTime, log_number;
  int marianX, marianY;
  int E1, E2, E3;

  // reading files and initializing structures
  ifstream fin("p3.in");
  ofstream fout("p3.out");

  fin >> totalTime >> log_number;
  fin >> marianX >> marianY;
  fin >> E1 >> E2 >> E3;

  vector<Log> logs(log_number);

  for (int i = 0; i < log_number; i++) {
    fin >> logs[i].x_start >> logs[i].y_start >> logs[i].x_end >> logs[i].y_end;
    logs[i].index = i + 1;
  }

  for (int i = 0; i < log_number; i++) {
    string s;
    fin >> s;
    logs[i].moves = vector<char>(s.begin(), s.end());
  }

  // parents map for O(1)
  unordered_map<GraphNode, GraphNode, GraphNodeHash> parents;
  // map for costs to reach the node
  unordered_map<GraphNode, int, GraphNodeHash> costs;

  // map with the best cost move to reach the node
  unordered_map<GraphNode, string, GraphNodeHash> best_move;

  // set with the visited nodes
  unordered_set<GraphNode, GraphNodeHash> visited;

  // map of maps where we can access a list of nodes by the given time and
  // coordinates
  unordered_map<int, unordered_map<pair<int, int>, list<GraphNode>, pair_hash>>
      timeSpaceToLogSegm;

  // priority queue for Dijkstra, containing Node and cost to reach it
  priority_queue<pair<int, GraphNode>, vector<pair<int, GraphNode>>,
                 GraphNodeComparator>
      pq;

  int curr_x_start, curr_y_start;
  char c;
  GraphNode node;

  // we initialize all the Nodes, based on time, log_index and log segment
  // to create the timeSpaceToLogSegm map for knowing where Robin Hood
  // can jump, efficiently
  for (Log log : logs) {
    curr_x_start = log.x_start;
    curr_y_start = log.y_start;

    for (int i = 0; i <= totalTime; i++) {
      // the log is vertical and not upside-down
      if (log.y_end > log.y_start && log.isVerticalLog()) {
        for (int j = 0; j <= log.y_end - log.y_start; j++) {
          node = {log.index, j, i, curr_x_start, curr_y_start + j};
          timeSpaceToLogSegm[i][{curr_x_start, curr_y_start + j}].push_back(
              node);
        }
        // the log is vertical and upside-down
      } else if (log.y_start > log.y_end && log.isVerticalLog()) {
        for (int j = log.y_end - log.y_start; j <= 0; j++) {
          node = {log.index, -j, i, curr_x_start, curr_y_start + j};
          timeSpaceToLogSegm[i][{curr_x_start, curr_y_start + j}].push_back(
              node);
        }
        // the log is not vertical and not upside-down
      } else if (log.x_end > log.x_start && !log.isVerticalLog()) {
        for (int j = 0; j <= log.x_end - log.x_start; j++) {
          node = {log.index, j, i, curr_x_start + j, curr_y_start};
          timeSpaceToLogSegm[i][{curr_x_start + j, curr_y_start}].push_back(
              node);
        }
        // the log is not vertical and upside-down
      } else if (log.x_start > log.x_end && !log.isVerticalLog()) {
        for (int j = log.x_end - log.x_start; j <= 0; j++) {
          node = {log.index, -j, i, curr_x_start + j, curr_y_start};
          timeSpaceToLogSegm[i][{curr_x_start + j, curr_y_start}].push_back(
              node);
        }
      }

      // condition to not access an invalid index
      if (i == totalTime)
        break;

      // we change the position of the log over time depending on the character
      c = log.moves[i];
      if (c == 'N') {
        curr_y_start += 1;
      } else if (c == 'S') {
        curr_y_start -= 1;
      } else if (c == 'E') {
        curr_x_start += 1;
      } else if (c == 'V') {
        curr_x_start -= 1;
      }
    }
  }

  list<pair<GraphNode, string>> secondList;
  list<GraphNode> thirdList;

  // Dijkstra starts here

  int x_node, y_node, some_y_end, some_x_end, some_x_start, some_y_start;
  GraphNode someNode, someOtherNode;

  // initialize source and add to data structures
  GraphNode source = {logs[0].index, 0, 0, logs[0].x_start, logs[0].y_start};
  pq.push({0, source});
  costs[source] = 0;
  best_move[source] = "";
  parents[source] = source;

  string finalRes;

  while (!pq.empty()) {
    auto [cost, node] = pq.top();
    pq.pop();

    // if it was visited it was at its minimum
    // we do not remove elements from the pq
    // so the first will always be the minimum
    // we ignore if we expanded it before
    if (visited.count(node))
      continue;
    visited.insert(node);

    // if the node's coordinates are marian's we stop the algorithm
    // and print the result
    if (node.x == marianX && node.y == marianY) {
      auto [finalRes, moveCount] = getString(node, parents, best_move);
      fout << cost << endl << moveCount << endl << finalRes;
      break;
    }

    // we search for the current state node's neighbours
    // first case -> only the log moves implicitely with us
    char move = logs[node.logIndex - 1].moves[node.timestamp];
    x_node = node.x;
    y_node = node.y;

    // we change the coordinate depending on the
    // move, which we can easily access in O(1)
    if (move == 'N')
      y_node++;
    else if (move == 'S')
      y_node--;
    else if (move == 'E')
      x_node++;
    else
      x_node--;

    // compute the next state
    someNode = {node.logIndex, node.logSegment, node.timestamp + 1, x_node,
                y_node};

    // if we can improve the cost we do it and update all the data structures
    // NOTE! costs.count(someNode) == 0 is equivalent to adding all the
    // initial nodes to the pq with infinity cost
    if (visited.count(someNode) == 0 &&
        (costs.count(someNode) == 0 || costs[someNode] > costs[node] + E1)) {
      costs[someNode] = costs[node] + E1;
      parents[someNode] = node;
      best_move[someNode] = "H\n";
      pq.push({costs[someNode], someNode});
    }

    // second case -> move left / right / up / down +
    // the log movement from before, which's index already
    // is computed, we must not compute again!

    // we add all the moves, depending on the
    // segment, if the segment is terminal we only have one move
    // otherwise we have 2, and we also consider the
    // position of the log and verticality
    // we compute all the possible next nodes
    // and add them to the list

    if (logs[node.logIndex - 1].isVerticalLog()) {
      some_y_end = logs[node.logIndex - 1].y_end;
      some_y_start = logs[node.logIndex - 1].y_start;

      if (some_y_end > some_y_start) {
        if (some_y_end - some_y_start == node.logSegment) {
          someNode = {node.logIndex, node.logSegment - 1, node.timestamp + 1,
                      x_node, y_node - 1};
          secondList.push_back(make_pair(someNode, "S\n"));
        } else if (0 == node.logSegment) {
          someNode = {node.logIndex, node.logSegment + 1, node.timestamp + 1,
                      x_node, y_node + 1};
          secondList.push_back(make_pair(someNode, "N\n"));
        } else {
          // middle
          someNode = {node.logIndex, node.logSegment - 1, node.timestamp + 1,
                      x_node, y_node - 1};
          secondList.push_back(make_pair(someNode, "S\n"));
          someNode = {node.logIndex, node.logSegment + 1, node.timestamp + 1,
                      x_node, y_node + 1};
          secondList.push_back(make_pair(someNode, "N\n"));
        }
      } else {
        if (some_y_start - some_y_end == node.logSegment) {
          someNode = {node.logIndex, node.logSegment - 1, node.timestamp + 1,
                      x_node, y_node + 1};
          secondList.push_back(make_pair(someNode, "N\n"));
        } else if (0 == node.logSegment) {
          someNode = {node.logIndex, node.logSegment + 1, node.timestamp + 1,
                      x_node, y_node - 1};
          secondList.push_back(make_pair(someNode, "S\n"));
        } else {
          // middle
          someNode = {node.logIndex, node.logSegment - 1, node.timestamp + 1,
                      x_node, y_node + 1};
          secondList.push_back(make_pair(someNode, "N\n"));
          someNode = {node.logIndex, node.logSegment + 1, node.timestamp + 1,
                      x_node, y_node - 1};
          secondList.push_back(make_pair(someNode, "S\n"));
        }
      }
    } else {
      some_x_end = logs[node.logIndex - 1].x_end;
      some_x_start = logs[node.logIndex - 1].x_start;

      if (some_x_end > some_x_start) {
        if (some_x_end - some_x_start == node.logSegment) {
          someNode = {node.logIndex, node.logSegment - 1, node.timestamp + 1,
                      x_node - 1, y_node};
          secondList.push_back(make_pair(someNode, "V\n"));
        } else if (0 == node.logSegment) {
          someNode = {node.logIndex, node.logSegment + 1, node.timestamp + 1,
                      x_node + 1, y_node};
          secondList.push_back(make_pair(someNode, "E\n"));
        } else {
          // middle
          someNode = {node.logIndex, node.logSegment - 1, node.timestamp + 1,
                      x_node - 1, y_node};
          secondList.push_back(make_pair(someNode, "V\n"));
          someNode = {node.logIndex, node.logSegment + 1, node.timestamp + 1,
                      x_node + 1, y_node};
          secondList.push_back(make_pair(someNode, "E\n"));
        }
      } else {
        if (some_x_start - some_x_end == node.logSegment) {
          someNode = {node.logIndex, node.logSegment - 1, node.timestamp + 1,
                      x_node + 1, y_node};
          secondList.push_back(make_pair(someNode, "E\n"));
        } else if (0 == node.logSegment) {
          someNode = {node.logIndex, node.logSegment + 1, node.timestamp + 1,
                      x_node - 1, y_node};
          secondList.push_back(make_pair(someNode, "V\n"));
        } else {
          // middle
          someNode = {node.logIndex, node.logSegment - 1, node.timestamp + 1,
                      x_node + 1, y_node};
          secondList.push_back(make_pair(someNode, "E\n"));
          someNode = {node.logIndex, node.logSegment + 1, node.timestamp + 1,
                      x_node - 1, y_node};
          secondList.push_back(make_pair(someNode, "V\n"));
        }
      }
    }

    // we do the same thing as before with the elements from the second list
    for (auto &pair : secondList) {
      GraphNode &someNode = pair.first;
      if (visited.count(someNode) == 0 &&
          (costs.count(someNode) == 0 || costs[someNode] > costs[node] + E2)) {
        best_move[someNode] = pair.second;
        costs[someNode] = costs[node] + E2;
        parents[someNode] = node;
        pq.push({costs[someNode], someNode});
      }
    }
    secondList.clear();

    // third case -> jump from a log to another
    thirdList = timeSpaceToLogSegm[node.timestamp][make_pair(node.x, node.y)];

    // we iterate through the accessible elements
    for (GraphNode someNode : thirdList) {
      // only if the node is different than our node
      // since we don't jump from a state to the same state
      // that is done at E1 cost
      if (someNode != node) {
        char move = logs[someNode.logIndex - 1].moves[someNode.timestamp];
        x_node = someNode.x;
        y_node = someNode.y;

        // compute where the log that Robin will jump
        // on will reach
        if (move == 'N')
          y_node++;
        else if (move == 'S')
          y_node--;
        else if (move == 'E')
          x_node++;
        else
          x_node--;

        // the someOtherNode consists of the node
        // where Robin will reach after jumping and
        // waiting a second, where someNode is
        // the node where the coordinates are the same
        // but the log is different
        someOtherNode = {someNode.logIndex, someNode.logSegment,
                         someNode.timestamp + 1, x_node, y_node};

        // same step of Dijkstra that we did before
        if (visited.count(someOtherNode) == 0 &&
            (costs.count(someOtherNode) == 0 ||
             costs[someOtherNode] > costs[node] + E3)) {
          costs[someOtherNode] = costs[node] + E3;
          parents[someOtherNode] = node;
          pq.push({costs[someOtherNode], someOtherNode});
          best_move[someOtherNode] =
              "J " + to_string(someOtherNode.logIndex) + "\n";
        }
      }
    }
  }

  // close the files
  fin.close();
  fout.close();
  return 0;
}
