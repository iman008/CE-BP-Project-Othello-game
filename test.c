#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <time.h>
#include <math.h>
#include <cjson/cjson.h>

#define BOARD_SIZE 8
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_BLACK "\x1b[30m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_WHITE "\x1b[37m"
#define ANSI_COLOR_BG_BLACK "\x1b[40m"
#define ANSI_COLOR_BG_WHITE "\x1b[47m"


typedef struct
{
    char board[BOARD_SIZE + 2][BOARD_SIZE + 2];
    char current_player;
    struct OthelloGame *previous_state;
} OthelloGame;

typedef struct
{
    char name[100];
    int score;
    int previous_score;
    bool used_prev;
    int time;
} Player;

typedef struct
{
    int x;
    int y;
} Move;

char *board_to_str(char board[BOARD_SIZE + 2][BOARD_SIZE + 2])
{
    char *res = malloc((BOARD_SIZE + 2) * (BOARD_SIZE + 2) + 1); // Adjust the size as needed
    if (res == NULL)
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    strcpy(res, "");
    for (int i = 0; i < BOARD_SIZE + 2; i++)
    {
        strncat(res, board[i], BOARD_SIZE + 2);
    }
    return res;
}

char **str_to_board(const char *str)
{
    char **board = malloc((BOARD_SIZE + 2) * sizeof(char *));
    if (board == NULL)
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < BOARD_SIZE + 2; i++)
    {
        board[i] = malloc((BOARD_SIZE + 2) * sizeof(char));
        if (board[i] == NULL)
        {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        strncpy(board[i], str + i * (BOARD_SIZE + 2), BOARD_SIZE + 2);
    }
    return board;
}

void free_board(char **board)
{
    for (int i = 0; i < BOARD_SIZE + 2; i++)
    {
        free(board[i]);
    }
    free(board);
}

void init_game(OthelloGame *game)
{
    game->previous_state = NULL;
    for (int i = 0; i < BOARD_SIZE + 2; i++)
    {
        for (int j = 0; j < BOARD_SIZE + 2; j++)
        {
            game->board[i][j] = ' ';
        }
    }
    game->board[4][4] = 'X';
    game->board[5][5] = 'X';
    game->board[4][5] = 'O';
    game->board[5][4] = 'O';
    game->current_player = 'X';
}
void printgame(const OthelloGame *const game)
{
  printf("   ");
  for (int i = 1; i <= BOARD_SIZE; i++)
  {
    printf(" %d", i);
  }
  printf("\n");

  for (int i = 1; i <= BOARD_SIZE; i++)
  {
    printf("%2d ", i);
    for (int j = 1; j <= BOARD_SIZE; j++)
    {
      char piece = game->board[i][j];
      char displayChar;

      switch (piece)
      {
      case 'X':
        printf(ANSI_COLOR_GREEN " X" ANSI_COLOR_RESET);
        break;
      case 'O':
        printf(ANSI_COLOR_BLUE " O" ANSI_COLOR_RESET);
        break;
      default:
        printf(" .");
      }
    }
    printf("\n");
  }
}
void init_game_from_json(const cJSON *json, OthelloGame *game)
{
    const cJSON *board_array = cJSON_GetObjectItem(json, "board");
    if (board_array == NULL)
    {
        fprintf(stderr, "Missing 'board' key in JSON\n");
        exit(EXIT_FAILURE);
    }

    // Convert the JSON array to a 2D char array
    char **loaded_board = str_to_board(cJSON_GetStringValue(board_array));

    // Initialize the game with the loaded board
    init_game(game);

    // Copy the loaded board to the game board
    for (int i = 0; i < BOARD_SIZE + 2; i++)
    {
        strncpy(game->board[i], loaded_board[i], BOARD_SIZE + 2);
    }

    // Set the current player
    const cJSON *current_player_json = cJSON_GetObjectItem(json, "curr");
    if (current_player_json == NULL)
    {
        fprintf(stderr, "Missing 'curr' key in JSON\n");
        exit(EXIT_FAILURE);
    }
    game->current_player = current_player_json->valuestring[0];

    // Free allocated memory
    free_board(loaded_board);
}

int main()
{
    FILE *fptr = fopen("data.json", "r");

    fseek(fptr, 0, SEEK_END);
    long file_size = ftell(fptr);
    rewind(fptr);

    char *json_data = malloc(file_size + 1);

    fread(json_data, 1, file_size, fptr);
    fclose(fptr);

    json_data[file_size] = '\0';

    cJSON *json = cJSON_Parse(json_data);

    cJSON *games_array = cJSON_GetObjectItem(json, "games");
    if (games_array == NULL)
    {
        fprintf(stderr, "Missing 'games' key in JSON\n");
        cJSON_Delete(json);
        return 1;
    }

    cJSON *game_json = cJSON_GetArrayItem(games_array, 0);
    if (game_json == NULL)
    {
        fprintf(stderr, "Missing game data in JSON\n");
        cJSON_Delete(json);
        return 1;
    }

    OthelloGame *game = malloc(sizeof(OthelloGame)); // Allocate memory for game
    if (game == NULL)
    {
        perror("Memory allocation failed");
        cJSON_Delete(json);
        return 1;
    }

    init_game_from_json(game_json, game);

    // Print the board
    char *res = board_to_str(game->board);
    printf("%s\n", res);
    printgame(game);

    // Free allocated memory
    free_board(game->board);
    free(res);
    free(game);
    cJSON_Delete(json);
    free(json_data);

    return 0;
}
