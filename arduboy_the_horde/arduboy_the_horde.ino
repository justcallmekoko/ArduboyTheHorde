#include <Arduboy2.h>
#include <LinkedList.h>

Arduboy2 arduboy;

#define fps 60
#define circle_width 2
#define SPAWN_DIST 20
#define X_MAX (WIDTH - circle_width - 1)
#define X_MIN circle_width
#define Y_MAX (HEIGHT - circle_width - 1)
#define Y_MIN (circle_width + 1)

#define BULLET_SPEED 4
#define ENEMY_MIN_SPEED 5 // Lower is faster
#define ENEMY_MAX_SPEED 4

//int x = WIDTH / 2;
//int y = HEIGHT / 2;
//int mod = 60 / fps; // Consistent movement speed even with fps changes

String dir = "R";

bool just_pressed = false;
bool lose = false;

class Player {
  public:
    int x = WIDTH / 2;
    int y = HEIGHT / 2;
    int mod = 60 / fps;
    
    int mov_itter = 0;
    int PSPEED = 0; // Lower is faster
    
    int wave = 1;
    int total_shots = 0;
    int kills = 0;
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
};

// Create lists for storing enemies and shots
LinkedList<Shot> shots = LinkedList<Shot>();
LinkedList<Enemy> enemies = LinkedList<Enemy>();

// Create player
Player player;

// Check if enemy hit player
bool checkCollision() {
  for (int i = 0; i < enemies.size(); i++) {
    //if ((enemies.get(i).x == player.x) &&
    //    (enemies.get(i).y == player.y))
    //  lose = true;
    
    // Calculate distance
    float distance = sqrt(sq(player.x - enemies.get(i).x) + sq(player.y - enemies.get(i).y));
    
    // Check if distance is less than the radius of the player plus the radius of the enemy
    // Player dies if enemy circle touches player circle
    if (distance <= circle_width * 2)
      lose = true;
  }
}


// Spawn enemies for the current wave
void generateWave() {
  for (int i = 0; i < player.wave; i++) {
    Enemy test_enemy;

    int spawn_x = player.x;
    int spawn_y = player.y;

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

    Serial.println("Generating enemy w/ speed: " + (String)test_enemy.ENEMY_SPEED);

    enemies.add(test_enemy);
  }
  player.wave++;
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
        //Serial.println("Moving enemy");
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

// Function to move shots and check shot collision
void runShots() {
  for (int i = 0; i < shots.size(); i++) {
    // Move coordinates based on direction
    //Serial.println("Got shot: " + (String)shots.get(i).x + ":" + (String)shots.get(i).y);

    Shot shot;
    shot.x = shots.get(i).x + shots.get(i).x_mod;
    shot.y = shots.get(i).y + shots.get(i).y_mod;
    shot.x_mod = shots.get(i).x_mod;
    shot.y_mod = shots.get(i).y_mod;

    shots.set(i, shot);

    //Serial.println("Shooting shot: " + (String)shots.get(i).x + ":" + (String)shots.get(i).y);

    // Check if off screen and if it is, remove from list
    
    if ((shots.get(i).x < X_MIN) ||
        (shots.get(i).x > X_MAX) ||
        (shots.get(i).y < Y_MIN) ||
        (shots.get(i).y > Y_MAX)) {
      shots.remove(i);
      continue;
    }
    //else {
      //shots.remove(i);
      //shots.add(coord);
      //shots.get(i) = coord;
    //}

    // Draw the shot
    //Serial.println("Drawing shot...");
    arduboy.drawCircle(shots.get(i).x, shots.get(i).y, 0, WHITE);

    // Check enemies
    for (int z = 0; z < enemies.size(); z++) {
      
      // Enemy hit if true
      if ((shots.get(i).x >= enemies.get(z).x - circle_width) && 
          (shots.get(i).x <= enemies.get(z).x + circle_width) &&
          (shots.get(i).y >= enemies.get(z).y - circle_width) &&
          (shots.get(i).y <= enemies.get(z).y + circle_width)) {
        shots.remove(i);
        enemies.get(z).dead = true;
        enemies.remove(z);
        Serial.println("Enemies remaining: " + (String)enemies.size());
        player.kills++;
        continue;
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  arduboy.begin();
  arduboy.initRandomSeed();
  arduboy.setFrameRate(fps);
  arduboy.drawCircle(player.x, player.y, circle_width, WHITE);
  arduboy.display();

  generateWave();
}

void loop() {
  if (!(arduboy.nextFrame()))
    return;

  if (!lose) {
    // Shoot
    if (arduboy.pressed(B_BUTTON)) {
      if (!just_pressed) {
        just_pressed = true;
        player.total_shots++;
  
        Shot shot;
        
        if (dir == "R") {
          shot.x = player.x;
          shot.y = player.y;
          shot.x_mod = BULLET_SPEED;
          shot.y_mod = 0;
          shots.add(shot);
        }
        else if (dir == "L") {
          shot.x = player.x;
          shot.y = player.y;
          shot.x_mod = -BULLET_SPEED;
          shot.y_mod = 0;
          shots.add(shot);
        }
        else if (dir == "U") {
          shot.x = player.x;
          shot.y = player.y;
          shot.x_mod = 0;
          shot.y_mod = -BULLET_SPEED;
          shots.add(shot);
        }
        else if (dir == "D") {
          shot.x = player.x;
          shot.y = player.y;
          shot.x_mod = 0;
          shot.y_mod = BULLET_SPEED;
          shots.add(shot);
        }
      }
    }
    else
      just_pressed = false;
  
    // Sprint
    if (arduboy.pressed(A_BUTTON))
      player.mod = (60 / fps) * 2;
    else
      player.mod = 60 / fps;
      
    player.mov_itter++;
  
    // Right
    if ((arduboy.pressed(RIGHT_BUTTON)) && (player.x < X_MAX) && (player.mov_itter >= player.PSPEED)) {
      player.mov_itter = 0;
      dir = "R";
      if (player.x + (1 * player.mod) < X_MAX)
        player.x = player.x + (1 * player.mod);
      else
        player.x++;
    }
  
    // Left
    if ((arduboy.pressed(LEFT_BUTTON)) && (player.x > X_MIN) && (player.mov_itter >= player.PSPEED)) {
      player.mov_itter = 0;
      dir = "L";
      if (player.x - (1 * player.mod) > X_MIN)
        player.x = player.x - (1 * player.mod);
      else
        player.x--;
    }
  
    // UP
    if ((arduboy.pressed(UP_BUTTON)) && (player.y > Y_MIN) && (player.mov_itter >= player.PSPEED)) {
      player.mov_itter = 0;
      dir = "U";
      if (player.y - (1 * player.mod) > Y_MIN)
        player.y = player.y - (1 * player.mod);
      else
        player.y--;
    }
  
    // Down
    if ((arduboy.pressed(DOWN_BUTTON)) && (player.y < Y_MAX) && (player.mov_itter >= player.PSPEED)) {
      player.mov_itter = 0;
      dir = "D";
      if (player.y + (1 * player.mod) < Y_MAX)
        player.y = player.y + (1 * player.mod);
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
  
    // Draw enemies
    runEnemies();
  
    runShots();
  
    checkCollision();
  }
  else {
    arduboy.clear();

    arduboy.println("You lose");
    arduboy.println("Waves Survived: " + (String)player.wave);
    arduboy.println("Kills: " + (String)player.kills);
    arduboy.println("Accuracy: " + (String)(((float)player.kills * 100.0) / (float)player.total_shots) + "%");
  }

  arduboy.display();

}
