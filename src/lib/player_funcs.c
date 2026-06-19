#include "player_funcs.h"
#include "./db/func.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void play(audiobook book){
    if (!book.link) return;
    pid_t pid = fork();
    if (pid < 0) {
        return;
    }
    if (pid == 0) {
        execlp("xdg-open", "xdg-open", book.link, (char*)NULL);
        _exit(EXIT_FAILURE);
    }
    waitpid(pid, NULL, 0);
}

audiobook pick_random(void){
    audiobook empty = {NULL, NULL, 0, -1};
    audiobook* list = get_audiobooks();
    if (!list) return empty;

    size_t count = 0;
    while (list[count].id != -1) ++count;

    if (count == 0) {
        free(list);
        return empty;
    }

    static int seeded = 0;
    if (!seeded) {
        srand((unsigned)time(NULL));
        seeded = 1;
    }

    size_t idx = (size_t)(rand() % count);
    audiobook chosen;
    chosen.id = list[idx].id;
    chosen.episode_count = list[idx].episode_count;
    chosen.name = list[idx].name ? strdup(list[idx].name) : NULL;
    chosen.link = list[idx].link ? strdup(list[idx].link) : NULL;

    for (size_t i = 0; i < count; ++i) {
        free(list[i].name);
        free(list[i].link);
    }
    free(list);

    return chosen;
}