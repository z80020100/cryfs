#pragma once
#ifndef CRYFS_LIB_CRYNODE_H_
#define CRYFS_LIB_CRYNODE_H_

#include <messmer/fspp/fs_interface/Node.h>
#include "messmer/cpp-utils/macros.h"
#include <messmer/fspp/fs_interface/Dir.h>

#include "CryDevice.h"

namespace cryfs {

class CryNode: public virtual fspp::Node {
public:
  CryNode(CryDevice *device, std::unique_ptr<DirBlob> parent, const blockstore::Key &key);
  void access(int mask) const override;
  void stat(struct ::stat *result) const override;
  void chmod(mode_t mode) override;
  void chown(uid_t uid, gid_t gid) override;
  void rename(const boost::filesystem::path &to) override;
  void utimens(const timespec times[2]) override;
  void remove() override;

protected:
  CryNode();
  virtual ~CryNode();

  CryDevice *device();
  const CryDevice *device() const;
  std::unique_ptr<blobstore::Blob> LoadBlob() const;

  virtual fspp::Dir::EntryType getType() const = 0;

private:
  CryDevice *_device;
  std::unique_ptr<DirBlob> _parent;
  blockstore::Key _key;

  DISALLOW_COPY_AND_ASSIGN(CryNode);
};

}

#endif
