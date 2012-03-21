#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 4096
#define FILEMAX 255
#define USAGE "usage: todo [-d N] [-Dhl] [ITEM]\n"
#define USAGE_FULL "Todo List\n" \
USAGE \
"\nOptions:\n" \
"-d N: Delete item #N\n" \
"-D: Delete all items\n" \
"-h: Show this help message\n" \
"-l: Show all items\n" \
"\nIf options are omitted the remaining arguments are treated as a "\
"\nnew item. If no arguments are provided, shows usage and lists all "\
"\nitems.\n"

void list_items();

int main(int argc, char **argv) {
    FILE *f, *tmp;
    bool in_delete = false;
    char c, buf[BUFSIZE], filename[FILEMAX * 2 + 1], home[FILEMAX],
         item[BUFSIZE];
    int opt_c, i, delete_i, item_i = 1;

    strncpy(home, getenv("HOME"), FILEMAX);
    sprintf(filename, "%s/%s", home, ".todo");

    while ((opt_c = getopt(argc, argv, ":dDhl")) != -1)
        switch (opt_c) {
            case 'D':
                if ((f = fopen(filename, "w"))) {
                    fclose(f);
                    puts("Deleted all items.");
                    exit(0);
                }

            case 'd':
                if (optind == argc) {
                    fprintf(stderr, "error: no item number provided to delete\n");
                    exit(1);
                }
                delete_i = atoi(argv[optind]);
                tmp = tmpfile();
                if ((f = fopen(filename, "r")) != NULL) {
                    while ((c = fgetc(f)) != EOF) {
                        in_delete = item_i == delete_i;
                        if (c == '\n')
                            item_i++;
                        if (!in_delete)
                            fputc(c, tmp);
                    }
                    fclose(f);
                }
                if ((f = fopen(filename, "w")) != NULL) {
                    fseek(tmp, 0, SEEK_SET);
                    while ((c = fgetc(tmp)) != EOF)
                        fputc(c, f);
                    fclose(f);
                    if (item_i > delete_i && delete_i > 0)
                        printf("Deleted item #%i.\n", delete_i);
                }
                fclose(tmp);
                exit(0);

            case 'l':
                list_items(filename);
                exit(0);

            case 'h':
                fprintf(stderr, USAGE_FULL);
                exit(1);
        }

    if (optind == argc) {
        fprintf(stderr, USAGE);
        list_items(filename);
        exit(1);
    }
    
    for (i = optind; i < argc; i++)
        if (strlen(item) < BUFSIZE) {
            strncat(item, argv[i], BUFSIZE - strlen(item));
            if (i < argc - 1)
                strcat(item, " ");
        }
    if (strlen(item)
            && (f = fopen(filename, "a")) != NULL 
            && fwrite(item, sizeof(item[0]), strlen(item), f)
            && fputc('\n', f)) {
        printf("Added \"%s\"\n", item);
        fclose(f);
    }
    return 0;
}

void list_items(const char *filename) {
    FILE *f;
    bool in_item = false;
    char c;
    int item_i = 0;

    if ((f = fopen(filename, "r")) != NULL) {
        while ((c = fgetc(f)) != EOF) {
            if (!in_item) {
                printf("%d. ", ++item_i);
                in_item = true;
            }
            if (c == '\n')
                in_item = false;
            putc(c, stdout);
        }
        fclose(f);
    }
    exit(0);
}
