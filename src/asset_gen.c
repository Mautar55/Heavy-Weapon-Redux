#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "wutils.h"
#include <ctype.h>

char* replace_non_alnum(char *s, char replacement)
{
    if (!s) return NULL;

    for (char *p = s; *p; ++p)
    {
        unsigned char ch = (unsigned char)*p;   // important for ctype functions
        if (!isalnum(ch))
            *p = replacement;
    }
    return s;
}

char* replace_characters(char *s, char find, char replacement)
{
    if (!s) return NULL;

    for (char *p = s; *p; ++p)
    {
        unsigned char ch = (unsigned char)*p;   // important for ctype functions
        if (ch==find)
            *p = replacement;
    }
    return s;
}
#define MAX_FILEPATH_SIZE 1024

typedef struct {
    char path[MAX_FILEPATH_SIZE];
}AssetPath;

declare_list_item(AssetPath);

static void PrintFilesRelativeToAssets(const char *assetsBase, const char *dir)
{
    FilePathList list = LoadDirectoryFiles(dir);

    for (size_t i = 0; i < list.count; i++)
    {
        const char *path = list.paths[i];

        if (IsPathFile(path))
        {
            size_t baseLen = strlen(assetsBase);

            // If path begins with "<assetsBase>/", print the remainder
            if (strncmp(path, assetsBase, baseLen) == 0)
            {
                const char *rel = path + baseLen;
                if (*rel == '/' || *rel == '\\') rel++;   // skip separator
                printf("%s\n", rel);
            }
            else
            {
                // Fallback: not under base (shouldn't happen), print full path
                printf("%s\n", path);
            }
        }
        else if (DirectoryExists(path))
        {
            PrintFilesRelativeToAssets(assetsBase, path);
        }
    }

    UnloadDirectoryFiles(list);
}

static void MakeFilesRelativeToAssets(const char *assetsBase, const char *dir, WList * files)
{
    FilePathList list = LoadDirectoryFiles(dir);

    for (size_t i = 0; i < list.count; i++)
    {
        const char *path = list.paths[i];

        if (IsPathFile(path))
        {
            size_t baseLen = strlen(assetsBase);

            // If path begins with "<assetsBase>/", print the remainder
            if (strncmp(path, assetsBase, baseLen) == 0)
            {
                const char *rel = path + baseLen;
                if (*rel == '/' || *rel == '\\') rel++;   // skip separator
                AssetPath ap;
                strncpy_s(ap.path,MAX_FILEPATH_SIZE, rel, MAX_FILEPATH_SIZE);
                list_insert_last(files, &ap);
                //printf("%s\n", rel);
            }
            else
            {
                // Fallback: not under base (shouldn't happen), print full path
                printf("SHOULD NOT HAPPEN, NOT UNDER BASE %s\n", path);
            }
        }
        else if (DirectoryExists(path))
        {
            MakeFilesRelativeToAssets(assetsBase, path, files);
        }
    }

    UnloadDirectoryFiles(list);
}

void TraverseAssetsAndPrintRelativePaths(void)
{
    char assetsBase[1024] = {0};
    strcpy(assetsBase, GetWorkingDirectory());
    strcat(assetsBase, "/../assets");

    PrintFilesRelativeToAssets(assetsBase, assetsBase);
}

void CreateAssetsReferenceFile(void)
{
    char assetsBase[1024] = {0};
    strcpy(assetsBase, GetWorkingDirectory());
    strcat(assetsBase, "/../assets");

    list_new(file_paths, AssetPath);
    MakeFilesRelativeToAssets(assetsBase, assetsBase, &file_paths);

    arr_new(contents);
    arr_append_str(contents, "#pragma once\n\n");
    foreach_list(AssetPath, i, file_paths,
        replace_characters(i.path, '\\', '/');
        char this_file_name[MAX_FILEPATH_SIZE] = {0};
        strcpy_s(this_file_name, MAX_FILEPATH_SIZE, i.path);
        replace_non_alnum(this_file_name, '_');
        char part1[] = "extern const char * ass_";
        arr_append_str(contents, part1);
        arr_append_str(contents, this_file_name);
        arr_append_str(contents, " = \"../assets/");
        arr_append_str(contents, i.path);
        arr_append_str(contents, "\";\n");
    );
    SaveFileText("../src/generated_assets.h", WMemRefFromOffset(contents.items_ref)->ptr);
}

int main(void) {
    WMemClear();

    CreateAssetsReferenceFile();

    //size_t i = files.count;
    //IsPathFile(files.paths[i]);
    //DirectoryExists(files.paths[i]);
    //IsFileExtension(files.paths[i],".glb");

    WMemClear();
    return 0;
}