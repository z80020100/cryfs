#include "CryDir.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include "messmer/fspp/fuse/FuseErrnoException.h"
#include "CryDevice.h"
#include "CryFile.h"
#include "CryOpenFile.h"

//TODO Get rid of this in favor of exception hierarchy
using fspp::fuse::CHECK_RETVAL;
using fspp::fuse::FuseErrnoException;

namespace bf = boost::filesystem;

using std::unique_ptr;
using std::make_unique;
using std::string;
using std::vector;

using blockstore::Key;

namespace cryfs {

CryDir::CryDir(CryDevice *device, unique_ptr<DirBlob> parent, const Key &key)
: CryNode(device, std::move(parent), key) {
}

CryDir::~CryDir() {
}

unique_ptr<fspp::OpenFile> CryDir::createAndOpenFile(const string &name, mode_t mode) {
  //TODO Create file owned by calling user (fuse user is given in fuse context)
  auto blob = LoadBlob();
  auto child = device()->CreateBlob();
  Key childkey = child->key();
  blob->AddChildFile(name, childkey, mode);
  //TODO Do we need a return value in createDir for fspp? If not, change fspp Dir interface!
  auto childblob = FileBlob::InitializeEmptyFile(std::move(child));
  return make_unique<CryOpenFile>(std::move(childblob));
}

void CryDir::createDir(const string &name, mode_t mode) {
  //TODO Create dir owned by calling user (fuse user is given in fuse context)
  auto blob = LoadBlob();
  auto child = device()->CreateBlob();
  Key childkey = child->key();
  blob->AddChildDir(name, childkey, mode);
  DirBlob::InitializeEmptyDir(std::move(child), device());
}

unique_ptr<DirBlob> CryDir::LoadBlob() const {
  //TODO Without const_cast?
  return make_unique<DirBlob>(CryNode::LoadBlob(), const_cast<CryDevice*>(device()));
}

unique_ptr<vector<fspp::Dir::Entry>> CryDir::children() const {
  auto children = make_unique<vector<fspp::Dir::Entry>>();
  children->push_back(fspp::Dir::Entry(fspp::Dir::EntryType::DIR, "."));
  children->push_back(fspp::Dir::Entry(fspp::Dir::EntryType::DIR, ".."));
  LoadBlob()->AppendChildrenTo(children.get());
  return children;
}

fspp::Dir::EntryType CryDir::getType() const {
  return fspp::Dir::EntryType::DIR;
}

}