#pragma once

#include <algorithm>

class HealthComponent
{
private:
    int currentHealth = 0;
    int maxHealth = 0;

public:
    explicit HealthComponent(int maximum = 0)
        : currentHealth(std::max(0, maximum)), maxHealth(std::max(0, maximum))
    {
    }

    void TakeDamage(int points)
    {
        if (points > 0)
        {
            currentHealth = std::max(0, currentHealth - points);
        }
    }

    void Heal(int points)
    {
        if (points > 0)
        {
            currentHealth = points >= maxHealth - currentHealth ? maxHealth : currentHealth + points;
        }
    }

    bool IsDead() const
    {
        return currentHealth == 0;
    }

    int Current() const
    {
        return currentHealth;
    }

    int Maximum() const
    {
        return maxHealth;
    }
};