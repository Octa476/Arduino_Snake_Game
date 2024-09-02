#include <SPI.h>
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include <EEPROM.h>

// HARDWARE SETUP

// Button - Pin configuration.
int RIGHT = 2;
int LEFT = 3;
int UP = 4;
int DOWN = 5;
int START_STOP = 6;

void setup() {
  // Decomment this line, flash and run the program a single time in order to reset the best score
  // and the comment it again to preserve the best score.
  // EEPROM.write(0, 0);

  // If analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));

  // Pin config.
  pinMode(RIGHT, INPUT);
  pinMode(LEFT, INPUT);
  pinMode(UP, INPUT);
  pinMode(DOWN, INPUT);
  pinMode(START_STOP, INPUT);

  // LCD config
  Config_Init();
  LCD_Init();
  LCD_SetBacklight(100);

  // Config the painting area and clear the screen. 
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 90, WHITE);
  Paint_Clear(BLACK);
}

// DISPLAY/MODES FUNCTIONS AND DATA

// Color vector.
const UWORD colors[] = { 0xFFFF, 0x001F, 0XF81F, 0XFFE0, 0X07FF, 0xF81F, 0xF81F,
                        0x07E0, 0x7FFF, 0xFFE0, 0XFC07, 0X8430, 0X01CF, 0x7E0,
                        0X7D7C, 0X5458, 0X841F, 0XC618, 0XA651, 0X2B12, 0xF800 };

// Square drawing function.
void Square(int x, int y, UWORD Color) {
  Paint_DrawRectangle(20 * x, 20 * y + 5, 20 * x + 20, 20 * y + 20 + 5, Color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
}

// Circle drawing function.
void Fruit(int x, int y, UWORD Color) {
  Paint_DrawCircle(20 * x + 10, 20 * y + 10 + 5, 8, Color, DOT_PIXEL_1X1, DRAW_FILL_FULL);
}

// The function draws a fruit using a random color and a random position
// that isn't taken by the snake's body.
void random_rewards(int matrix[8][16]) {
  // Search for a good random position.
  int x = random(16);
  int y = random(8);
  while (matrix[y][x] != 0) {
    x = random(16);
    y = random(8);
  }
  // Mark the position.
  matrix[y][x] = -1;
  // Choose a random color and draw the fruit.
  int COLOR = random(21);
  Fruit(x, y, colors[COLOR]);
}

// Map setter function.
void map_mode(int matrix[8][16], int map, UWORD Color, int *head_x, int *head_y) {
  // Clean the screen and reset the matrix.
  Paint_Clear(BLACK);
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 16; j++) {
      matrix[i][j] = 0;
    }
  }
  // Create the walls of the map.
  if (map == 1) {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 16; j++) {
        if ((i == 0) || (i == 7) || (j == 0) || (j == 15)) {
          matrix[i][j] = 1;
          Square(j, i, Color);
        }
      }
    }
  }
  if (map == 2) {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 16; j++) {
        if ((i == 4) || (j == 7)) {
          matrix[i][j] = 1;
          Square(j, i, Color);
        }
      }
    }
  }
  // Choose a random position for the starting coordinates.
  int x = random(16);
  int y = random(8);
  while (matrix[y][x] != 0) {
    x = random(16);
    y = random(8);
  }
  // position the snake at the starting coordinates.
  *head_x = x;
  *head_y = y;
}

// Speed mod function for the snake velocity.
int speed_mode(int speed) {
  if (speed == 0)
    return 250;
  if (speed == 1)
    return 200;
  if (speed == 2)
    return 150;
  if (speed == 3)
    return 100;
}

// Returns the color of the index index.
UWORD color_mode(int index) {
  return colors[index];
}

// GAME DATA

// The matrix represents the squares on the screen.
// The screen resolution is 170x320(height x width) and is partitioned
// into squares of 20 pixels with a buffer of 5 pixels on the y axis on each sides.
int matrix[8][16];

// 0 - right, 1 - left, 2 - up, 3 - down
int direction = 0;

// The start position of the snake.
int head_x = 0;
int head_y = 0;
int tail_x = 0;
int tail_y = 0;

// Fruit is used to store whether or not a fruit is on the screen.
bool fruit = 0;
int score = 0;

// Game modes.
int speed = 0;
int mapping = 0;
int snake_color = 0;
int map_color = 0;

// THE GAME LOOP

void loop() {
  // Reset the whole game display.
  Paint_Clear(BLACK);

  // Reset position, direction, fruit and score.
  // The position isn't determined until the player touches a button.
  direction = -1;
  fruit = 0;
  score = 0;

  mapping = 0;
  UWORD snake_color = 0;
  UWORD map_color = 0;

  int x = 0;
  int y = 0;
  int old_x = 0;
  int old_y = 0;

  // Starting screen
  Paint_DrawString_EN(40, 60, "PRESS THE BUTTON TO START", &Font24, BLACK, WHITE);
  // Wait until the button is pressed.
  while (digitalRead(START_STOP) == 0) {}

  // Speed Screen
  x = 0;
  y = 0;
  old_x = x;
  old_y = y;
  Paint_Clear(BLACK);
  Paint_DrawString_EN(50, 0, "CHOOSE SPEED", &Font24, BLACK, WHITE);
  char DIFF[4][10] ={"EASY", "MEDIUM", "HARD", "EXPERT"};
  for (int i = 0; i < 4; i++) {
    Paint_DrawString_EN(100, 20 * i + 20, DIFF[i], &Font24, BLACK, WHITE);
  }
  // Until the button is pressed, wait.
  while (digitalRead(START_STOP) == 0) {
    Paint_DrawString_EN(100, 20 * y + 20, DIFF[y], &Font24, WHITE, YELLOW);
    if (digitalRead(UP) == 1)
      if (y > 0)
        y--;
    if (digitalRead(DOWN) == 1)
      if (y < 3)
        y++;
    if (old_y != y) {
      Paint_DrawString_EN(100, 20 * old_y + 20, DIFF[old_y], &Font24, BLACK, WHITE);
    }
    old_y = y;
  }
  speed = y;

  // Map Screen
  x = 0;
  y = 0;
  old_x = x;
  old_y = y;
  Paint_Clear(BLACK);
  Paint_DrawString_EN(50, 0, "CHOOSE MAP", &Font24, BLACK, WHITE);
  char MAPS[3][15] ={"EMPTY MAP", "BORDER MAP", "CROSS MAP"};
  y = 0;
  for (int i = 0; i < 3; i++) {
    Paint_DrawString_EN(100, 20 * i + 20, MAPS[i], &Font24, BLACK, WHITE);
  }
  while (digitalRead(START_STOP) == 0) {
    Paint_DrawString_EN(100, 20 * y + 20, MAPS[y], &Font24, WHITE, YELLOW);
    if (digitalRead(UP) == 1)
      if (y > 0)
        y--;
    if (digitalRead(DOWN) == 1)
      if (y < 2)
        y++;
    if (old_y != y) {
      Paint_DrawString_EN(100, 20 * old_y + 20, MAPS[old_y], &Font24, BLACK, WHITE);
    }
    old_y = y;
  }
  mapping = y;

  // Snake_color Screen
  x = 0;
  y = 0;
  old_x = x;
  old_y = y;
  Paint_Clear(BLACK);
  Paint_DrawString_EN(40, 0, "SNAKE COLOR", &Font24, BLACK, WHITE);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 7; j++) {
      Fruit(1 + 2 * j, 2 + 2 * i, colors[i * 7 + j]);
    }
  }

  Square(1 + 2 * x, 2 + 2 * y, YELLOW);
  Fruit(1 + 2 * x, 2 + 2 * y, colors[y * 7 + x]);

  while (digitalRead(START_STOP) == 0) {

    if (digitalRead(LEFT) == 1)
      if (x > 0)
        x--;
    if (digitalRead(RIGHT) == 1)
      if (x < 6)
        x++;

    if (digitalRead(UP) == 1)
      if (y > 0)
        y--;
    if (digitalRead(DOWN) == 1)
      if (y < 2)
        y++;

    if ((old_y != y) || (old_x != x)) {

      Square(1 + 2 * old_x, 2 + 2 * old_y, BLACK);
      Fruit(1 + 2 * old_x, 2 + 2 * old_y, colors[old_y * 7 + old_x]);

      Square(1 + 2 * x, 2 + 2 * y, YELLOW);
      Fruit(1 + 2 * x, 2 + 2 * y, colors[y * 7 + x]);

    }
    old_x = x;
    old_y = y;
  }
  snake_color = color_mode(y * 7 + x);

  // Map_color Screen
  x = 0;
  y = 0;
  old_x = x;
  old_y = y;
  Paint_Clear(BLACK);
  Paint_DrawString_EN(40, 0, "MAP COLOR", &Font24, BLACK, WHITE);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 7; j++) {
      Fruit(1 + 2 * j, 2 + 2 * i, colors[i * 7 + j]);
    }
  }
  Square(1 + 2 * x, 2 + 2 * y, YELLOW);
  Fruit(1 + 2 * x, 2 + 2 * y, colors[y * 7 + x]);

  while (digitalRead(START_STOP) == 0) {

    if (digitalRead(LEFT) == 1)
      if (x > 0)
        x--;
    if (digitalRead(RIGHT) == 1)
      if (x < 6)
        x++;

    if (digitalRead(UP) == 1)
      if (y > 0)
        y--;
    if (digitalRead(DOWN) == 1)
      if (y < 2)
        y++;

    if ((old_y != y) || (old_x != x)) {

      Square(1 + 2 * old_x, 2 + 2 * old_y, BLACK);
      Fruit(1 + 2 * old_x, 2 + 2 * old_y, colors[old_y * 7 + old_x]);

      Square(1 + 2 * x, 2 + 2 * y, YELLOW);
      Fruit(1 + 2 * x, 2 + 2 * y, colors[y * 7 + x]);

    }
    old_x = x;
    old_y = y;
  }
  map_color = color_mode(y * 7 + x);

  // As long as the START-STOP button isn't pressed do nothing.
  while (digitalRead(START_STOP) == 0) {}

  // Calculate the delay, construct the map and position the snake.
  int delay_ms = speed_mode(speed);
  map_mode(matrix, mapping, map_color, &head_x, &head_y);
  // The snake is one square long at the beginning.
  tail_x = head_x;
  tail_y = head_y;
  matrix[head_x][head_y] = 1;

  // When the button is pressed, start the game.
  while (digitalRead(START_STOP) == 1) {}

  // THE GAME STARTS

  while (1) {

    Square(head_x, head_y, snake_color);
    // Wait until the player touches a button.
    while (direction == -1) {
      if (digitalRead(UP) == 1)
        direction = 2;
      if (digitalRead(DOWN) == 1)
        direction = 3;
      if (digitalRead(RIGHT) == 1)
        direction = 0;
      if (digitalRead(LEFT) == 1)
        direction = 1;
      if (digitalRead(START_STOP) == 1)
        direction = -1;
    }
    // Stop the game before it even begins.
    if (direction == -1)
      break;

    // Create a new fruit if there is none on the screen. 
    if (fruit == 0) {
      score += 1;
      fruit = 1;
      random_rewards(matrix);
    }

    // Change the direction using buttons.
    delay(delay_ms);
    if (digitalRead(START_STOP) == 1)
      break;
    if (digitalRead(UP) == 1)
      direction = 2;
    if (digitalRead(DOWN) == 1)
      direction = 3;
    if (digitalRead(RIGHT) == 1)
      direction = 0;
    if (digitalRead(LEFT) == 1)
      direction = 1;
    delay(delay_ms);

    // Change the head position.
    int old_head_val = matrix[head_y][head_x];
    int old_head_x = head_x;
    int old_head_y = head_y;

    if (direction == 2)
      head_y = (7 + head_y) % 8;
    if (direction == 3)
      head_y = (9 + head_y) % 8;

    if (direction == 0)
      head_x = (17 + head_x) % 16;
    if (direction == 1)
      head_x = (15 + head_x) % 16;

    // The head colides with the body(game over).
    if (matrix[head_y][head_x] > 0)
      break;

    // If the head reach a -1 (fruit) then the tail remains where it is.
    if (matrix[head_y][head_x] == -1) {
      fruit = 0;
      matrix[head_y][head_x] = old_head_val + 1;
      Square(head_x, head_y, snake_color);
    } else {
        //  Old value of tail.
        int old_tail_value = matrix[tail_y][tail_x];
        // The tail changes if the head doesn't reach a -1 (fruit).

        // Clear the tail.
        // Single square snake case.
        if ((tail_x == old_head_x) && (tail_y == old_head_y)) {
          Square(tail_x , tail_y, BLACK);
          matrix[tail_y][tail_x] = 0;
          tail_x = head_x;
          tail_y = head_y;
        //  Multi-square snake cases.
        } else if (matrix[tail_y][(tail_x + 1) % 16] == old_tail_value + 1) {
          Square(tail_x , tail_y, BLACK);
          matrix[tail_y][tail_x] = 0;
          tail_x = (tail_x + 1) % 16;
          tail_y = tail_y;
        } else if (matrix[tail_y][(tail_x + 15) % 16] == old_tail_value + 1) {
          Square(tail_x , tail_y, BLACK);
          matrix[tail_y][tail_x] = 0;
          tail_x = (tail_x + 15) % 16;
          tail_y = tail_y;
        } else if (matrix[(tail_y + 1) % 8][tail_x] == old_tail_value + 1) {
          Square(tail_x , tail_y, BLACK);
          matrix[tail_y][tail_x] = 0;
          tail_x = tail_x;
          tail_y = (tail_y + 1) % 8;
        } else if (matrix[(tail_y + 7) % 8][tail_x] == old_tail_value + 1) {
            Square(tail_x , tail_y, BLACK);
            matrix[tail_y][tail_x] = 0;
            tail_x = tail_x;
            tail_y = (tail_y + 7) % 8;
        }
        // Increment the value of the new head.
        matrix[head_y][head_x] = old_head_val + 1;
        Square(head_x, head_y, snake_color);
      }
  }
  // Game is over, display a message, the score and then reset it.
  delay(500);
  Paint_Clear(BLACK);
  Paint_DrawString_EN(40, 40, "GAME OVER!", &Font24, BLACK, WHITE);
  Paint_DrawString_EN(40, 80, "SCORE: ", &Font24, BLACK, WHITE);
  Paint_DrawNum(150, 80, score, &Font24, BLACK, WHITE);

  // Read the best score.
  int best_score = EEPROM.read(0);
  if (score > best_score) {
    Paint_DrawString_EN(20, 120, "NEW BEST SCORE: ", &Font24, BLACK, WHITE);
    Paint_DrawNum(280, 120, score, &Font24, BLACK, WHITE);
    EEPROM.write(0, score);
  } else {
    Paint_DrawString_EN(40, 120, "BEST SCORE: ", &Font24, BLACK, WHITE);
    Paint_DrawNum(240, 120, best_score, &Font24, BLACK, WHITE);
  }

  // Wait until the button is pressed to start a new game.
  while (digitalRead(START_STOP) == 0) {}
}



/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
