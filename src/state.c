#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
    game_state_t* start = calloc(1, sizeof(game_state_t));
    
    start -> num_rows = 18;
    start -> num_snakes = 1;
    snake_t* snek = malloc(sizeof(snake_t));
    snek -> tail_row = 2;
    snek -> tail_col = 2;
    snek -> head_row = 2;
    snek -> head_col = 4;
    snek -> live = true;
    start -> snakes = snek;

    char** B = malloc(18*sizeof(char*));
    for (int i = 0; i < 18; i++) {
        B[i] =  malloc(21*sizeof(char));
    }

    strcpy(B[0], "####################");
    strcpy(B[1], "#                  #");
    strcpy(B[2], "# d>D    *         #");
    strcpy(B[3], "#                  #");
    strcpy(B[4], "#                  #");
    strcpy(B[5], "#                  #");
    strcpy(B[6], "#                  #");
    strcpy(B[7], "#                  #");
    strcpy(B[8], "#                  #");
    strcpy(B[9], "#                  #");
    strcpy(B[10], "#                  #");
    strcpy(B[11], "#                  #");
    strcpy(B[12], "#                  #");
    strcpy(B[13], "#                  #");
    strcpy(B[14], "#                  #");
    strcpy(B[15], "#                  #");
    strcpy(B[16], "#                  #");
    strcpy(B[17], "####################");

    start -> board = B;
    return start;
}

/* Task 2 */
void free_state(game_state_t* state) {
    for (int i = 0; i < state -> num_rows; i++) {
        free(state->board[i]);
    }
    free(state->snakes);
    free(state->board);
    free(state);
    return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
    for (int i = 0; i < state -> num_rows; i++) {
        fprintf(fp, "%s", state->board[i]);
        fprintf(fp, "\n");
    }
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
    if (c == 'w' || c == 'a' || c == 's' || c == 'd') {
        return true;
    }
    return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
    if (c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x') {
        return true;
    }
    return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
    if (c == '^' || c == '<' || c == 'v' || c == '>' || is_head(c) || is_tail(c)) {
        return true;
    }
    return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
    if (c == 'v') {
        return 's';
    } else if (c == '^') {
        return 'w';
    } else if (c == '>') {
        return 'd';
    } else if (c == '<') {
        return 'a';
    }
    return '$';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
    if (c == 'S') {
        return 'v';
    } else if (c == 'W') {
        return '^';
    } else if (c == 'D') {
        return '>';
    } else if (c == 'A') {
        return '<';
    }
    return '$';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
    if (c == 'v' || c == 's' || c == 'S') {
        return cur_row + 1;
    } else if (c == '^' || c == 'w' || c == 'W') {
        return cur_row - 1;
    } else {
        return cur_row;
    }
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
    if (c == '>' || c == 'd' || c == 'D') {
        return cur_col + 1;
    } else if (c == '<' || c == 'a' || c == 'A') {
        return cur_col - 1;
    } else {
        return cur_col;
    }
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
    return get_board_at(state, get_next_row(state->snakes[snum].head_row, get_board_at(state, state->snakes[snum].head_row, state->snakes[snum].head_col)), get_next_col(state->snakes[snum].head_col, get_board_at(state, state->snakes[snum].head_row, state->snakes[snum].head_col)));
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
    char head = get_board_at(state, state->snakes[snum].head_row, state->snakes[snum].head_col);
    unsigned int next_row = get_next_row(state->snakes[snum].head_row, head);
    unsigned int next_col = get_next_col(state->snakes[snum].head_col, head);
    set_board_at(state, next_row, next_col, head);
    set_board_at(state, state->snakes[snum].head_row, state->snakes[snum].head_col, head_to_body(head));
    state->snakes[snum].head_row = next_row;
    state->snakes[snum].head_col = next_col;
    return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  //TODO: Implement this function.
    char tail = get_board_at(state, state->snakes[snum].tail_row, state->snakes[snum].tail_col);
    unsigned int next_row = get_next_row(state->snakes[snum].tail_row, tail);
    unsigned int next_col = get_next_col(state->snakes[snum].tail_col, tail);
    set_board_at(state, next_row, next_col, body_to_tail(state->board[next_row][next_col]));
    set_board_at(state, state->snakes[snum].tail_row, state->snakes[snum].tail_col, ' ');
    state->snakes[snum].tail_row = next_row;
    state->snakes[snum].tail_col = next_col;
    return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
    unsigned int bean = state->num_snakes;
    unsigned int snum;
    for (snum = 0; snum < bean; snum++) {
        char head = get_board_at(state, state->snakes[snum].head_row, state->snakes[snum].head_col);
        unsigned int next_head_r = get_next_row(state->snakes[snum].head_row, head);
        unsigned int next_head_c = get_next_col(state->snakes[snum].head_col, head);
        char next_head = state->board[next_head_r][next_head_c];

        if (is_snake(next_head) || next_head == '#') {
            state->board[state->snakes[snum].head_row][state->snakes[snum].head_col] = 'x';
            state->snakes[snum].live = false;
        } else if (next_head == '*') {
            update_head(state, snum);
            add_food(state);
        } else {
            update_head(state, snum);
            update_tail(state, snum);
        }
    }
    return;
}

/* Task 5 */
game_state_t* load_board(char* filename) {
    game_state_t* start = calloc(1, sizeof(game_state_t));
    
    start -> num_snakes = 0;
    
    start -> snakes = NULL;
    
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        return start;
    }
    char** B = malloc(sizeof(char*));    
    
    unsigned int i = 0;
    B[i] = calloc(1, sizeof(char));
    size_t n = 0;
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if ((char) c == '\n') {
            B[i][n] = '\0';
            i++;
            B = realloc(B, (i+1)*sizeof(char*));
            n = 0;
            B[i] = calloc(1, sizeof(char));
        } else {
            B[i][n] = (char) c;
            n++;
            B[i] = realloc(B[i], (n+1)*sizeof(char));
        }
    }
    
    free(B[i]);
    B = realloc(B, i*sizeof(char*));

    //for (int j = 0; j < i; j++) {
    //    printf("%s\n", B[j]);
    //}

    fclose(fp);

    start -> num_rows = i;

    start -> board = B;
    return start;
}

/* Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
    
    unsigned int tr = state->snakes[snum].tail_row;
    unsigned int tc = state->snakes[snum].tail_col;
    unsigned int temp;

    while (state->board[tr][tc] != 'W' && state->board[tr][tc] != 'A' && state->board[tr][tc] != 'S' && state->board[tr][tc] != 'D') {
        temp = get_next_row(tr, state->board[tr][tc]);
        tc = get_next_col(tc, state->board[tr][tc]);
        tr = temp;
    }
    
    state->snakes[snum].head_row = tr;
    state->snakes[snum].head_col = tc;

    return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
    
    snake_t* snek = malloc(sizeof(snake_t));
    unsigned int snum = 0;
    for (unsigned int i = 0; i < state->num_rows; i++) {
        for (unsigned int j = 0; j < strlen(state->board[i]); j++) {
            if (state->board[i][j] == 'w' || state->board[i][j] == 'a' || state->board[i][j] == 's' || state->board[i][j] == 'd') {
                snek = realloc(snek, (snum+1)*sizeof(snake_t));
                snek[snum].tail_row = i;
                snek[snum].tail_col = j;
                snum++;
            }
        }
    }
    
    state -> num_snakes = snum;
    state -> snakes = snek;

    for (unsigned int i = 0; i < snum; i++) {
        find_head(state, i);
    }

    return state;
}
