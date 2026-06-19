#include "./lib/db/init.h"
#include "./lib/db/func.h"
#include "./lib/player_funcs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void free_audiobook_list(audiobook* list){
    if (!list) return;

    for (int i = 0; list[i].id != -1; ++i){
        free(list[i].name);
        free(list[i].link);
    }
    free(list);
}

static void print_audiobook(const audiobook* book){
    printf("%d: %s (%d episodes)\n", book->id,book->name ? book->name : "(null)",book->episode_count);
}

static void show_all_audiobooks(void){
    audiobook* list = get_audiobooks();
    if (!list){
        printf("No audiobooks found.\n");
        return;
    }

    if (list[0].id == -1){
        printf("No audiobooks found.\n");
        free(list);
        return;
    }

    printf("\n--- Audiobooks ---\n");
    for (int i = 0; list[i].id != -1; ++i){
        print_audiobook(&list[i]);
    }

    free_audiobook_list(list);
}

static int read_int(const char* prompt){
    char buf[64];
    long value;

    printf("%s", prompt);
    if (!fgets(buf, sizeof(buf), stdin)) return -1;

    value = strtol(buf, NULL, 10);
    return (int)value;
}

static void read_string(const char* prompt, char* out, size_t size){
    printf("%s", prompt);
    if (!fgets(out, size, stdin)) {
        out[0] = '\0';
        return;
    }
    out[strcspn(out, "\n")] = '\0';
}

int main(){
    init();

    for (;;){
        printf("\n=== Audiobook Menu ===\n");
        printf("1. Show all audiobooks\n");
        printf("2. Add audiobook\n");
        printf("3. Play audiobook by ID\n");
        printf("4. Play random audiobook\n");
        printf("5. Update audiobook by ID\n");
        printf("6. Delete audiobook by ID\n");
        printf("0. Exit\n");

        int choice = read_int("Select an option: ");
        system("clear");
        audiobook book;
        int id = -1, rc = -1;

        switch (choice){
            case 0:
                printf("Bye\n");
                sqlite3_close(db);
                return 0;
                break;
            case 1:
                show_all_audiobooks();
                break;
            case 2:
                char name[256];
                char link[1024];
                int episode_count;

                read_string("Name: ", name, sizeof(name));
                episode_count = read_int("Episode count: ");
                read_string("Link: ", link, sizeof(link));

                rc = add_audiobook(name, episode_count, link);
                printf(rc == SQLITE_OK ? "Added successfully.\n" : "Add failed.\n");
                break;
            case 3:
                id = read_int("Enter audiobook ID: ");
                book = get_audiobook_by_id(id);

                if (book.id == -1){
                    printf("Audiobook not found.\n");
                } else {
                    play(book);
                }

                free(book.name);
                free(book.link);
                break;
            case 4:
                book = pick_random();
                if (book.id == -1){
                    printf("No audiobooks available.\n");
                } else {
                    play(book);
                }

                free(book.name);
                free(book.link);
                break;
            case 5:
                id = read_int("Enter audiobook ID: ");
                book = get_audiobook_by_id(id);

                if (book.id == -1){
                    printf("Audiobook not found.\n");
                } else {
                    char name[256];
                    char link[1024];
                    int episode_count;

                    read_string("New name: ", name, sizeof(name));
                    episode_count = read_int("New episode count: ");
                    read_string("New link: ", link, sizeof(link));

                    audiobook updated = {name, link, episode_count, id};
                    rc = update_audiobook_by_id(&updated);
                    printf(rc == SQLITE_OK ? "Updated successfully.\n" : "Update failed.\n");
                }

                free(book.name);
                free(book.link);
                break;
            case 6:
                id = read_int("Enter audiobook ID: ");
                rc = delete_audiobook_by_id(id);
                printf(rc == SQLITE_OK ? "Deleted successfully.\n" : "Delete failed.\n");
                break;
            
            default:
                printf("Invalid option.\n");
                break;
        }
    }

    return 0;
}