// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#ifndef UTILS_GRAPH_H_
#define UTILS_GRAPH_H_

#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace util {

typedef std::string Node;
typedef std::set<Node> Nodes;
typedef std::unordered_map<Node, Nodes> Edges;
typedef std::vector<Node> SerializedGraph;

class Graph {
 public:
  Graph();
  virtual ~Graph();

  void InsertNode(const Node& node);
  void InsertEdge(const Node& from, const Node& to);

  Nodes GetEdges(const Node& from);
  // Topological sorting.
  SerializedGraph SerializeGraph() const;

 private:
  Nodes FindRoots() const;

  Nodes nodes_;
  Edges edges_;
};

}  // namespace util

#endif  // UTILS_GRAPH_H_
