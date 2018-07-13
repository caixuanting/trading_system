// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#include "utils/graph.h"

using std::vector;

namespace util {

Graph::Graph() {}

Graph::~Graph() {}

void Graph::InsertNode(const Node& node) { nodes_.insert(node); }

void Graph::InsertEdge(const Node& from, const Node& to) {
  edges_[from].insert(to);
}

Nodes Graph::GetEdges(const Node& from) { return edges_[from]; }

SerializedGraph Graph::SerializeGraph() const {
  SerializedGraph serialized_graph;

  Nodes roots = FindRoots();
  Edges edges = edges_;
  vector<Node> processing_queue;
  Nodes finished_nodes;

  // Iterate through each root.
  while (!roots.empty()) {
    processing_queue.push_back(*roots.begin());
    roots.erase(roots.begin());

    // Iterate through all node under the root.
    while (!processing_queue.empty()) {
      Node current_node = processing_queue.back();

      if (edges[current_node].empty()) {
        // If the node is done and not inserted.
        if (finished_nodes.count(current_node) == 0) {
          serialized_graph.push_back(current_node);
          finished_nodes.insert(current_node);
        }

        processing_queue.pop_back();
      } else {
        Nodes& children = edges[current_node];
        processing_queue.push_back(*children.begin());
        children.erase(children.begin());
      }
    }
  }

  return serialized_graph;
}

Nodes Graph::FindRoots() const {
  Nodes tmp_nodes = nodes_;

  for (const auto& edges : edges_) {
    // If there is an edge to the node, then it is not a root node.
    for (const auto& node : edges.second) {
      tmp_nodes.erase(node);
    }
  }

  return tmp_nodes;
}

}  // namespace util
