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
  int previous_score; // New field to store the previous score
  bool used_prev;
  int time; // New attribute for player's remaining time (in seconds)
} Player;

typedef struct
{
  int x;
  int y;
} Move;

void displayRemainingTime(Player *player, int remainingTime)
{
  printf("%s's remaining time: %d seconds\n", player->name, remainingTime);
}

char complement(char x)
{
  return (x == 'X') ? 'O' : 'X';
}

void init_Player(Player *player)
{
  player->score = 0;
  player->previous_score = 0;
  player->used_prev = 0;
  player->time = 0; // Initialize the time attribute
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
    return false; // The position is not empty
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
        return true; // At least one opponent piece is flanked by player's pieces
      }
    }
  }

  return false; // No valid move found
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
    player1->previous_score = player1->score; // Store the previous score
    player2->previous_score = player2->score;
    player1->score += flips + 1;
  }
  else
  {
    game->current_player = 'O';
    game->previous_state = (OthelloGame *)malloc(sizeof(OthelloGame));
    copy(&tmp, game->previous_state);
    player2->previous_score = player2->score; // Store the previous score
    player1->previous_score=player1->score;
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

  int flips = 0; // Keep track of the flips to revert the scores
  for (int i = 0; i < BOARD_SIZE + 2; i++)
  {
    for (int j = 0; j < BOARD_SIZE + 2; j++)
    {
      if (game->board[i][j] != tmp->board[i][j])
      {
        // A change in the board, indicating a flip
        flips++;
      }
      game->board[i][j] = tmp->board[i][j];
    }
  }

  player1->score=player1->previous_score;
  player2->score=player2->previous_score;
  currentPlayer->used_prev=1;
  free(tmp); // Free the memory allocated for the previous state
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

bool whowon(OthelloGame *game)
{
  if (can_move(game, 'X') || can_move(game, 'O'))
    return false;
  return true;
}

void rungame(Player *player1, Player *player2, OthelloGame *game, int gameMode)
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
    else
    {
      make_move(game, move, player1, player2);
      game->current_player = complement(game->current_player);
    }

    if (is_gameover(game))
    {
      printf("Final Scores:\n%s: %d\n%s: %d\n", player1->name, player1->score, player2->name, player2->score);
      break;
    }
  }
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

  printf("Select the game mode:\n");
  printf("1. Timed\n");
  printf("2. Untimed\n");
  printf("3. See the history\n");

  int gameMode;
  scanf("%d", &gameMode);

  if (gameMode!=3){
    if (gameMode != 1 && gameMode != 2)
  {
    printf("Invalid option. Exiting.\n");
    return 1;
  }

  if (gameMode == 1)
  {
    player1.time = 600; // Set the initial time for player 1 (in seconds)
    player2.time = 600; // Set the initial time for player 2 (in seconds)
  }
  
  OthelloGame game;
  init_game(&game);

  rungame(&player1, &player2, &game, gameMode);
  } else{
    return 0;
  }
  return 0;
}
