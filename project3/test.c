// if successful this program should print the following
// read: 89 bytes
// Content : just a test document.You should be able to read this one off the disk, with no problem.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "cat16.h"

int score = 0;

int main(int argc, char **argv)
{

    // copy test image
    // important for any test that modifies its disk image
    if (system("cp simple.img ./temp.img") != 0)
    {
        printf("Could not make copy of simple.img.\n");
        return EXIT_FAILURE;
    }

    cat16_volume_t *vol = cat16_mount("./temp.img");

    if (vol == NULL)
    {
        printf("mount failed\n");
        return EXIT_FAILURE;
    }

    size_t b_read;
    uint8_t *results = cat16_readfile(vol, "/A.TXT", &b_read);

    if (results == NULL)
    {
        printf("AAAAH!!! cat16_readfile returned NULL. (%s)\n", cat16_vstrerror(vol));
        return EXIT_FAILURE;
    }

    printf("read: %zu bytes\nContent: %s\n", b_read, results); // note this only works because it's a text file.

    int rc = cat16_unmount(vol);

    if (rc != CAT16_SUCCESS)
    {
        printf("unmount failed: %s\n", cat16_strerror(rc));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}