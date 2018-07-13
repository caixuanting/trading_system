// Copyright 2016, Xuanting Cai
// Email: caixuanting@gmail.com

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE

#include <boost/test/unit_test.hpp>

#include "utils/graph.h"

namespace util {

//        root_1    root_2 -> middle_1
//        |    |    |    |    |
//        v    v    v    v    v
//   leaf_1    leaf_2    leaf_3

struct GraphFixture {
  GraphFixture() {
    graph.InsertNode("root_1");
    graph.InsertNode("root_2");
    graph.InsertNode("middle_1");
    graph.InsertNode("leaf_1");
    graph.InsertNode("leaf_2");
    graph.InsertNode("leaf_3");

    graph.InsertEdge("root_1", "leaf_1");
    graph.InsertEdge("root_1", "leaf_2");
    graph.InsertEdge("root_2", "leaf_2");
    graph.InsertEdge("root_2", "leaf_3");
    graph.InsertEdge("root_2", "middle_1");
    graph.InsertEdge("middle_1", "leaf_3");
  }

  virtual ~GraphFixture() {}

  Graph graph;
};

BOOST_FIXTURE_TEST_SUITE(GraphTests, GraphFixture)

BOOST_AUTO_TEST_CASE(SerializeGraphTest) {
  SerializedGraph serialized_graph = graph.SerializeGraph();

  SerializedGraph expected = {"leaf_1", "leaf_2",   "root_1",
                              "leaf_3", "middle_1", "root_2"};

  BOOST_CHECK_EQUAL_COLLECTIONS(serialized_graph.begin(),
                                serialized_graph.end(), expected.begin(),
                                expected.end());
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace util
