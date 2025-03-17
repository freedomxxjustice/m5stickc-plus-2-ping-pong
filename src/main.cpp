#include <M5StickCPlus2.h>
#include <M5Unified.h>

int DISPLAY_WIDTH = 240;
int DISPLAY_HEIGHT = 135;

int scores[] = {0, 0};

float aiTargetY = DISPLAY_HEIGHT / 2;
unsigned long lastFrameTime = 0;

struct Ball
{
  float speed;
  int x;
  int y;
  int radius;
  int dx;
  int dy;
  double prevX, prevY;
} ball = {3, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, 3, 4, 4, DISPLAY_WIDTH / 2.0, DISPLAY_HEIGHT / 2.0};

struct Paddle
{
  int x;
  int y;
  int width;
  int height;
  int dy;
  int prevY;
} player = {5, DISPLAY_HEIGHT / 2 - 20, 5, 40, 3},
  enemy = {DISPLAY_WIDTH - 10, DISPLAY_HEIGHT / 2 - 20, 5, 40, 2, DISPLAY_HEIGHT / 2};

void resetBall(int direction)
{
  ball.x = DISPLAY_WIDTH / 2;
  ball.y = DISPLAY_HEIGHT / 2;

  ball.speed = 3;

  ball.dy = (rand() % 2 > 0.5 ? 1 : -1) * ball.speed;

  ball.dx = direction * ball.speed;
}

void adjustAITarget()
{
  aiTargetY = ball.y + (rand() % 2 * 60 - 30);
}

void movePaddle(Paddle *paddle, int y)
{
  paddle->prevY = paddle->y;
  StickCP2.Display.fillRect(paddle->x, paddle->y, paddle->width, paddle->height, TFT_BLACK);
  paddle->y = y;
  if (paddle->y < 0)
  {
    paddle->y = 0;
  }
  if (paddle->y + paddle->height > DISPLAY_HEIGHT)
  {
    paddle->y = DISPLAY_HEIGHT - paddle->height;
  }
  StickCP2.Display.fillRect(paddle->x, paddle->y, paddle->width, paddle->height, TFT_WHITE);
}

void resetPaddles() {
  StickCP2.Display.fillRect(player.x, player.y, player.width, player.height, TFT_BLACK);
  StickCP2.Display.fillRect(enemy.x, enemy.y, enemy.width, enemy.height, TFT_BLACK);
  player.y = DISPLAY_HEIGHT / 2;
  enemy.y = DISPLAY_HEIGHT / 2;
  StickCP2.Display.fillRect(player.x, player.y, player.width, player.height, TFT_WHITE);
  StickCP2.Display.fillRect(enemy.x, enemy.y, enemy.width, enemy.height, TFT_WHITE);
}

void handleInput()
{
  M5.update();

  if (M5.BtnA.isPressed())
    movePaddle(&player, player.y - player.dy);
  if (M5.BtnB.isPressed())
    movePaddle(&player, player.y + player.dy);
}

void moveBall()
{
  ball.prevX = ball.x;
  ball.prevY = ball.y;
  ball.x += ball.dx;
  ball.y += ball.dy;

  if (ball.y + ball.radius > DISPLAY_HEIGHT || ball.y - ball.radius < 0)
  {
    ball.dy *= -1;
  }
  if (
      ball.x - ball.radius < player.x + player.width &&
      ball.x + ball.radius > player.x &&
      ball.y - ball.radius < player.y + player.height &&
      ball.y + ball.radius > player.y)
  {
    ball.dx *= -1;

    ball.speed++;
    float collidePoint = ball.y - (player.y + player.height / 2);
    collidePoint = collidePoint / (player.height / 2);
    float angleRad = (PI / 4) * collidePoint;
    ball.dy = ball.speed * sin(angleRad);
    adjustAITarget();
  }
  if (
      ball.x - ball.radius < enemy.x + enemy.width &&
      ball.x + ball.radius > enemy.x &&
      ball.y - ball.radius < enemy.y + enemy.height &&
      ball.y + ball.radius > enemy.y)
  {
    ball.dx *= -1;
    ball.speed++;

    float collidePoint = ball.y - (enemy.y + enemy.height / 2);
    collidePoint = collidePoint / (enemy.height / 2);
    float angleRad = (PI / 4) * collidePoint;
    ball.dy = ball.speed * sin(angleRad);
    adjustAITarget();
  }

  if (ball.x + ball.radius > DISPLAY_WIDTH)
  {
    scores[0]++;
    StickCP2.Display.fillRect(DISPLAY_WIDTH / 4, 10, 50, 15, TFT_BLACK);
    StickCP2.Display.setCursor(DISPLAY_WIDTH / 4, 10);
    StickCP2.Display.printf("%d", scores[0]);
    StickCP2.Speaker.tone(10000, 100);
    sleep(1);
    StickCP2.Display.fillRect(DISPLAY_WIDTH / 4, 10, 50, 15, TFT_BLACK);
    resetPaddles();
    resetBall(-1);
  }
  if (ball.x + ball.radius < 0)
  {
    scores[1]++;
    StickCP2.Display.fillRect((3 * DISPLAY_WIDTH) / 4, 10, 50, 15, TFT_BLACK);
    StickCP2.Display.setCursor((3 * DISPLAY_WIDTH) / 4, 10);
    StickCP2.Display.printf("%d", scores[1]);
    StickCP2.Speaker.tone(10000, 100);
    sleep(1);
    StickCP2.Display.fillRect((3 * DISPLAY_WIDTH) / 4, 10, 50, 15, TFT_BLACK);
    resetPaddles();
    resetBall(1);
  }
}

void aiLogic()
{
  if (ball.dx > 0)
  {
    adjustAITarget();
  }
  if (enemy.y + enemy.height / 2 < aiTargetY)
  {
    movePaddle(&enemy, (enemy.y + enemy.dy));
  }
  else
  {
    movePaddle(&enemy, (enemy.y - enemy.dy));
  }
}

void draw()
{
  StickCP2.Display.fillCircle(ball.prevX, ball.prevY, ball.radius, TFT_BLACK);
  StickCP2.Display.fillCircle(ball.x, ball.y, ball.radius, TFT_WHITE);
}

void setup()
{
  auto cfg = M5.config();
  StickCP2.begin(cfg);
  StickCP2.Display.setRotation(3);
  StickCP2.Display.fillScreen(TFT_BLACK);
  int textsize = StickCP2.Display.height() / 60;
  if (textsize == 0)
  {
    textsize = 1;
  }
  StickCP2.Display.setTextSize(textsize);
  StickCP2.Display.setColor(TFT_WHITE);
  StickCP2.Display.fillScreen(TFT_BLACK);
  StickCP2.Display.fillRect(player.x, player.y, player.width, player.height, TFT_WHITE);
  StickCP2.Display.fillRect(enemy.x, enemy.y, enemy.width, enemy.height, TFT_WHITE);

  randomSeed(analogRead(0));
}

void loop()
{
  unsigned long currentTime = millis();
  if (currentTime - lastFrameTime < 16)
    return;
  lastFrameTime = currentTime;
  handleInput();
  moveBall();
  aiLogic();
  draw();
}