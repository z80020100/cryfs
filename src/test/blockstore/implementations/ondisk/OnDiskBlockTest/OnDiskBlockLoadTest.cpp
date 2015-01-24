#include <blockstore/implementations/ondisk/OnDiskBlock.h>
#include <blockstore/utils/Data.h>
#include <blockstore/utils/FileDoesntExistException.h>
#include <test/testutils/DataBlockFixture.h>
#include "gtest/gtest.h"

#include "test/testutils/TempFile.h"
#include "test/testutils/TempDir.h"
#include <fstream>

using ::testing::Test;
using ::testing::WithParamInterface;
using ::testing::Values;

using std::ofstream;
using std::unique_ptr;
using std::ios;

using namespace blockstore;
using namespace blockstore::ondisk;

namespace bf = boost::filesystem;

class OnDiskBlockLoadTest: public Test, public WithParamInterface<size_t> {
public:
  OnDiskBlockLoadTest():
    dir(),
    key(Key::FromString("1491BB4932A389EE14BC7090AC772972")),
    file(dir.path() / key.ToString()) {
  }
  TempDir dir;
  Key key;
  TempFile file;

  void SetFileSize(size_t size) {
    Data data(size);
    data.StoreToFile(file.path());
  }

  void StoreData(const DataBlockFixture &data) {
    //TODO Implement data.StoreToFile(filepath) instead
    Data dataobj(data.size());
    std::memcpy(dataobj.data(), data.data(), data.size());
    dataobj.StoreToFile(file.path());
  }

  unique_ptr<OnDiskBlock> LoadBlock() {
    return OnDiskBlock::LoadFromDisk(dir.path(), key);
  }

  void EXPECT_BLOCK_DATA_EQ(const DataBlockFixture &expected, const OnDiskBlock &actual) {
    EXPECT_EQ(expected.size(), actual.size());
    EXPECT_EQ(0, std::memcmp(expected.data(), actual.data(), expected.size()));
  }
};
INSTANTIATE_TEST_CASE_P(OnDiskBlockLoadTest, OnDiskBlockLoadTest, Values(0, 1, 5, 1024, 10*1024*1024));

TEST_P(OnDiskBlockLoadTest, FileSizeIsCorrect) {
  SetFileSize(GetParam());

  auto block = LoadBlock();

  EXPECT_EQ(GetParam(), block->size());
}

TEST_P(OnDiskBlockLoadTest, LoadedDataIsCorrect) {
  DataBlockFixture randomData(GetParam());
  StoreData(randomData);

  auto block = LoadBlock();

  EXPECT_BLOCK_DATA_EQ(randomData, *block);
}

TEST_F(OnDiskBlockLoadTest, LoadNotExistingBlock) {
  Key key2 = Key::FromString("272EE5517627CFA147A971A8E6E747E0");
  EXPECT_FALSE(
      (bool)OnDiskBlock::LoadFromDisk(dir.path(), key2)
  );
}
