#ifndef CAT16_H
#define CAT16_H

#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct cat16_volume cat16_volume_t;
typedef struct cat16_file cat16_file_t;
typedef struct cat16_dir cat16_dir_t;

#define CAT16_SUCCESS 0

typedef struct
{
    char name[13]; // 8 + '.' + 3 + '\0'
    uint8_t attributes;
    uint32_t size;
    bool deleted; // true if this is a deleted file - only used for extra credit.
} cat16_dirent_t;

typedef struct
{
    uint8_t attributes;
    uint32_t size;
    uint16_t first_cluster;
} cat16_stat_t;

/*
 * cat16_mount -- load's a FAT16 disk image into memory.
 * Return:
 *   - NULL: on failure if the image could not be
 *      mounted --- file not found, image doesn't follow
 *      the expected format (too small, corrupted boot
 *      sector, does not use the right geometry).
 *   - Pointer to an opaque struct representing the mounted
 *      disk image. You can store whatever you need to store
 *      in this struct.
 */
cat16_volume_t *cat16_mount(const char *image_path);

/*
 * cat16_unmount -- unmounts a volume object previously
 *      returned from cat16_mount(). It should close any open
 *      file descriptors and free any memory associated with
 *      the mounted volume, including the memory for the
 *      volume struct.
 * Return:
 *  - 0 (zero) on success
 *  - On failure, it should return a negative error code.
 *      I should be able to pass this error code into the
 *      cat16_strerror() function, in order to get the
 *      corresponding error message. The standard system
 *      error message should be used for EINVAL.
 */
int cat16_unmount(cat16_volume_t *volume);

/*
 * cat16_writefile -- write a file with the specified name
 *      (path) to the mounted volume. If successful, the
 *      newly created file should have the length
 *      (bytes_to_write) and content (filedata) specified.
 *      If a file with that name already exists, it should be
 *      overwritten, and the old file data removed.
 * Return:
 *  - 0 (zero) on success.
 *  - On failures, it should return a negative error code.
 *      I should be able to pass this error code into the
 *      cat16_strerror() function, in order to get the
 *      corresponding error message. The standard system
 *      error message should be used for ENOSPC, EINVAL,
 *      ENAMETOOLONG, ENOENT, and ENOTDIR.
 */
int cat16_writefile(cat16_volume_t *volume, const char *path, uint8_t *filedata, size_t bytes_to_write);

// read a file from the volume at the specified path.
// returns
//  - on success: a pointer to the contents of the file (should be allocated on the heap).
//  - on error: NULL
// And, the number of bytes read should be stored at the address pointed to by "bytes_read".

/*
 * cat16_readfile -- read a file with the specified name
 *      (path) from the mounted volume.
 *      (bytes_to_write) and content (filedata) specified.
 *      If a file with that name already exists, it should be
 *      overwritten, and the old file data removed.
 * Return:
 *  - On success, it should return a pointer to the bytes
 *      read from the file (the file's contents) in a block
 *      of memory allocated on the heap. You should also
 *      store the number of bytes read in the memory pointed
 *      to by the "bytes_read" argument.
 *
 *  - On failure, it should return NULL, and you should store
 *      a negative error code in the volume somewhere, so
 *      that I can get the error message by calling
 *      cat16_vstrerror(). The standard system error message
 *      should be used for EINVAL, ENAMETOOLONG,
 *      ENOENT, EISDIR, and ENOTDIR.
 */
uint8_t *cat16_readfile(cat16_volume_t *volume, const char *path, size_t *bytes_read);

/*
 * cat16_listdir -- retrieve the contents (the entries) of
 *      the directory specified by the path argument on the
 *      specified mounted volume.
 * Return:
 *  - On success, it should return a pointer to a
 *      heap-allocated array of cat16_dirent_t structs, each
 *      representing a single directory entry. You should
 *      also store the number of entries returned in the
 *      memory pointed to by the "entries_read" argument.
 *      you should not include the "." and ".." entries in
 *      subdirectories.
 *
 *  - On failure, it should return NULL, and you should
 *      store a negative error code in the volume somewhere,
 *      so that I can get the error message by calling
 *      cat16_vstrerror(). The standard system error messages
 *      should be used for EINVAL, ENAMETOOLONG, ENOENT, and
 *      ENOTDIR.
 */
cat16_dirent_t *cat16_listdir(cat16_volume_t *volume, const char *path, size_t *entries_read);

/*
 * cat16_stat -- retrieve information about the file or
 *      directory pointed to by path on the specified volume.
 * Return:
 *  - On success, it should return zero (0). You should
 *      also store the requested information into the
 *      memory pointed to by the "out_stat" argument.
 *
 *  - On failure, it should return a negative error code.
 *      I should be able to pass this error code to
 *      cat16_strerror() to retrieve the corresponding error
 *      message. The standard system error messages
 *      should be used for EINVAL, ENAMETOOLONG, ENOENT, and
 *      ENOTDIR.
 */
int cat16_stat(cat16_volume_t *volume, const char *path,
               cat16_stat_t *out_stat);

/*
 * cat16_mkdir -- create a new directory with the specified
 *      name (path) on the volume. If the directory already
 *      exists don't overwrite it (EEXIST).
 * Return:
 *  - On success, it should return zero (0).
 *  - On failure, it should return a negative error code.
 *      I should be able to pass this error code to
 *      cat16_strerror() to retrieve the corresponding error
 *      message. The standard system error messages
 *      should be used for EEXIST, EINVAL, ENAMETOOLONG,
 *      ENOENT, and ENOTDIR.
 */
int cat16_mkdir(cat16_volume_t *volume, const char *path);

/*
 * cat16_unlink -- unlink the file specified by path on the
 *      volume. This function should follow the approach we
 *      discussed in class. Mark the directory entry as
 *      deleted, and zero out its FAT entries. Leave the
 *      rest of its information (directory entry and
 *      contents) alone.
 * Return:
 *  - On success, it should return zero (0).
 *  - On failure, it should return a negative error code.
 *      I should be able to pass this error code to
 *      cat16_strerror() to retrieve the corresponding error
 *      message. The standard system error messages
 *      should be used for EINVAL, ENAMETOOLONG,
 *      ENOENT, and EISDIR.
 */
int cat16_unlink(cat16_volume_t *volume, const char *path);

/*
 * cat16_rmdir -- removes a directory with the specified
 *      name (path) on the volume. This function should
 *      follow the approach we discussed in class. Mark the
 *      directory entry as deleted, and zero out its FAT
 *      entries. Leave the rest of its information
 *      (directory entry and contents) alone.
 * Return:
 *  - On success, it should return zero (0).
 *  - On failure, it should return a negative error code.
 *      I should be able to pass this error code to
 *      cat16_strerror() to retrieve the corresponding error
 *      message. The standard system error messages
 *      should be used for EINVAL, ENAMETOOLONG,
 *      ENOENT, ENOTDIR, and ENOTEMPTY.
 */
int cat16_rmdir(cat16_volume_t *volume, const char *path);

/*
 * cat16_vstrerror -- returns the error message corresponding
 *      to the last function failure. For example, if listdir
 *      just returned NULL, then this function would tell us
 *      why. Error codes should be stored on a per-volume
 *      basis. This function's behavior is undefined after
 *      a successful cat16 function return.
 * Return:
 *  - On success, it should return an error message
 *      corresponding to the last cat16_ function failure.
 *      You should use the standard error messages (returned
 *      by strerror() for the error codes specified in the
 *      project description. For any other error codes you
 *      choose to support, feel free to use an error message
 *      of your choosing.
 *  - On failure, it should return NULL. The only time it
 *      would do that is if volume is NULL.
 */
const char *cat16_vstrerror(cat16_volume_t *volume);

/*
 * cat16_vstrerror -- returns the error message corresponding
 *      to the error_code, returned from a recent cat16_
 *      function failure. For example, if mkdir returned
 *      a negative error code, then this function would
 *      tell us what that code means. Note that since these
 *      messages are read only, they can be string constants.
 *      They do not need to be allocated on the heap. In
 *      fact, it's best if they are not, because I will not
 *      free them.
 * Return:
 *  - This function should always return an error message
 *      (including for success (0) and error_codes you don't
 *      recognize).
 *      If I pass in an error code you don't recognize, then
 *      you should still return a default message. You should
 *      use the standard error messages (returned by
 *      strerror() for the error codes specified in the
 *      project description. For any other error codes you
 *      choose to support, feel free to use an error message
 *      of your choosing.
 */
const char *cat16_strerror(int error_code);

#endif