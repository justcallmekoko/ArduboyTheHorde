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
#define ENEMY_MAX_SPEED 3

int wave = 1;
int total_shots = 0;
int kills = 0;

int x = WIDTH / 2;
int y = HEIGHT / 2;
int mod = 60 / fps; // Consistent movement speed even with fps changes

String dir = "R";

bool just_pressed = false;
bool lose = false;

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

LinkedList<Shot> shots = LinkedList<Shot>();
LinkedList<Enemy> enemies = LinkedList<Enemy>();

bool checkCollision() {
  for (int i = 0; i < enemies.size(); i++) {
    if ((enemies.get(i).x == x) &&
        (enemies.get(i).y == y))
      lose = true;
  }
}

void generateWave() {
  for (int i = 0; i < wave; i++) {
    Enemy test_enemy;

    int spawn_x = x;
    int spawn_y = y;

    while ((spawn_x > X_MIN) &&
           (spawn_x < X_MAX) &&
           (spawn_y > Y_MIN) &&
           (spawn_y < Y_MAX)){
      spawn_x = random(X_MIN - 5, X_MAX + 5);
      spawn_y = random(X_MIN - 5, X_MAX + 5);
    }

    test_enemy.x = spawn_x;
    test_enemy.y = spawn_y;

    test_enemy.ENEMY_SPEED = random(ENEMY_MAX_SPEED, ENEMY_MIN_SPEED);

    Serial.println("Generating enemy w/ speed: " + (String)test_enemy.ENEMY_SPEED);

    enemies.add(test_enemy);
  }
  wave++;
}

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
        if (enemy.x < x)
          enemy.x++;
        if (enemy.x > x)
          enemy.x--;

        if (enemy.y < y)
          enemy.y++;
        if (enemy.y > y)
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
        kills++;
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
  arduboy.drawCircle(x, y, circle_width, WHITE);
  arduboy.display();

  generateWave();
  /*
  for (int i = 0; i < 3; i++) {
    Enemy test_enemy;

    test_enemy.x = random(X_MIN + circle_width, X_MAX - circle_width);
    test_enemy.y = random(Y_MIN + circle_width, Y_MAX - circle_width);

    enemies.add(test_enemy);
  }
  */
}

void loop() {
  if (!(arduboy.nextFrame()))
    return;

  if (!lose) {
    // Shoot
    if (arduboy.pressed(B_BUTTON)) {
      if (!just_pressed) {
        //Serial.println("Shoot direction: " + dir);
        just_pressed = true;
        total_shots++;
  
        //LinkedList<int> *shot = new LinkedList<int>();
        //shot.add(x);
        //shot.add(y);
  
        Shot shot;
        
        if (dir == "R") {
          shot.x = x;
          shot.y = y;
          shot.x_mod = BULLET_SPEED;
          shot.y_mod = 0;
          shots.add(shot);
        }
        else if (dir == "L") {
          shot.x = x;
          shot.y = y;
          shot.x_mod = -BULLET_SPEED;
          shot.y_mod = 0;
          shots.add(shot);
        }
        else if (dir == "U") {
          shot.x = x;
          shot.y = y;
          shot.x_mod = 0;
          shot.y_mod = -BULLET_SPEED;
          shots.add(shot);
        }
        else if (dir == "D") {
          shot.x = x;
          shot.y = y;
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
      mod = (60 / fps) * 2;
    else
      mod = 60 / fps;
  
    // Right
    if (arduboy.pressed(RIGHT_BUTTON) && (x < X_MAX)) {
      dir = "R";
      if (x + (1 * mod) < X_MAX)
        x = x + (1 * mod);
      else
        x++;
    }
  
    // Left
    if (arduboy.pressed(LEFT_BUTTON) && (x > X_MIN)) {
      dir = "L";
      if (x - (1 * mod) > X_MIN)
        x = x - (1 * mod);
      else
        x--;
    }
  
    // UP
    if (arduboy.pressed(UP_BUTTON) && (y > Y_MIN)) {
      dir = "U";
      if (y - (1 * mod) > Y_MIN)
        y = y - (1 * mod);
      else
        y--;
    }
  
    // Down
    if (arduboy.pressed(DOWN_BUTTON) && (y < Y_MAX)) {
      dir = "D";
      if (y + (1 * mod) < Y_MAX)
        y = y + (1 * mod);
      else
        y++;
    }
  
    //Serial.println("x: " + (String)x + " y: " + (String)y);
  
    if (enemies.size() <= 0) {
      generateWave();
    }
  
    arduboy.clear();
  
    // Draw the player
    arduboy.drawCircle(x, y, circle_width, WHITE);
  
    // Draw enemies
    runEnemies();
  
    runShots();
  
    checkCollision();
  }
  else {
    arduboy.clear();

    arduboy.println("You lose");
    arduboy.println("Waves Survived: " + (String)wave);
    arduboy.println("Kills: " + (String)kills);
    arduboy.println("Accuracy: " + (String)(((float)kills * 100.0) / (float)total_shots) + "%");
  }

  arduboy.display();

}
