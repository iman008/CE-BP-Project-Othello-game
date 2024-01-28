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
int modInverse(int a, int m)
{
  a = a % m;
  for (int x = 1; x < m; x++)
    if ((a * x) % m == 1)
      return x;
  return 1;
}

char *enc(char input[])
{
  int a=23;
  int k=7;
  int prime=101;
  int len = strlen(input);
  for (int i = 0; i < len; i++)
  {
    if (input[i] != ' ')
    {
      input[i] = ((a * (input[i] - ' ')) + k) % prime;
      input[i] += ' ';
    }
  }
  return input;
}

char *dec(char input[])
{
  int a=23;
  int k=7;
  int prime=101;
  int a_inv = modInverse(a, prime);
  int len = strlen(input);
  for (int i = 0; i < len; i++)
  {
    if (input[i] != ' ')
    {
      input[i] = (a_inv * (input[i] - ' ' - k + prime)) % prime;
      input[i] += ' ';
    }
  }
  return input;
}

void displayRemainingTime(Player *player, int remainingTime)
{
  printf("%s's remaining time: %d seconds\n", player->name, remainingTime);
}

char complement(char x)
{
  return (x == 'X') ? 'O' : 'X';
}
char *board_to_str(char board[BOARD_SIZE + 2][BOARD_SIZE + 2])
{
  char *res = malloc((BOARD_SIZE + 2) * (BOARD_SIZE + 2) + 1); 
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
void init_Player(Player *player)
{
  player->score = 0;
  player->previous_score = 0;
  player->used_prev = 0;
  player->time = 0; 
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

void copy(OthelloGame *game, OthelloGame *tmp)
{
  tmp->previous_state = NULL;

  for (int i = 0; i < BOARD_SIZE + 2; i++)
  {
    for (int j = 0; j < BOARD_SIZE + 2; j++)
    {
      tmp->board[i][j] = game->board[i][j];
    }
  }

  tmp->current_player = game->current_player;
}

bool is_valid(OthelloGame *game, Move move)
{
  if (game->board[move.x][move.y] != ' ')
  {
    return false;
  }

  char player = game->current_player;
  char opponent = complement(player);

  for (int dx = -1; dx <= 1; dx++)
  {
    for (int dy = -1; dy <= 1; dy++)
    {
      if (dx == 0 && dy == 0)
        continue;

      int x = move.x + dx;
      int y = move.y + dy;
      bool found_opponent = false;

      while (game->board[x][y] == opponent)
      {
        x += dx;
        y += dy;
        found_opponent = true;
      }

      if (found_opponent && game->board[x][y] == player)
      {
        return true; 
      }
    }
  }

  return false; 
}

void make_move(OthelloGame *game, Move move, Player *player1, Player *player2)
{
  if (!is_valid(game, move))
  {
    printf("Invalid move\n");
    game->current_player = complement(game->current_player);
    return;
  }

  char player = game->current_player;
  char opponent = complement(player);

  OthelloGame tmp;
  copy(game, &tmp);

  game->board[move.x][move.y] = player;

  int flips = 0;

  for (int dx = -1; dx <= 1; dx++)
  {
    for (int dy = -1; dy <= 1; dy++)
    {
      if (dx == 0 && dy == 0)
        continue;

      int x = move.x + dx;
      int y = move.y + dy;
      bool flag = false;

      while (game->board[x][y] != ' ')
      {
        x += dx;
        y += dy;

        if (game->board[x][y] == player)
        {
          flag = true;
          break;
        }
      }

      if (!flag)
      {
        continue;
      }

      x = move.x + dx;
      y = move.y + dy;

      while (game->board[x][y] == opponent)
      {
        game->board[x][y] = player;
        x += dx;
        y += dy;
        flips++;
      }
    }
  }

  if (player == 'X')
  {
    game->current_player = 'X';
    game->previous_state = (OthelloGame *)malloc(sizeof(OthelloGame));
    copy(&tmp, game->previous_state);
    player1->previous_score = player1->score; 
    player2->previous_score = player2->score;
    player1->score += flips + 1;
  }
  else
  {
    game->current_player = 'O';
    game->previous_state = (OthelloGame *)malloc(sizeof(OthelloGame));
    copy(&tmp, game->previous_state);
    player2->previous_score = player2->score; 
    player1->previous_score = player1->score;
    player2->score += flips + 1;
  }
}

void undo_move(OthelloGame *game, Player *player1, Player *player2)
{
  Player *currentPlayer = game->current_player == 'X' ? player1 : player2;
  printf("%d", currentPlayer->time);
  currentPlayer->time = max(currentPlayer->time - 30, 0);
  if (currentPlayer->used_prev)
  {
    printf("%s has already used the undo feature and cannot use it again.\n", currentPlayer->name);
    return;
  }

  if (game->previous_state == NULL)
  {
    printf("No previous state to undo.\n");
    return;
  }

  OthelloGame *tmp = game->previous_state;
  game->previous_state = NULL;

  int flips = 0; 
  for (int i = 0; i < BOARD_SIZE + 2; i++)
  {
    for (int j = 0; j < BOARD_SIZE + 2; j++)
    {
      if (game->board[i][j] != tmp->board[i][j])
      {
       
        flips++;
      }
      game->board[i][j] = tmp->board[i][j];
    }
  }

  player1->score = player1->previous_score;
  player2->score = player2->previous_score;
  currentPlayer->used_prev = 1;
  free(tmp); 
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

bool can_move(OthelloGame *game, char piece)
{
  char tmp = game->current_player;
  game->current_player = piece;
  for (int i = 1; i <= BOARD_SIZE; i++)
  {
    for (int j = 1; j <= BOARD_SIZE; j++)
    {
      Move move = {i, j};
      if (is_valid(game, move))
      {
        game->current_player = tmp;
        return true;
      }
    }
  }
  game->current_player = tmp;
  return false;
}

bool is_gameover(OthelloGame *game)
{
  if (can_move(game, 'X') || can_move(game, 'O'))
    return false;
  return true;
}

char rungame(Player *player1, Player *player2, OthelloGame *game, int gameMode)
{
  while (true)
  {
    system("cls");
    // printf("iman previous:%d aminprevious:%d\n",player1->previous_score,player2->previous_score);
    printf("Current Scores:\n%s: %d\n%s: %d\n", player1->name, player1->score, player2->name, player2->score);
    printgame(game);

    if (!can_move(game, game->current_player))
    {
      game->current_player = complement(game->current_player);
    }

    Move move;
    move.x = 100;
    move.y = 100;

    printf("%s's turn (%c): Enter your move (row column or -1 -1 to undo): ",
           game->current_player == 'X' ? player1->name : player2->name,
           (game->current_player));
    time_t start = time(NULL);
    time_t end = time(NULL);

  pp:
    start = time(NULL);
    scanf("%d %d", &move.x, &move.y);
    end = time(NULL);
    int elapsedTime = (int)(end - start);
    if (gameMode == 1)
    {
      (game->current_player == 'X' ? player1 : player2)->time -= elapsedTime;
    }

    if (move.x == -1 && move.y == -1)
    {
      undo_move(game, player1, player2);
    }
    else if (move.x == -2)
    {
      printf("Remaining time for %s: %d seconds\n",
             game->current_player == 'X' ? player1->name : player2->name,
             game->current_player == 'X' ? player1->time : player2->time);
      goto pp;
    }
    else if (move.x == -3 && move.y == -3)
    {
      return '-';
    }
    else
    {
      make_move(game, move, player1, player2);
      game->current_player = complement(game->current_player);
    }

    if (is_gameover(game))
    {
      if (!can_move(game, 'O'))
        return 'X';
      else if (!can_move(game, 'X'))
        return 'O';
      printf("Final Scores:\n%s: %d\n%s: %d\n", player1->name, player1->score, player2->name, player2->score);
      break;
    }
  }
  return '-';
}

void init_game_from_json(OthelloGame *game, Player *player1, Player *player2, cJSON *selected_game)
{
  strcpy(player1->name, dec(cJSON_GetStringValue(cJSON_GetObjectItem(selected_game, "player1"))));
  player1->score = cJSON_GetNumberValue(cJSON_GetObjectItem(selected_game, "player1score"));
  player1->time = cJSON_GetNumberValue(cJSON_GetObjectItem(selected_game, "player1time"));
  player1->used_prev = cJSON_GetNumberValue(cJSON_GetObjectItem(selected_game, "player1usedprev"));
  player1->previous_score = cJSON_GetNumberValue(cJSON_GetObjectItem(selected_game, "player1previousscore"));
  strcpy(player2->name, dec(cJSON_GetStringValue(cJSON_GetObjectItem(selected_game, "player2"))));
  player2->score = cJSON_GetNumberValue(cJSON_GetObjectItem(selected_game, "player2score"));
  player2->time = cJSON_GetNumberValue(cJSON_GetObjectItem(selected_game, "player2time"));
  player2->used_prev = cJSON_GetNumberValue(cJSON_GetObjectItem(selected_game, "player2usedprev"));
  player2->previous_score = cJSON_GetNumberValue(cJSON_GetObjectItem(selected_game, "player2previousscore"));

  const cJSON *board_array = cJSON_GetObjectItem(selected_game, "board");
  char **loaded_board = str_to_board(cJSON_GetStringValue(board_array));
  init_game(game);
  for (int i = 0; i < BOARD_SIZE + 2; i++)
  {
    strncpy(game->board[i], loaded_board[i], BOARD_SIZE + 2);
  }

  const cJSON *current_player_json = cJSON_GetObjectItem(selected_game, "curr");
  game->current_player = current_player_json->valuestring[0];
  const cJSON *previous_board_array = cJSON_GetObjectItem(selected_game, "previous_board");
  char **loaded_previous_board = str_to_board(cJSON_GetStringValue(previous_board_array));
  OthelloGame *tmp = malloc(sizeof(OthelloGame));

  for (int i = 0; i < BOARD_SIZE + 2; i++)
  {
    strncpy(tmp->board[i], loaded_previous_board[i], BOARD_SIZE + 2);
  }
  tmp->previous_state = NULL;
  tmp->current_player = game->current_player;
  game->previous_state = tmp;
}

void add_game_to_json(OthelloGame *game, Player *player1, Player *player2, char winner, cJSON *games_array)
{
  cJSON *new_game = cJSON_CreateObject();
  char* sss=(cJSON_CreateString(enc(player1->name)));
  printf(sss);
  cJSON_AddItemToObject(new_game, "player1", sss);
  char*ss=(cJSON_CreateString(enc(player2->name)));
  printf(ss);
  cJSON_AddItemToObject(new_game, "player2", ss);
  cJSON_AddItemToObject(new_game, "player1score", cJSON_CreateNumber(player1->score));
  cJSON_AddItemToObject(new_game, "player1time", cJSON_CreateNumber(player1->time));
  cJSON_AddItemToObject(new_game, "player1usedprev", cJSON_CreateBool(player1->used_prev));
  cJSON_AddItemToObject(new_game, "player1previousscore", cJSON_CreateNumber(player1->previous_score));
  cJSON_AddItemToObject(new_game, "player2score", cJSON_CreateNumber(player2->score));
  cJSON_AddItemToObject(new_game, "player2time", cJSON_CreateNumber(player2->time));
  cJSON_AddItemToObject(new_game, "player2usedprev", cJSON_CreateBool(player2->used_prev));
  cJSON_AddItemToObject(new_game, "board", cJSON_CreateString(board_to_str(game->board)));

  OthelloGame tmp;
  copy(game->previous_state, &tmp);
  cJSON_AddItemToObject(new_game, "previous_board", cJSON_CreateString(board_to_str(tmp.board)));

  cJSON_AddItemToObject(new_game, "player2previousscore", cJSON_CreateNumber(player2->previous_score));

  char curr[2] = {game->current_player, '\0'};
  cJSON_AddItemToObject(new_game, "curr", cJSON_CreateString(curr));

  cJSON_AddItemToObject(new_game, "winner", cJSON_CreateString(&winner));

  cJSON_AddItemToArray(games_array, new_game);
}

int main()
{
  Player player1, player2;
  init_Player(&player1);
  init_Player(&player2);

  printf("Enter the name of player 1:\n");
  scanf("%s", player1.name);
  printf("Enter the name of player 2:\n");
  scanf("%s", player2.name);

  FILE *fptr = fopen("data.json", "r");

  fseek(fptr, 0, SEEK_END);

  long file_size = ftell(fptr);
  rewind(fptr);

  char *json_data = malloc(file_size + 1);

  fread(json_data, 1, file_size, fptr);
  // fclose(fptr);

  json_data[file_size] = '\0';

  cJSON *json = cJSON_Parse(json_data);
  cJSON *games_array = cJSON_GetObjectItem(json, "games");

  printf("Select the game mode:\n");
  printf("1. Timed\n");
  printf("2. Untimed\n");
  printf("3. Select from the history\n");
  int gameMode;
  scanf("%d", &gameMode);

  if (gameMode != 3)
  {
    if (gameMode != 1 && gameMode != 2)
    {
      printf("Invalid option. Exiting.\n");
      return 1;
    }

    if (gameMode == 1)
    {
      player1.time = 600; 
      player2.time = 600; 
    }

    OthelloGame *game = malloc(sizeof(OthelloGame));
    init_game(game);
    char winner = rungame(&player1, &player2, game, gameMode);
    add_game_to_json(game, &player1, &player2, winner, games_array);

  }
  else
  { 

    int games_array_size = cJSON_GetArraySize(games_array);
    printf("%d", games_array_size);
    for (int i = 0; i < games_array_size; i++)
    {
      cJSON *specific_game = cJSON_GetArrayItem(games_array, i);
      char *player1name = dec(cJSON_GetStringValue(cJSON_GetObjectItem(specific_game, "player1")));
      char *player2name = dec(cJSON_GetStringValue(cJSON_GetObjectItem(specific_game, "player2")));
      if (!strcmp(player1name, player1.name) && !strcmp(player2name, player2.name))
      {
        char *string = cJSON_Print(specific_game);
        printf("id: %d\n", i);
        printf(string);
        printf("\n");
      }
      else
      {
        continue;
      }
    }
    int Selection = 0;
    printf("select the id of the game you want to continue:\n");
    scanf("%d", &Selection);
    cJSON *selected_game = cJSON_GetArrayItem(games_array, Selection);
    OthelloGame *game = malloc(sizeof(OthelloGame));
    init_game_from_json(game, &player1, &player2, selected_game);
    int gamemode = cJSON_GetNumberValue(cJSON_GetObjectItem(selected_game, "gamemode"));
    cJSON_DeleteItemFromArray(games_array, Selection);
    printf("%d", gamemode);
    char winner = rungame(&player1, &player2, game, gamemode);
    add_game_to_json(game, &player1, &player2, winner, games_array);
    
  }
  fptr = fopen("data.json", "w");
  char *ppr = cJSON_Print(json);
  fprintf(fptr, "%s", ppr);
  fclose(fptr);
  return 0;
}
