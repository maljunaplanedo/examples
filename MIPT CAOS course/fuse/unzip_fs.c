#define FUSE_USE_VERSION 30

#include <errno.h>
#include <fuse.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>

enum { PATH_MAX_SURE = PATH_MAX + 5 };

typedef struct {
    char** buffer;
    size_t capacity;
    size_t size;
} strset_t;

void strset_init(strset_t* strset)
{
    const size_t INITIAL_CAPACITY = 1;

    strset->capacity = INITIAL_CAPACITY;
    strset->size = 0;
    strset->buffer = malloc(strset->capacity * sizeof(char*));
}

void strset_destroy(strset_t* strset)
{
    free(strset->buffer);
}

char* strset_get(strset_t* strset, size_t index)
{
    return strset->buffer[index];
}

void strset_insert(strset_t* strset, char* value)
{
    for (size_t i = 0; i < strset->size; ++i) {
        if (strcmp(value, strset->buffer[i]) == 0) {
            return;
        }
    }

    if (strset->size == strset->capacity) {
        strset->capacity *= 2;
        char** new_buffer = malloc(strset->capacity * sizeof(char*));
        memcpy(new_buffer, strset->buffer, strset->size * sizeof(char*));
        free(strset->buffer);
        strset->buffer = new_buffer;
    }

    strset->buffer[strset->size++] = value;
}

void trim(const char* path, char* result)
{
    size_t len = strlen(path);
    if (len > 0 && path[0] == '/') {
        ++path;
        --len;
    }
    if (len > 0 && path[len - 1] == '/') {
        --len;
    }

    strncpy(result, path, len);
    result[len] = '\0';
}

void split_path(const char* path, char* dirname, char* name)
{
    size_t len = strlen(path);

    if (len == 0) {
        dirname[0] = '\0';
        name[0] = '\0';
        return;
    }

    size_t last_slash_idx = len - 1;
    for (;;) {
        if (path[last_slash_idx] == '/') {
            break;
        }
        if (last_slash_idx == 0) {
            last_slash_idx = -1;
            break;
        }
        --last_slash_idx;
    }

    if (last_slash_idx == -1) {
        strcpy(name, path);
        dirname[0] = '\0';
    } else {
        strncpy(dirname, path, last_slash_idx);
        dirname[last_slash_idx] = '\0';

        size_t name_len = len - last_slash_idx - 1;
        strncpy(name, path + last_slash_idx + 1, name_len);
        name[name_len] = '\0';
    }
}

zip_t* archive;

typedef enum { NOT_EXIST, IS_FILE, IS_DIR } check_t;

check_t check(const char* path)
{
    if (path[0] == '\0') {
        return IS_DIR;
    }

    zip_stat_t stat;
    if (zip_stat(archive, path, 0, &stat) != 0) {
        char path_as_dir[PATH_MAX_SURE];
        strcpy(path_as_dir, path);
        strcat(path_as_dir, "/");

        if (zip_stat(archive, path_as_dir, 0, &stat) == 0) {
            return IS_DIR;
        } else {
            return NOT_EXIST;
        }
    } else {
        return IS_FILE;
    }
}

typedef struct {
    strset_t entries;
    size_t n_dirs;
} dirinfo_t;

void dirinfo_get(dirinfo_t* dirinfo, const char* path)
{
    strset_init(&dirinfo->entries);
    dirinfo->n_dirs = 0;

    size_t entries = zip_get_num_entries(archive, 0);
    for (size_t i = 0; i < entries; ++i) {
        const char* name = zip_get_name(archive, i, 0);
        int is_dir = name[strlen(name) - 1] == '/';

        char trimmed_name[PATH_MAX_SURE];

        trim(name, trimmed_name);

        char dirname[PATH_MAX_SURE];
        char filename[PATH_MAX_SURE];

        split_path(trimmed_name, dirname, filename);

        if (strcmp(dirname, path) != 0) {
            continue;
        }

        dirinfo->n_dirs += is_dir;

        char* entry = malloc(PATH_MAX_SURE + 5);
        strcpy(entry, filename);
        strset_insert(&dirinfo->entries, entry);
    }
}

void dirinfo_destroy(dirinfo_t* dirinfo)
{
    for (size_t i = 0; i < dirinfo->entries.size; ++i) {
        free(strset_get(&dirinfo->entries, i));
    }
    strset_destroy(&dirinfo->entries);
}

int unzipfs_readdir(
    const char* path,
    void* out,
    fuse_fill_dir_t filler,
    off_t off,
    struct fuse_file_info* fi,
    enum fuse_readdir_flags flags)
{
    char trimmed_path[PATH_MAX_SURE];
    trim(path, trimmed_path);

    if (check(trimmed_path) != IS_DIR) {
        return -ENOENT;
    }

    dirinfo_t dirinfo;

    dirinfo_get(&dirinfo, trimmed_path);

    filler(out, ".", NULL, 0, 0);
    filler(out, "..", NULL, 0, 0);
    for (size_t i = 0; i < dirinfo.entries.size; ++i) {
        filler(out, strset_get(&dirinfo.entries, i), NULL, 0, 0);
    }

    dirinfo_destroy(&dirinfo);
    return 0;
}

int unzipfs_getattr(
    const char* path,
    struct stat* st,
    struct fuse_file_info* fi)
{
    char trimmed_path[PATH_MAX_SURE];
    trim(path, trimmed_path);

    check_t check_result = check(trimmed_path);
    if (check_result == NOT_EXIST) {
        return -ENOENT;
    }

    if (check_result == IS_DIR) {
        dirinfo_t dirinfo;
        dirinfo_get(&dirinfo, trimmed_path);

        st->st_mode = 0555 | S_IFDIR;
        st->st_nlink = dirinfo.n_dirs + 2;

        dirinfo_destroy(&dirinfo);
    } else {
        st->st_mode = 0444 | S_IFREG;
        st->st_nlink = 1;

        zip_stat_t stat;
        zip_stat(archive, trimmed_path, 0, &stat);

        st->st_size = stat.size;
    }

    return 0;
}

int unzipfs_open(const char* path, struct fuse_file_info* fi)
{
    char trimmed_path[PATH_MAX_SURE];
    trim(path, trimmed_path);

    if (check(trimmed_path) != IS_FILE) {
        return -ENOENT;
    }

    if (O_RDONLY != (fi->flags & O_ACCMODE)) {
        return -EACCES;
    }

    return 0;
}

int unzipfs_read(
    const char* path,
    char* out,
    size_t size,
    off_t off,
    struct fuse_file_info* fi)
{
    char trimmed_path[PATH_MAX_SURE];
    trim(path, trimmed_path);

    zip_file_t* file = zip_fopen(archive, trimmed_path, 0);

    while (off > 0) {
        off -= zip_fread(file, out, off);
    }

    size_t result = zip_fread(file, out, size);

    zip_fclose(file);

    return result;
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

    archive = zip_open(options.src, ZIP_RDONLY, NULL);

    struct fuse_operations operations = {
        .readdir = unzipfs_readdir,
        .getattr = unzipfs_getattr,
        .open = unzipfs_open,
        .read = unzipfs_read};

    int ret = fuse_main(args.argc, args.argv, &operations, NULL);

    zip_close(archive);
    fuse_opt_free_args(&args);

    return ret;
}
