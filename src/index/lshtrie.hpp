#pragma once

#include <vector>

#include "index.hpp"
#include "point.hpp"
#include "../hash/hashfamily.hpp"

template<ui32 D>
class LSHTrie {

public:
  LSHTrie(HashFamily<D>& hfamily) : hashes(hfamily) {
    root = new LSHNode();
  }
  
  ui32 size() { return index; }

  void add(std::vector<Point<D>>& points) {
    for (auto& point : points) {
      add(point);
    }
  }

  void add(const Point<D>& p) {
    // Start at root node
    LSHNode *node = root;

    // Iterate through depth of trie
    for (ui32 d = 0; d < depth(); ++d) {

      // Calculate hash of point at hash function d
      ui32 h_i = hashes[d](p);

      // If children does not exist, create them
      if (!node->children[h_i])
      {
        node->children[0] = new LSHNode();
        node->children[1] = new LSHNode();
      }

      // Get child based on hash
      node = node->children[h_i];
    }

    // Add index of current point to leaf node
    node->idx.push_back(index++);
  }

  ui32 depth() { return hashes.size(); } 

  // Returns bucket
  std::vector<ui32> query(const Point<D> &point) {
    // Start at root node
    LSHNode *node = root;

    // Iterate through depth of trie
    for (ui32 d = 0; d < depth(); ++d) {
      // Calculate hash of point at hash function d
      ui32 h_i = hashes[d](point);

      // If children does not exist, no points exists in subtree of node
      if (!node->children[h_i]) {
        return std::vector<ui32>();
      }

      // Get child based on hash
      node = node->children[h_i];
    }

    // Return indices of points in leaf node
    return node->idx;
  }

private:
  //! TODO: Use inheritence
  class LSHNode {
    //! TODO: Unique ptr
    public:
      LSHNode* children[2];

      // Leaf nodes
      std::vector<ui32> idx;

      LSHNode() {
        children[0] = nullptr;
        children[1] = nullptr;
      }

      bool isLeaf() { return idx.size(); }
  };
  
  // Stores root node of trie
  LSHNode *root;
  HashFamily<D> hashes;
  ui32 index = 0;
};

// template<ui32 D>
// class LSHForest {
//   // The number of inserted nodes
//   ui32 index_count = 0;
// public:
//   std::vector<LSHTrie*> tries;
// };
