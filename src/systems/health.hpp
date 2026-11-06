#include <raylib.h>

class Health{
    private:
    int current_health = 0;
    int max_health = 100;

    bool isDead = false;
    
    public:
    Health(int max_hp) {
        max_health = max_hp;
        current_health = max_health; 
    }
    ~Health() {

    }

    void CheckHealth() {
        if (current_health < 0) {
            isDead = true;
        }
    }

    void UpdateHealth(int points, bool toAdd = false) {
        if(current_health > max_health) {
            return;
        }

        if(toAdd) {
            current_health += points;
        }else {
            current_health -= points;
        }

        CheckHealth();
    }
};