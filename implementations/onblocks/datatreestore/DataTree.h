#pragma once
#ifndef BLOBSTORE_IMPLEMENTATIONS_ONBLOCKS_DATATREE_H_
#define BLOBSTORE_IMPLEMENTATIONS_ONBLOCKS_DATATREE_H_

#include <memory>
#include <messmer/cpp-utils/macros.h>
#include <messmer/cpp-utils/optional_ownership_ptr.h>

namespace blockstore {
class Key;
}
namespace blobstore {
namespace onblocks {
namespace datanodestore {
class DataNodeStore;
class DataInnerNode;
class DataLeafNode;
class DataNode;
}
namespace datatreestore {

class DataTree {
public:
  DataTree(datanodestore::DataNodeStore *nodeStore, std::unique_ptr<datanodestore::DataNode> rootNode);
  virtual ~DataTree();

  std::unique_ptr<datanodestore::DataLeafNode> addDataLeaf();
  void removeLastDataLeaf();

  const blockstore::Key &key() const;

  void flush() const;

  void traverseLeaves(uint32_t beginIndex, uint32_t endIndex, std::function<void (datanodestore::DataLeafNode*, uint32_t)> func);
  uint64_t numStoredBytes() const;

private:
  datanodestore::DataNodeStore *_nodeStore;
  std::unique_ptr<datanodestore::DataNode> _rootNode;

  std::unique_ptr<datanodestore::DataNode> releaseRootNode();
  friend class DataTreeStore;

  std::unique_ptr<datanodestore::DataLeafNode> addDataLeafAt(datanodestore::DataInnerNode *insertPos);
  cpputils::optional_ownership_ptr<datanodestore::DataNode> createChainOfInnerNodes(unsigned int num, datanodestore::DataLeafNode *leaf);
  std::unique_ptr<datanodestore::DataLeafNode> addDataLeafToFullTree();

  void deleteLastChildSubtree(datanodestore::DataInnerNode *node);
  void ifRootHasOnlyOneChildReplaceRootWithItsChild();

  void traverseLeaves(datanodestore::DataNode *root, uint32_t leafOffset, uint32_t beginIndex, uint32_t endIndex, std::function<void (datanodestore::DataLeafNode*, uint32_t)> func);
  uint32_t leavesPerFullChild(const datanodestore::DataInnerNode &root) const;
  uint64_t numStoredBytes(const datanodestore::DataNode &root) const;

  DISALLOW_COPY_AND_ASSIGN(DataTree);
};

}
}
}

#endif