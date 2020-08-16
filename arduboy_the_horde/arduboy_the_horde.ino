#include <Arduboy2.h>
#include <LinkedList.h>

Arduboy2 arduboy;

#define START_GUN 2
#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8
#define fps 60
#define circle_width 2
#define SPAWN_DIST 20
#define STATUS_BAR 9
#define X_MAX (WIDTH - circle_width - 1)
#define X_MIN circle_width
#define Y_MAX (HEIGHT - circle_width - 1)
//#define Y_MIN (circle_width + 1 + STATUS_BAR)
#define Y_MIN (circle_width + 1)

#define SPAWN_LIMIT 15
#define BULLET_SPEED 4 // Higher is faster
#define ENEMY_MIN_SPEED 5 // Lower is faster
#define ENEMY_MAX_SPEED 4

PROGMEM const unsigned char output_map1[] = {
0xFF, 0xF1, 0xFF, 0xFF, 0xFF, 0xFC, 0x80, 0x19, 
0xFF, 0xFF, 0xFF, 0xFC, 0xB9, 0xD9, 0xFF, 0xFF, 
0xFF, 0xFC, 0xF9, 0xF9, 0xFF, 0xFF, 0xFF, 0xFC, 
0xF9, 0xF9, 0x1F, 0x0F, 0xFF, 0xFC, 0xF9, 0xF8, 
0xCE, 0x77, 0xFF, 0xFC, 0xF9, 0xF9, 0xCC, 0xF3, 
0xFF, 0xFC, 0xF9, 0xF9, 0xCC, 0xF3, 0xFF, 0xFC, 
0xF9, 0xF9, 0xCC, 0x03, 0xFF, 0xFC, 0xF9, 0xF9, 
0xCC, 0xFF, 0xFF, 0xFC, 0xF9, 0xF9, 0xCC, 0xFF, 
0xFF, 0xFC, 0xF9, 0xF9, 0xCE, 0x7B, 0xFF, 0xFC, 
0xF0, 0xF0, 0x87, 0x07, 0xFF, 0xFC, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 
0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 
0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 
0xC7, 0xFC, 0x86, 0x1F, 0xFF, 0xFF, 0xE7, 0xFC, 
0xCF, 0x3F, 0xFF, 0xFF, 0xE7, 0xFC, 0xCF, 0x3C, 
0x38, 0x9E, 0x27, 0x84, 0xCF, 0x39, 0x9C, 0x5C, 
0xC7, 0x34, 0xCF, 0x33, 0xCC, 0xF9, 0xE6, 0x70, 
0xC0, 0x33, 0xCC, 0xF9, 0xE6, 0x70, 0xCF, 0x33, 
0xCC, 0xF9, 0xE6, 0x00, 0xCF, 0x33, 0xCC, 0xF9, 
0xE6, 0x7C, 0xCF, 0x33, 0xCC, 0xF9, 0xE6, 0x7C, 
0xCF, 0x39, 0x9C, 0xFC, 0xC7, 0x38, 0x86, 0x1C, 
0x38, 0x7E, 0x23, 0x84, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00
};

PROGMEM const unsigned char output_map2[] = {
0x0F, 0x80, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 
0x00, 0x00, 0x00, 0x07, 0x80, 0x0F, 0xFF, 0xFF, 
0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x03, 0x80, 
0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 
0xC0, 0x01, 0x00, 0x3F, 0xF8, 0x3F, 0xFF, 0xFF, 
0x80, 0x00, 0x00, 0xC0, 0x01, 0x80, 0x1F, 0x80, 
0x07, 0xFF, 0xFF, 0x80, 0x00, 0x01, 0xC0, 0x00, 
0xA0, 0x0F, 0x00, 0x03, 0xFF, 0xFE, 0x80, 0x00, 
0x01, 0xC0, 0x00, 0xE0, 0x0C, 0x00, 0x00, 0x7F, 
0xFF, 0x80, 0x00, 0x01, 0xC0, 0x08, 0xE0, 0x30, 
0x07, 0xE0, 0x0F, 0xFD, 0x00, 0x00, 0x03, 0xC0, 
0x00, 0xC0, 0x20, 0x3F, 0xF8, 0x07, 0xFB, 0x00, 
0x00, 0x03, 0xC0, 0x00, 0xC0, 0xE0, 0xF8, 0x0F, 
0x03, 0xFE, 0x00, 0x00, 0x03, 0xC0, 0x00, 0xC1, 
0xE3, 0xC3, 0x83, 0x81, 0xFF, 0x00, 0x00, 0x03, 
0xC0, 0x01, 0xC1, 0xFF, 0x0F, 0xF0, 0x40, 0x7F, 
0x00, 0x00, 0x03, 0xC0, 0x03, 0x80, 0xF0, 0x3F, 
0xF8, 0x20, 0x67, 0x00, 0x00, 0x03, 0xC0, 0x07, 
0x1A, 0xE0, 0xFF, 0xFC, 0x10, 0x2F, 0x00, 0x00, 
0x03, 0xC0, 0x06, 0x1F, 0xF0, 0x7F, 0xFF, 0x08, 
0x1C, 0x00, 0x00, 0x03, 0xC0, 0x02, 0x1F, 0xF6, 
0x7F, 0xFF, 0x83, 0x10, 0x00, 0x00, 0x03, 0xC0, 
0x00, 0x1F, 0xFE, 0x3F, 0xFF, 0xC0, 0x00, 0x00, 
0x00, 0x03, 0xC0, 0x00, 0x1F, 0xFF, 0x3F, 0xFF, 
0xE0, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x0F, 
0xFF, 0x0F, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x01, 
0xC0, 0x02, 0x0F, 0xFF, 0x87, 0xFF, 0xF0, 0x01, 
0x80, 0x1E, 0x01, 0xC0, 0x06, 0x07, 0xFF, 0xC1, 
0xFF, 0xE0, 0x07, 0x80, 0xFF, 0x81, 0xC0, 0x06, 
0x03, 0xFF, 0xC4, 0x7F, 0x00, 0x0E, 0x01, 0xFF, 
0x81, 0xC0, 0x06, 0x01, 0xFF, 0xE3, 0x00, 0x01, 
0x9C, 0x03, 0xFF, 0x01, 0xC0, 0x04, 0x10, 0xFF, 
0xF1, 0x80, 0x03, 0xB8, 0x00, 0xFE, 0x03, 0xC0, 
0x0D, 0x18, 0xFF, 0xF8, 0xE0, 0x07, 0x30, 0x00, 
0x08, 0x07, 0xC0, 0x08, 0x18, 0xFF, 0xFC, 0x3F, 
0xC6, 0x70, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x3E, 
0x7F, 0xFF, 0x1F, 0xE0, 0x60, 0x00, 0x00, 0x1F, 
0xC0, 0x00, 0x3F, 0x7F, 0xFF, 0xC7, 0xE0, 0xC0, 
0x00, 0x00, 0x1F, 0xC0, 0x00, 0x27, 0x3F, 0xFF, 
0xFF, 0xE0, 0x80, 0x00, 0x00, 0x3F, 0xC0, 0x00, 
0x7F, 0xBF, 0xFF, 0xFF, 0xE0, 0x80, 0x00, 0x00, 
0x3F, 0xC0, 0x00, 0x7E, 0xFF, 0xFF, 0xFF, 0xE0, 
0x00, 0x00, 0x00, 0x3F, 0xC0, 0x00, 0x7E, 0xFF, 
0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x7F, 0xC0, 
0x00, 0xFD, 0xBF, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 
0x00, 0x7F, 0xC0, 0x00, 0xFF, 0xB7, 0xF0, 0x50, 
0x80, 0x00, 0x00, 0x00, 0xFF, 0xC0, 0x01, 0xFF, 
0x67, 0x80, 0x7E, 0x0C, 0x00, 0x00, 0x01, 0xFF, 
0xC0, 0x07, 0xFF, 0x4F, 0x00, 0xFF, 0x3C, 0x00, 
0x00, 0xE3, 0xFF, 0xC0, 0x05, 0xFE, 0x4E, 0x00, 
0x3F, 0xFE, 0x00, 0x00, 0xFF, 0xFF, 0xC0, 0x05, 
0xFE, 0x0C, 0x00, 0x3F, 0xF2, 0x00, 0x01, 0xFF, 
0xFF, 0xC0, 0x05, 0xFE, 0x18, 0x04, 0x67, 0x77, 
0x00, 0x01, 0xFF, 0xFF, 0xC0, 0x05, 0x9F, 0x80, 
0x02, 0xC4, 0xF5, 0x00, 0x03, 0xFF, 0xFF, 0xC0, 
0x03, 0xBD, 0x80, 0x03, 0xDD, 0xE5, 0x00, 0x03, 
0xFF, 0xFF, 0xC0, 0x03, 0xFD, 0xA0, 0x03, 0xDC, 
0x44, 0x00, 0x03, 0xFF, 0xFF, 0xC0, 0x03, 0xFF, 
0xA0, 0x00, 0xFF, 0xD0, 0x00, 0x07, 0xFF, 0xFF, 
0xC0, 0x01, 0xFF, 0xA0, 0x00, 0x3D, 0xB0, 0x00, 
0x07, 0xFF, 0xFF, 0xC0, 0x00, 0xFF, 0x60, 0x00, 
0x0F, 0xF3, 0x00, 0x07, 0xFF, 0xFF, 0xC0, 0x01, 
0xFF, 0x60, 0x00, 0x03, 0xF3, 0x20, 0x0F, 0xFF, 
0xFF, 0xC0, 0x00, 0xFF, 0xC0, 0x00, 0x00, 0x37, 
0x60, 0x0F, 0xFF, 0xFF, 0xC0, 0x00, 0x7F, 0x80, 
0x00, 0x00, 0x07, 0x64, 0x0F, 0xFF, 0xFF, 0xC0, 
0x00, 0x30, 0x00, 0x00, 0x00, 0x02, 0x64, 0x1F, 
0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x44, 0x3F, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x04, 0x7F, 0xFF, 0xFF, 
0xC0, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x0F, 
0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x01, 0x80, 
0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 
0x00, 0x01, 0x9C, 0x00, 0x00, 0x07, 0xFF, 0xFF, 
0xFF, 0xC0, 0x00, 0x00, 0x03, 0x1C, 0x00, 0x00, 
0x03, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x27, 
0x1C, 0x80, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xC0, 
0x00, 0x00, 0x1F, 0x01, 0xC0, 0x00, 0x01, 0xFF, 
0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x1D, 0x73, 0xC8, 
0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 
0x1E, 0xFB, 0xD8, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 
0xC0, 0x00, 0x00, 0x3E, 0xFF, 0x98, 0x00, 0x00, 
0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x0E, 0xFF, 
0x98, 0x88, 0x00, 0xFF, 0xFF, 0xFF, 0xC0, 0x10, 
0x00, 0x1F, 0xFF, 0x00, 0x08, 0x00, 0x1F, 0xFF, 
0xFF, 0xC0, 0x18, 0x00, 0x17, 0xBF, 0x60, 0x10, 
0x00, 0x0F, 0xFF, 0xFF, 0xC0, 0x10, 0x00, 0x0B, 
0xFF, 0xF0, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0xC0, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00};

String dir = "R";

bool just_pressed = false;
bool lose = false;
int current_text_size = 1;
int menu_index = 0;
int queue = 0;
uint8_t current_text_color = WHITE;
uint8_t current_background = BLACK;

// Run modes
// 0 - Menu
// 1 - Game
// 2 - Lose
// 3 - SFX
// 4 - Restart game
int mode = 0;

struct Menu;

struct MenuNode {
  String name;
  int mode;
};

// Full Menus
struct Menu {
  LinkedList<MenuNode>* list;
};

class Player {
  public:
    int x = WIDTH / 2;
    int y = HEIGHT / 2;
    //int mod = 60 / fps;

    // Weapon info
    /* Types
     * 0 - Pistol
     * 1 - Machine Gun
     * 2 - RPG
     */
    int gun_type = START_GUN; // Start with this gun
    int fire_rate = 10; // Lower is faster
    int bullet_itter = fire_rate;
        
    int wave = 1;
    int total_shots = 0;
    int kills = 0;
};

class Powerup {
  public:
    int type = 0;
    int x = 0;
    int y = 0;
};

class Enemy {
  public:
    int x = 0;
    int y = 0;
    bool dead = false;

    int mov_itter = 0;
    int ENEMY_SPEED = 0;
};

class Shot {
  public:
    int x = 0;
    int y = 0;
    int x_mod = 0;
    int y_mod = 0;
    int effect = 0;
    //int range = -1;
    int bsize = 0;
};

class Explosion {
  public:
    int x = 0;
    int y = 0;
    int r = 0;
    int lim = 18;
};

// Create lists for storing enemies and shots
LinkedList<Shot> shots = LinkedList<Shot>();
LinkedList<Enemy> enemies = LinkedList<Enemy>();
LinkedList<Powerup> powerups = LinkedList<Powerup>();
LinkedList<Explosion> explosions = LinkedList<Explosion>();

// Create player
Player player;

// Create main menu
Menu mainMenu;

// Create lose menu
Menu loseMenu;

/*
void drawStatusBar() {
  arduboy.fillRect(0, 0, WIDTH, STATUS_BAR, WHITE);
  
  // Setup text param
  arduboy.setTextSize(1);
  arduboy.setTextColor(BLACK);
  arduboy.setTextBackground(WHITE);
  
  // Draw status bar text
  arduboy.setCursor(1, 1);
  arduboy.print("Wave: " + (String)(player.wave - 1));
  
  arduboy.setCursor(70, 1);
  arduboy.print("Alive: " + (String)enemies.size());
  
  // Return text param to normal
  arduboy.setTextSize(current_text_size);
  arduboy.setTextColor(current_text_color);
  arduboy.setTextBackground(current_background);
}
*/

// Check if enemy hit player
bool checkCollision() {
  for (int i = 0; i < enemies.size(); i++) {
    
    // Calculate distance
    float distance = sqrt(sq(player.x - enemies.get(i).x) + sq(player.y - enemies.get(i).y));
    
    // Check if distance is less than the radius of the player plus the radius of the enemy
    // Player dies if enemy circle touches player circle
    if (distance <= circle_width * 2) {
      lose = true;
      mode = 2;
    }
  }
}

void runExplosions() {
  for (int i = 0; i < explosions.size(); i++) {
    Explosion explosion;
    for (int z = 0; z < enemies.size(); z++) {
      float distance = sqrt(sq(explosions.get(i).x - enemies.get(z).x) + sq(explosions.get(i).y - enemies.get(z).y));

      explosion.x = explosions.get(i).x;
      explosion.y = explosions.get(i).y;
      explosion.r = explosions.get(i).r + 1;
      explosion.lim = explosions.get(i).lim;

      if (explosion.r > explosion.lim) {
        explosions.remove(i);
        continue;
      }
      else
        explosions.set(i, explosion);

      arduboy.drawCircle(explosion.x, explosion.y, explosion.r, WHITE);

      // Check if enemy in explosion radius
      if (distance <= explosions.get(i).r + circle_width)
        enemies.remove(z);
    }
  }
}

void spawnEnemy(int count) {
  for (int i = 0; i < count; i++) {
    Enemy test_enemy;
  
    int spawn_x = player.x;
    int spawn_y = player.y;
  
    // Only spawn enemies off screen
    while ((spawn_x > X_MIN) &&
           (spawn_x < X_MAX) &&
           (spawn_y > Y_MIN) &&
           (spawn_y < Y_MAX)){
      spawn_x = random(X_MIN - 5, X_MAX + 5);
      spawn_y = random(X_MIN - 5, X_MAX + 5);
    }
  
    test_enemy.x = spawn_x;
    test_enemy.y = spawn_y;
  
    test_enemy.ENEMY_SPEED = random(ENEMY_MAX_SPEED - 1, ENEMY_MIN_SPEED + 1);
  
    //Serial.println("Generating enemy w/ speed: " + (String)test_enemy.ENEMY_SPEED);
  
    enemies.add(test_enemy);
  }
}

void checkEnemyQueue() {
  if ((enemies.size() < SPAWN_LIMIT) && (queue > 0)) {
    for (int i = 0; i < queue; i++) {
      //Serial.println("Enemies spawned from queue: " + (String)i);
      spawnEnemy(1);
      queue--;
      if (enemies.size() >= SPAWN_LIMIT)
        break;
    }
  }
}

// Spawn enemies for the current wave
void generateWave() {
  //Serial.println("Wave: " + (String)player.wave);
  for (int i = 0; i < player.wave; i++) {
    if (enemies.size() < SPAWN_LIMIT)
      spawnEnemy(1);
    else
      queue++;
  }
  player.wave++;
}

// Check if powerup gets hit by player
void runPowerups() {
  for (int i = 0; i < powerups.size(); i++) {
    // Get distance between player and powerup
    float distance = sqrt(sq(player.x - powerups.get(i).x) + sq(player.y - powerups.get(i).y));

    if (distance <= circle_width * 2) {
      player.gun_type = powerups.get(i).type;
      powerups.remove(i);
    }
    else
      arduboy.drawCircle(powerups.get(i).x, powerups.get(i).y, circle_width, WHITE);
  }
}

// Work enemy movement
void runEnemies() {
  for (int i = 0; i < enemies.size(); i++) {
    if (!enemies.get(i).dead) {
      Enemy enemy;
      enemy.mov_itter = enemies.get(i).mov_itter + 1;
      enemy.x = enemies.get(i).x;
      enemy.y = enemies.get(i).y;
      enemy.ENEMY_SPEED = enemies.get(i).ENEMY_SPEED;

      // Check if time for enemy to move
      if (enemy.mov_itter >= enemy.ENEMY_SPEED) {
        enemy.mov_itter = 0;

        // Move enemy closer to player
        if (enemy.x < player.x)
          enemy.x++;
        if (enemy.x > player.x)
          enemy.x--;

        if (enemy.y < player.y)
          enemy.y++;
        if (enemy.y > player.y)
          enemy.y--;

      }

      enemies.set(i, enemy);
      
      arduboy.drawCircle(enemies.get(i).x, enemies.get(i).y, circle_width, WHITE);
      arduboy.drawCircle(enemies.get(i).x, enemies.get(i).y, circle_width - 1, WHITE);
    }
  }
}

// Check chance of of spawning powerup
void generatePowerup(int x, int y) {
  int chance = random(0, 10);
  //Serial.println("Chance for powerup: " + (String)chance);
  if (chance == 0) {
    Powerup power;
    power.x = x;
    power.y = y;
    power.type = random(player.gun_type, 3);

    // Only generate a powerup equal or greater than the current powerup
    while (power.type < player.gun_type)
      power.type = random(player.gun_type, 3);
    powerups.add(power);
  }
}

// Function to move shots and check shot collision
void runShots() {
  for (int i = 0; i < shots.size(); i++) {
    
    // Move coordinates based on direction
    Shot shot;
    shot.x = shots.get(i).x + shots.get(i).x_mod;
    shot.y = shots.get(i).y + shots.get(i).y_mod;
    shot.x_mod = shots.get(i).x_mod;
    shot.y_mod = shots.get(i).y_mod;
    shot.effect = shots.get(i).effect;
    //shot.range = shots.get(i).range - 1;
    shot.bsize = shots.get(i).bsize;

    shots.set(i, shot);

    // Check if off screen and if it is, remove from list
    if ((shots.get(i).x < X_MIN) ||
        (shots.get(i).x > X_MAX) ||
        (shots.get(i).y < Y_MIN) ||
        (shots.get(i).y > Y_MAX)) {
        //(shot.range == 0)) {
      shots.remove(i);
      continue;
    }

    // Draw the shot
    arduboy.drawCircle(shots.get(i).x, shots.get(i).y, shots.get(i).bsize, WHITE);

    // Check enemies
    for (int z = 0; z < enemies.size(); z++) {
      
      // Enemy hit if true
      if ((shots.get(i).x >= enemies.get(z).x - circle_width) && 
          (shots.get(i).x <= enemies.get(z).x + circle_width) &&
          (shots.get(i).y >= enemies.get(z).y - circle_width) &&
          (shots.get(i).y <= enemies.get(z).y + circle_width)) {
        generatePowerup(enemies.get(z).x, enemies.get(z).y);
        bulletEffect(shots.get(i).effect, shots.get(i).x, shots.get(i).y);
        shots.remove(i);
        enemies.get(z).dead = true;
        enemies.remove(z);
        //Serial.println("Enemies remaining: " + (String)enemies.size());
        player.kills++;
        continue;
      }
    }
  }
}

// Reset game values
void restartGame() {
  // Remove all enemies
  enemies.clear();
  powerups.clear();
  shots.clear();
  explosions.clear();
  
  // Reset player values
  player.x = WIDTH / 2;
  player.y = HEIGHT / 2;
  //player.mod = 60 / fps;
      
  player.wave = 1;
  player.total_shots = 0;
  player.kills = 0;

  player.gun_type = START_GUN;
  
  // Start game
  //Serial.println("Restart Enemies: " + (String)enemies.size() + " Restart Waves: " + (String)player.wave);
  generateWave();
  
  lose = false;
  mode = 1;
}

// Function to add MenuNodes to a menu
void addNodes(Menu * menu, String name, int mode)
{
  menu->list->add(MenuNode{name, mode});
}

void runMenu(Menu * menu, bool inverted = false) {
  // Iterate through menu items
  for (int i = 0; i < menu->list->size(); i++) {

    // Format text to be printed on screen
    int num_chars = menu->list->get(i).name.length();
    //int x = (WIDTH / 2) - (num_chars * CHAR_WIDTH / 2);
    int x = WIDTH - (num_chars * CHAR_WIDTH) - 5;
    int y = (HEIGHT / 2) - (CHAR_HEIGHT / 2);
    arduboy.setCursor(x, y + (i * CHAR_HEIGHT) + 20);
    if (inverted) {
      arduboy.setTextColor(BLACK);
      arduboy.setTextBackground(WHITE);
    }
    arduboy.print(menu->list->get(i).name);
    arduboy.setTextColor(current_text_color);
    arduboy.setTextBackground(current_background);

    // Draw selector dot next to active menu item
    if (menu_index == i) {
      if (inverted)
        arduboy.drawCircle(x - 5, (y + (i * CHAR_HEIGHT)) + (CHAR_HEIGHT / 2) + 19, 1, BLACK);
      else
        arduboy.drawCircle(x - 5, (y + (i * CHAR_HEIGHT)) + (CHAR_HEIGHT / 2) + 19, 1, WHITE);
    }
  }

  // Cycle Down menu index
  if (arduboy.justPressed(UP_BUTTON)) {
    menu_index--;
    if (menu_index <= 0)
      menu_index = 0;
    //Serial.println("menu_index: " + (String)menu_index);
  }

  // Cycle Up menu index
  if (arduboy.justPressed(DOWN_BUTTON)) {
    menu_index++;
    if (menu_index >= menu->list->size() - 1)
      menu_index = menu->list->size() - 1;
    //Serial.println("menu_index: " + (String)menu_index);
  }

  // Menu item is selected
  if (arduboy.justPressed(B_BUTTON)) {
    mode = menu->list->get(menu_index).mode;
    menu_index = 0;
  }
}

void bulletEffect(int effect, int x, int y) {
  // RPG Burst
  //Serial.println("Running bullet effect: " + (String)effect);
  if (effect == 2) {
    Explosion explosion;
    explosion.x = x;
    explosion.y = y;
    explosion.r = 0;
    explosions.add(explosion);
    /*
    matchShot(x, y, BULLET_SPEED, 0, 0, 8);
    matchShot(x, y, -BULLET_SPEED, 0, 0, 8);
    matchShot(x, y, BULLET_SPEED, BULLET_SPEED, 0, 5);
    matchShot(x, y, -BULLET_SPEED, BULLET_SPEED, 0, 5);
    matchShot(x, y, 0, BULLET_SPEED, 0, 8);
    matchShot(x, y, 0, -BULLET_SPEED, 0, 8);
    matchShot(x, y, BULLET_SPEED, -BULLET_SPEED, 0, 5);
    matchShot(x, y, -BULLET_SPEED, -BULLET_SPEED, 0, 5);
    */
  }
}

//void matchShot(int x, int y, int xmod, int ymod, int effect, int range) {
void matchShot(int x, int y, int xmod, int ymod, int effect) {
  //Serial.println("Creating shot with effect: " + (String)effect);
  Shot shot;
  shot.x = x;
  shot.y = y;
  if (effect != 2) {
    shot.x_mod = xmod;
    shot.y_mod = ymod;
  }
  else {
    shot.x_mod = xmod / 2;
    shot.y_mod = ymod / 2;
  }
  shot.effect = effect;
  //shot.range = range;

  if (shot.effect == 2)
    shot.bsize = 1;
    
  shots.add(shot);
}

void setup() {
  arduboy.begin();
  Serial.begin(115200);

  // Setup menus
  mainMenu.list = new LinkedList<MenuNode>();
  loseMenu.list = new LinkedList<MenuNode>();

  // Populate menus with menu options
  addNodes(&mainMenu, "Start", 4);
  //addNodes(&mainMenu, "SFX", 3);

  addNodes(&loseMenu, "Play Again", 4);
  addNodes(&loseMenu, "Main Menu", 0);
  
  arduboy.initRandomSeed();
  arduboy.setFrameRate(fps);
  arduboy.drawCircle(player.x, player.y, circle_width, WHITE);
  arduboy.display();

  generateWave();
}

void loop() {
  if (!(arduboy.nextFrame()))
    return;

  arduboy.pollButtons();

  // This does not scale well
  // Main Menu
  if (mode == 0) {
    arduboy.clear();
    arduboy.fillRect(81,30,47,34,WHITE);
    arduboy.fillRect(127,0,1,31,WHITE);
    arduboy.drawSlowXYBitmap(0,0,output_map2,88,64,1);
    arduboy.drawSlowXYBitmap(81,0,output_map1,48,31,1);
    runMenu(&mainMenu, true);
  }

  // Play game
  else if ((!lose) && (mode == 1)) {
    
    // Shoot
    // Check if shoot button is pressed
    if (((arduboy.justPressed(B_BUTTON)) && (player.gun_type == 0)) ||
        ((arduboy.justPressed(B_BUTTON)) && (player.gun_type == 2)) ||
        ((arduboy.pressed(B_BUTTON)) && (player.gun_type == 1))) {
      player.total_shots++;

      player.bullet_itter++;

      // Check if fire rate allows shot to be fired
      if ((player.gun_type == 0) ||
          (player.gun_type == 2) ||
         ((player.gun_type == 1) && (player.bullet_itter >= player.fire_rate))) {
        player.bullet_itter = 0;
  
        Shot shot;
        
        if (dir == "R") {
          matchShot(player.x, player.y, BULLET_SPEED, 0, player.gun_type); // This
          /*
          matchShot(player.x, player.y, BULLET_SPEED, 0);
          matchShot(player.x, player.y, -BULLET_SPEED, 0);
          matchShot(player.x, player.y, BULLET_SPEED, BULLET_SPEED);
          matchShot(player.x, player.y, -BULLET_SPEED, BULLET_SPEED);
          matchShot(player.x, player.y, 0, BULLET_SPEED);
          matchShot(player.x, player.y, 0, -BULLET_SPEED);
          matchShot(player.x, player.y, BULLET_SPEED, BULLET_SPEED);
          matchShot(player.x, player.y, BULLET_SPEED, -BULLET_SPEED);
          matchShot(player.x, player.y, -BULLET_SPEED, -BULLET_SPEED);
          */
        }
        else if (dir == "L") {
          matchShot(player.x, player.y, -BULLET_SPEED, 0, player.gun_type); // This
          /*
          matchShot(player.x, player.y, BULLET_SPEED, 0);
          matchShot(player.x, player.y, -BULLET_SPEED, 0);
          matchShot(player.x, player.y, BULLET_SPEED, BULLET_SPEED);
          matchShot(player.x, player.y, -BULLET_SPEED, BULLET_SPEED);
          matchShot(player.x, player.y, 0, BULLET_SPEED);
          matchShot(player.x, player.y, 0, -BULLET_SPEED);
          matchShot(player.x, player.y, BULLET_SPEED, BULLET_SPEED);
          matchShot(player.x, player.y, BULLET_SPEED, -BULLET_SPEED);
          matchShot(player.x, player.y, -BULLET_SPEED, -BULLET_SPEED);
          */
        }
        else if (dir == "U") {
          matchShot(player.x, player.y, 0, -BULLET_SPEED, player.gun_type); // This
          /*
          matchShot(player.x, player.y, BULLET_SPEED, 0);
          matchShot(player.x, player.y, -BULLET_SPEED, 0);
          matchShot(player.x, player.y, BULLET_SPEED, BULLET_SPEED);
          matchShot(player.x, player.y, -BULLET_SPEED, BULLET_SPEED);
          matchShot(player.x, player.y, 0, BULLET_SPEED);
          matchShot(player.x, player.y, 0, -BULLET_SPEED);
          matchShot(player.x, player.y, BULLET_SPEED, BULLET_SPEED);
          matchShot(player.x, player.y, BULLET_SPEED, -BULLET_SPEED);
          matchShot(player.x, player.y, -BULLET_SPEED, -BULLET_SPEED);
          */
        }
        else if (dir == "D") {
          matchShot(player.x, player.y, 0, BULLET_SPEED, player.gun_type); // This
          /*
          matchShot(player.x, player.y, BULLET_SPEED, 0);
          matchShot(player.x, player.y, -BULLET_SPEED, 0);
          matchShot(player.x, player.y, BULLET_SPEED, BULLET_SPEED);
          matchShot(player.x, player.y, -BULLET_SPEED, BULLET_SPEED);
          matchShot(player.x, player.y, 0, BULLET_SPEED);
          matchShot(player.x, player.y, 0, -BULLET_SPEED);
          matchShot(player.x, player.y, BULLET_SPEED, BULLET_SPEED);
          matchShot(player.x, player.y, BULLET_SPEED, -BULLET_SPEED);
          matchShot(player.x, player.y, -BULLET_SPEED, -BULLET_SPEED);
          */
        }
      }
    } // End shoot

    // Reset shoot trigger
    // Need this so machine gun shoots as soon as player presses shoot button
    if ((arduboy.justReleased(B_BUTTON)) && (player.gun_type == 1))
      player.bullet_itter = player.fire_rate;
        
    // Right
    if ((arduboy.pressed(RIGHT_BUTTON)) && (player.x < X_MAX)) {
      dir = "R";
      if (player.x + (1 * 1) < X_MAX)
        player.x = player.x + (1 * 1);
      else
        player.x++;
    }
  
    // Left
    if ((arduboy.pressed(LEFT_BUTTON)) && (player.x > X_MIN)) {
      dir = "L";
      if (player.x - (1 * 1) > X_MIN)
        player.x = player.x - (1 * 1);
      else
        player.x--;
    }
  
    // UP
    if ((arduboy.pressed(UP_BUTTON)) && (player.y > Y_MIN)) {
      dir = "U";
      if (player.y - (1 * 1) > Y_MIN)
        player.y = player.y - (1 * 1);
      else
        player.y--;
    }
  
    // Down
    if ((arduboy.pressed(DOWN_BUTTON)) && (player.y < Y_MAX)) {
      dir = "D";
      if (player.y + (1 * 1) < Y_MAX)
        player.y = player.y + (1 * 1);
      else
        player.y++;
    }
  
    // Create new wave if all enemies defeated
    if (enemies.size() <= 0) {
      generateWave();
    }
  
    arduboy.clear();
  
    // Draw the player
    arduboy.drawCircle(player.x, player.y, circle_width, WHITE);
  
    // Run our updates
    runEnemies();
  
    runShots();

    runPowerups();
  
    checkCollision();

    checkEnemyQueue();

    runExplosions();
    
    //drawStatusBar();
  }

  // Lose Menu
  else if (mode == 2){
    arduboy.clear();

    //arduboy.println("You Died");
    arduboy.println("You Died\nWaves Survived: " + (String)(player.wave - 1) +
                    "\nKills: " + (String)player.kills);
                    //"\nAccuracy: " + (String)(((float)player.kills * 100.0) / (float)player.total_shots) + "%");

    runMenu(&loseMenu);
    
    // Restart ?
    //if (arduboy.pressed(B_BUTTON)) {
    //  restartGame();
    //}
  }

  else if (mode == 4)
    restartGame();

  arduboy.display();

}
