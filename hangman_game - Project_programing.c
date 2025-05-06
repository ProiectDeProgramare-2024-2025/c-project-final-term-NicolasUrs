#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define RESET  "\033[0m"
#define RED    "\033[1;31m"
#define GREEN  "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE   "\033[1;34m"

#ifdef _WIN32
    #define CLEAR_COMMAND "cls"
#else
    #define CLEAR_COMMAND "clear"
#endif

struct Player {
    char name[50];
    int score;
};

void clear_screen() {
    system(CLEAR_COMMAND);
}

void header() {
    printf("%s---------------------%s\n", BLUE, RESET);
    printf("%s1 - Start Game%s\n", GREEN, RESET);
    printf("%s2 - View Leaderboard%s\n", GREEN, RESET);
    printf("%s3 - View Game History%s\n", GREEN, RESET);
    printf("%s0 - Exit%s\n", GREEN, RESET);
    printf("%s---------------------%s\n", BLUE, RESET);
}

void load_words(char words[20][50], int *word_count) {
    FILE *file = fopen("words.txt", "r");
    if (file == NULL) return;
    *word_count = 0;
    while (fscanf(file, "%49s", words[*word_count]) != EOF && *word_count < 20) {
        (*word_count)++;
    }
    fclose(file);
}

void display_word(char current_state[]) {
    for (int i = 0; current_state[i] != '\0'; i++) {
        printf("%c ", current_state[i]);
    }
    printf("\n");
}

void update_word(char word[], char current_state[], char guess) {
    int updated = 0;
    for (int i = 0; word[i] != '\0'; i++) {
        if (word[i] == guess && current_state[i] == '_') {
            current_state[i] = guess;
            updated = 1;
        }
    }
    if (!updated) {
        printf("%sIncorrect guess! Remaining tries: %d%s\n", RED, 5, RESET);
    }
}

int check_word_completion(char current_state[]) {
    for (int i = 0; current_state[i] != '\0'; i++) {
        if (current_state[i] == '_') return 0;
    }
    return 1;
}

void start_game() {
    clear_screen();
    char words[20][50];
    int word_count;
    load_words(words, &word_count);
    if (word_count == 0) return;

    srand(time(NULL));
    char guessed_letters[50] = "";
    int incorrect_tries = 0;
    int score = 0;
    char name[50];
    int random_index = rand() % word_count;
    char *word = words[random_index];
    char current_state[50];
    strcpy(current_state, word);
    for (int i = 0; current_state[i] != '\0'; i++) {
        current_state[i] = '_';
    }

    while (incorrect_tries < 5) {
        printf(BLUE "\nCurrent word: " RESET);
        display_word(current_state);
        char guess;
        printf(YELLOW "Guess a letter: " RESET);
        scanf(" %c", &guess);
        clear_screen();

        if (strchr(guessed_letters, guess) == NULL) {
            strncat(guessed_letters, &guess, 1);
            if (strchr(word, guess) == NULL) {
                incorrect_tries++;
                printf("%sIncorrect guess! Remaining tries: %d%s\n", RED, 5 - incorrect_tries, RESET);
            } else {
                update_word(word, current_state, guess);
            }
        } else {
            printf("%sYou already guessed that letter!%s\n", YELLOW, RESET);
        }

        if (check_word_completion(current_state)) {
            printf("%s\nCongratulations! You completed the word!%s\n", GREEN, RESET);
            score += strlen(word);
            break;
        }
    }

    if (incorrect_tries == 5) {
        printf("%sGame over! You've used up all attempts.%s\n", RED, RESET);
    }

    printf("\nEnter your name: ");
    scanf("%s", name);
    clear_screen();

    FILE *file = fopen("leaderboard.txt", "a");
    if (file != NULL) {
        fprintf(file, "%s: %d\n", name, score);
        fclose(file);
    }
    FILE *history = fopen("history.txt", "a");
    if (history != NULL) {
        time_t now = time(NULL);
        fprintf(history, "Player: %s | Word: %s | Score: %d | Date: %s", name, word, score, ctime(&now));
        fclose(history);
    }

    printf("Your score: %s%d%s\n", YELLOW, score, RESET);
}

void display_leaderboard() {
    FILE *file = fopen("leaderboard.txt", "r");
    if (file == NULL) {
        printf("Leaderboard is empty.\n");
        return;
    }
    char line[100];
    printf("\n%sLeaderboard:%s\n", GREEN, RESET);
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
}

void view_game_history() {
    FILE *history = fopen("history.txt", "r");
    if (history == NULL) {
        printf("Game history is empty.\n");
        return;
    }
    char line[200];
    printf("\n%sGame History:%s\n", YELLOW, RESET);
    while (fgets(line, sizeof(line), history)) {
        printf("%s", line);
    }
    fclose(history);
}

void menu(int op) {
    clear_screen();
    switch(op) {
        case 1:
            start_game();
        break;
        case 2:
            display_leaderboard();
        break;
        case 3:
            view_game_history();
        break;
        default:
            printf("%sEXIT!%s\n", RED, RESET);
    }
}

int main() {
    int option;
    do {
        header();
        printf("Enter option: ");
        scanf("%d", &option);
        menu(option);
    } while(option > 0 && option <= 3);
    return 0;
}

