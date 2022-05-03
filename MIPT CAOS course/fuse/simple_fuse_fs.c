#define FUSE_USE_VERSION 30

#include <assert.h>
#include <errno.h>
#include <fuse.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

typedef struct {
    const char* name;
    size_t name_length;
    size_t size;
    size_t offset;
} simplefs_file_t;

struct {
    size_t count;
    simplefs_file_t* files;
    const char* data;
    size_t data_length;
} shared_data;

size_t file_index_by_path(const char* path)
{
    if (strlen(path) == 0 || path[0] != '/') {
        return -1;
    }

    for (size_t i = 0; i < shared_data.count; ++i) {
        if (strlen(path) - 1 == shared_data.files[i].name_length &&
            strncmp(
                path + 1,
                shared_data.files[i].name,
                shared_data.files[i].name_length) == 0) {
            return i;
        }
    }

    return -1;
}

int simplefs_readdir(
    const char* path,
    void* out,
    fuse_fill_dir_t filler,
    off_t off,
    struct fuse_file_info* fi,
    enum fuse_readdir_flags flags)
{
    if (strcmp(path, "/") != 0) {
        return -ENOENT;
    }

    filler(out, ".", NULL, 0, 0);
    filler(out, "..", NULL, 0, 0);

    char filename_buffer[NAME_MAX + 1];

    for (size_t i = 0; i < shared_data.count; ++i) {
        strncpy(
            filename_buffer,
            shared_data.files[i].name,
            shared_data.files[i].name_length);
        filename_buffer[shared_data.files[i].name_length] = '\0';
        filler(out, filename_buffer, NULL, 0, 0);
    }

    return 0;
}

int simplefs_getattr(
    const char* path,
    struct stat* st,
    struct fuse_file_info* fi)
{
    if (strcmp(path, "/") == 0) {
        st->st_mode = 0555 | S_IFDIR;
        st->st_nlink = 2;
        return 0;
    }

    size_t i = file_index_by_path(path);
    if (i == -1) {
        return -ENOENT;
    }

    st->st_mode = S_IFREG | 0444;
    st->st_nlink = 1;
    st->st_size = shared_data.files[i].size;
    return 0;
}

int simplefs_open(const char* path, struct fuse_file_info* fi)
{
    size_t index = file_index_by_path(path);
    if (index == -1) {
        return -1;
    }

    if (O_RDONLY != (fi->flags & O_ACCMODE)) {
        return -EACCES;
    }
    return 0;
}

int simplefs_read(
    const char* path,
    char* out,
    size_t size,
    off_t off,
    struct fuse_file_info* fi)
{
    size_t index = file_index_by_path(path);
    assert(index != -1);

    size_t file_size = shared_data.files[index].size;
    size_t offset = shared_data.files[index].offset;

    if (off > file_size) {
        return 0;
    }

    if (off + size > file_size) {
        size = file_size - off;
    }

    memcpy(out, shared_data.data + offset + off, size);
    return size;
}

size_t next_token_length(const char* str, size_t length)
{
    size_t i = 0;
    while (i < length && str[i] != '\0' && str[i] != '\n' && str[i] != ' ') {
        ++i;
    }

    return i;
}

size_t to_size_t(const char* str, size_t length)
{
    size_t result = 0;

    for (size_t i = 0; i < length; ++i) {
        result *= 10;
        result += (str[i] - '0');
    }

    return result;
}

void parse(const char* file, size_t length)
{
    size_t count_length = next_token_length(file, length);
    shared_data.count = to_size_t(file, count_length);

    file += count_length + 1;
    length -= count_length + 1;

    size_t offset = 0;

    shared_data.files = malloc(shared_data.count * sizeof(simplefs_file_t));

    for (size_t i = 0;; ++i) {
        size_t filename_length = next_token_length(file, length);
        if (filename_length == 0) {
            break;
        }

        shared_data.files[i].name = file;
        shared_data.files[i].name_length = filename_length;

        file += filename_length + 1;
        length -= filename_length + 1;

        size_t size_length = next_token_length(file, length);
        size_t size = to_size_t(file, size_length);

        shared_data.files[i].size = size;
        shared_data.files[i].offset = offset;

        offset += size;

        file += size_length + 1;
        length -= size_length + 1;
    }

    shared_data.data = file + 1;
    shared_data.data_length = length - 1;
}

int main(int argc, char** argv)
{
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    typedef struct {
        char* src;
    } options_t;

    options_t options = {NULL};

    struct fuse_opt opt_specs[] = {
        {"--src %s", offsetof(options_t, src), 0}, {NULL, 0, 0}};

    fuse_opt_parse(&args, &options, opt_specs, NULL);

    struct fuse_operations operations = {
        .readdir = simplefs_readdir,
        .getattr = simplefs_getattr,
        .open = simplefs_open,
        .read = simplefs_read};

    int src_fd = open(options.src, O_RDONLY);

    struct stat src_stat;
    fstat(src_fd, &src_stat);
    size_t length = src_stat.st_size;

    char* file = mmap(NULL, length, PROT_READ, MAP_SHARED, src_fd, 0);
    parse(file, length);

    int ret = fuse_main(args.argc, args.argv, &operations, NULL);

    free(shared_data.files);
    fuse_opt_free_args(&args);
    munmap(file, length);
    close(src_fd);

    return ret;
}