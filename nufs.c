// based on cs3650 starter code

#include <assert.h>
#include <stdlib.h>
#include <bsd/string.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FUSE_USE_VERSION 26
#include <fuse.h>

<<<<<<< HEAD
#include "helpers/bitmap.h"
#include "helpers/blocks.h"
#include "helpers/slist.h"
=======
#include "bitmap.h"
#include "blocks.h"
#include "slist.h"
>>>>>>> 8ddfb2de8e5e1ac3bb7249cf5e9d96c3b9f2d116

#define MAX_FILENAME 256
#define BLOCK_SIZE 4096
#define MAX_BLOCKS 256

typedef struct {
    char name[MAX_FILENAME];
    int inum;
} dir_entry;

typedef struct {
    int refs;
    mode_t mode;
    int size;
    int data[MAX_BLOCKS];
} inode;

typedef struct {
    char *data;
} block;

typedef struct {
    int size;
    int free[MAX_BLOCKS];
} free_block_list;

typedef struct {
    dir_entry entries[MAX_BLOCKS];
    int size;
} root_directory;

typedef struct {
    inode inodes[MAX_BLOCKS];
    block blocks[MAX_BLOCKS];
    free_block_list free_blocks;
    root_directory root_dir;
} filesystem;

filesystem fs;

void init_filesystem() {
    memset(&fs, 0, sizeof(filesystem));

    // Initialize the free block list using the blocks helper
    blocks_init("path_to_disk_image");  // Replace with the actual path
}

int find_free_inode() {
    for (int i = 1; i < MAX_BLOCKS; ++i) {
        if (fs.inodes[i].refs == 0) {
            return i;
        }
    }
    return -1; // No free inode
}

int find_free_block() {
    int block_num = alloc_block();
    if (block_num == -1) {
        return -1; // No free block
    }
    return block_num;
}

void release_block(int block_num) {
    free_block(block_num);
}

int create_file(const char *path, mode_t mode) {
    int inum = find_free_inode();
    if (inum == -1) {
        return -1; // No free inode
    }

    int block_num = find_free_block();
    if (block_num == -1) {
        return -1; // No free block
    }

    strncpy(fs.root_dir.entries[fs.root_dir.size].name, path, MAX_FILENAME - 1);
    fs.root_dir.entries[fs.root_dir.size].inum = inum;
    fs.root_dir.size++;

    fs.inodes[inum].refs = 1;
    fs.inodes[inum].mode = mode;
    fs.inodes[inum].size = 0;
    fs.inodes[inum].data[0] = block_num;

    return 0; // Success
}

<<<<<<< HEAD
// Helper function to find the inode number by path
int find_inum_by_path(const char *path) {
    // Iterate through root directory entries to find the matching path
    for (int i = 0; i < fs.root_dir.size; ++i) {
        if (strcmp(fs.root_dir.entries[i].name, path) == 0) {
            // Found a matching entry, return the corresponding inode number
            return fs.root_dir.entries[i].inum;
        }
    }

    // If the path is not found, return an error value (you may choose -1 or any other value)
    return -1;
=======
int nufs_access(const char *path, int mask) {
    int rv = 0;

    if (strcmp(path, "/") != 0 && strcmp(path, "/hello.txt") != 0) {
        rv = -ENOENT;
    }

    printf("access(%s, %04o) -> %d\n", path, mask, rv);
    return rv;
>>>>>>> 8ddfb2de8e5e1ac3bb7249cf5e9d96c3b9f2d116
}

// implementation for: man 2 access
// Checks if a file exists.
int nufs_access(const char *path, int mask) {
  int rv = 0;

  // Only the root directory and our simulated file are accessible for now...
  if (strcmp(path, "/") == 0 || strcmp(path, "/hello.txt") == 0) {
    rv = 0;
  } else { // ...others do not exist
    rv = -ENOENT;
  }

  printf("access(%s, %04o) -> %d\n", path, mask, rv);
  return rv;
}

// Gets an object's attributes (type, permissions, size, etc).
// Implementation for: man 2 stat
// This is a crucial function.
int nufs_getattr(const char *path, struct stat *st) {
  int rv = 0;

  // Return some metadata for the root directory...
  if (strcmp(path, "/") == 0) {
    st->st_mode = 040755; // directory
    st->st_size = 0;
    st->st_uid = getuid();
  }
  // ...and the simulated file...
  else if (strcmp(path, "/hello.txt") == 0) {
    st->st_mode = 0100644; // regular file
    st->st_size = 6;
    st->st_uid = getuid();
  } else { // ...other files do not exist on this filesystem
    rv = -ENOENT;
  }
  printf("getattr(%s) -> (%d) {mode: %04o, size: %ld}\n", path, rv, st->st_mode,
         st->st_size);
  return rv;
}

// implementation for: man 2 readdir
// lists the contents of a directory
int nufs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                 off_t offset, struct fuse_file_info *fi) {
  struct stat st;
  int rv;

  rv = nufs_getattr("/", &st);
  assert(rv == 0);

  filler(buf, ".", &st, 0);

  rv = nufs_getattr("/hello.txt", &st);
  assert(rv == 0);
  filler(buf, "hello.txt", &st, 0);

  printf("readdir(%s) -> %d\n", path, rv);
  return 0;
}

// mknod makes a filesystem object like a file or directory
// called for: man 2 open, man 2 link
// Note, for this assignment, you can alternatively implement the create
// function.
int nufs_mknod(const char *path, mode_t mode, dev_t rdev) {
  int rv = -1;
  printf("mknod(%s, %04o) -> %d\n", path, mode, rv);
  return rv;
}

// most of the following callbacks implement
// another system call; see section 2 of the manual
int nufs_mkdir(const char *path, mode_t mode) {
  int rv = nufs_mknod(path, mode | 040000, 0);
  printf("mkdir(%s) -> %d\n", path, rv);
  return rv;
}

int nufs_unlink(const char *path) {
  int rv = -1;
  printf("unlink(%s) -> %d\n", path, rv);
  return rv;
}

int nufs_link(const char *from, const char *to) {
  int rv = -1;
  printf("link(%s => %s) -> %d\n", from, to, rv);
  return rv;
}

int nufs_rmdir(const char *path) {
  int rv = -1;
  printf("rmdir(%s) -> %d\n", path, rv);
  return rv;
}

// implements: man 2 rename
// called to move a file within the same filesystem
int nufs_rename(const char *from, const char *to) {
  int rv = -1;
  printf("rename(%s => %s) -> %d\n", from, to, rv);
  return rv;
}

int nufs_chmod(const char *path, mode_t mode) {
  int rv = -1;
  printf("chmod(%s, %04o) -> %d\n", path, mode, rv);
  return rv;
}

int nufs_truncate(const char *path, off_t size) {
  int rv = -1;
  printf("truncate(%s, %ld bytes) -> %d\n", path, size, rv);
  return rv;
}

// This is called on open, but doesn't need to do much
// since FUSE doesn't assume you maintain state for
// open files.
// You can just check whether the file is accessible.
int nufs_open(const char *path, struct fuse_file_info *fi) {
  int rv = 0;
  printf("open(%s) -> %d\n", path, rv);
  return rv;
}

// Aly read data
int nufs_read(const char *path, char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi) {
  int inum = find_inum_by_path(path);
  if (inum == -1) {
    return -ENOENT; // File not found
  }

  inode *node = &fs.inodes[inum];
  if (offset >= node->size) {
    return 0; // End of file
  }

  size_t remaining = node->size - offset;
  size = (size < remaining) ? size : remaining;

  int block_num = node->data[offset / BLOCK_SIZE];
  int block_offset = offset % BLOCK_SIZE;

  block *data_block = &fs.blocks[block_num];
  memcpy(buf, data_block->data + block_offset, size);

  return size;
}

// Actually write data
int nufs_write(const char *path, const char *buf, size_t size, off_t offset,
               struct fuse_file_info *fi) {
  int inum = find_inum_by_path(path);
  if (inum == -1) {
    return -ENOENT; // File not found
  }

  inode *node = &fs.inodes[inum];
  int block_num = node->data[offset / BLOCK_SIZE];
  int block_offset = offset % BLOCK_SIZE;

  // Check if there's enough space in the current block
  if (size > BLOCK_SIZE - block_offset) {
    // If not, you might need to allocate more blocks or handle this situation accordingly
    return -ENOSPC; // Placeholder return value for no space
  }

  block *data_block = &fs.blocks[block_num];
  memcpy(data_block->data + block_offset, buf, size);

  // Update file size if needed
  if (offset + size > node->size) {
    node->size = offset + size;
  }

  return size;

}
 
// Update the timestamps on a file or directory.
int nufs_utimens(const char *path, const struct timespec ts[2]) {
  int rv = -1;
  printf("utimens(%s, [%ld, %ld; %ld %ld]) -> %d\n", path, ts[0].tv_sec,
         ts[0].tv_nsec, ts[1].tv_sec, ts[1].tv_nsec, rv);
  return rv;
}

// Extended operations
int nufs_ioctl(const char *path, int cmd, void *arg, struct fuse_file_info *fi,
               unsigned int flags, void *data) {
  int rv = -1;
  printf("ioctl(%s, %d, ...) -> %d\n", path, cmd, rv);
  return rv;
}

void nufs_init_ops(struct fuse_operations *ops) {
  memset(ops, 0, sizeof(struct fuse_operations));
  ops->access = nufs_access;
  ops->getattr = nufs_getattr;
  ops->readdir = nufs_readdir;
  ops->mknod = nufs_mknod;
  // ops->create   = nufs_create; // alternative to mknod
  ops->mkdir = nufs_mkdir;
  ops->link = nufs_link;
  ops->unlink = nufs_unlink;
  ops->rmdir = nufs_rmdir;
  ops->rename = nufs_rename;
  ops->chmod = nufs_chmod;
  ops->truncate = nufs_truncate;
  ops->open = nufs_open;
  ops->read = nufs_read;
  ops->write = nufs_write;
  ops->utimens = nufs_utimens;
  ops->ioctl = nufs_ioctl;
};

struct fuse_operations nufs_ops;

int main(int argc, char *argv[]) {
  assert(argc > 2 && argc < 6);
  printf("TODO: mount %s as data file\n", argv[--argc]);
  // storage_init(argv[--argc]);
  nufs_init_ops(&nufs_ops);
  return fuse_main(argc, argv, &nufs_ops, NULL);
}

