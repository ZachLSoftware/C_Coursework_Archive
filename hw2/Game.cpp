#include "Game.h"
#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>

// -------------------- Object class ----------------------

// Object::Object() {} // REMOVE ME

Object::Object(string name, int value)
{

    if (value < 0)
    {
        throw std::invalid_argument("value must not be negative");
    }
    this->name = name;
    this->value = value;
    this->owner_ = nullptr;
}

Object::~Object() {}

string Object::getName() const
{

    return name;
}

int Object::getValue() const
{

    return value;
}

void Object::use(){
}

string Object::print() const
{

    return type + ", name: " + name + ", value: " + std::to_string(value);
}



std::ostream &operator<<(std::ostream &os, const Object &o)
{

    os << o.print();
    return os;
}

// ------------------- Food class ----------------------

Food::Food(string name, int value) : Object(name, value)
{
    this->type = "Food";
}

Food::~Food() {}

void Food :: use(){
     // Find the food item with the given name in the list of items
    for (auto it = this->owner_->getItems().begin(); it != this->owner_->getItems().end(); ++it)
  {
    std::unique_ptr<Object> &item = *it;
    if (item->getName() == name)
    {
        int max_increase = 100 - this->owner_->stamina_;
            this->owner_->stamina_ += std::min(value, max_increase);
            
            this->owner_->getItems().erase(it);
            return;
   
      this->owner_->getItems().erase(it);
      return;
    }
  }
}



// ------------------- Weapon class ----------------------

Weapon::Weapon(string name, int value) : Object(name, value)
{
    this->type = "Weapon";
}

Weapon::~Weapon() {}



void Weapon::use(){

    auto it = std::find(owner_->weapon_in_use_.begin(), owner_->weapon_in_use_.end(), this);
    if (it != owner_->weapon_in_use_.end())
    {
        owner_->weapon_in_use_.erase(it);
    }

    // Add this weapon to the player's inventory
    owner_->weapon_in_use_.push_back(this);
    
}
// ------------------- Armour class ----------------------

Armour::Armour(string name, int value) : Object(name, value)
{
    this->type = "Armour";
}

Armour::~Armour() {}

void Armour::use(Player &player)
{
    
    player.armour_in_use_.clear();
    player.armour_in_use_.push_back(this);


}

// ------------------- Player class ----------------------

//   Player::Player() {} // REMOVE ME

Player::Player(string name) : items_(std::vector<std::unique_ptr<Object>>())
{
    this->name_ = name;
    this->health_ = 100;
    this->stamina_ = 100;
}

Player::~Player() {}

string Player::getName() const
{
    // IMPLEMENT ME

    return this->name_;
}

int Player::getHealth() const
{
    // IMPLEMENT ME

    return health_;
}

int Player::getStamina() const
{
    // IMPLEMENT ME

    return stamina_;
}


void Player::pickup(std::unique_ptr<Object> o)
{
    o->owner_ = this;

    if (health_ == 0)
    {
        throw std::logic_error("Player is already dead.");
    }

    items_.push_back(std::move(o));
}

string Player::getInventory() const
{

    string inventory = "List of items:";

    // if the inventory is empty, return "List of items: none"
    if (items_.empty())
    {
        return "List of items: none";
    }

    // if the inventory is not empty, return "List of items: \n" + the name of the object
    for (const auto &object : items_)
    {
        inventory += "\n " + object->print();
    }

    return inventory;
}
string Player::print() const
{

    std::stringstream output;
    output << "Name: " << name_ << "\n";
    output << "Type: " << getType() << "\n";
    output << "Health: " << health_ << "\n";
    output << "Stamina: " << stamina_ << "\n";
    output << getInventory();
    if (getType() == "Fighter")
    {
        output << std::endl
               << "Weapon in use: ";

        if (weapon_in_use_.size() > 0)
        {
            for (auto &weapon : weapon_in_use_)
            {
                output << "\n"
                       << weapon->getType() << ", name: " << weapon->getName() << ", value: " << weapon->getValue();
            }
        }
        else
        {
            output << "none";
        }
    }
    else
    {
    }

    output << "\nArmour in use: ";

    if (armour_in_use_.size() == 0)
    {
        output << "none";
    }
    else
    {
        for (auto &armour : armour_in_use_)
        {
            output << "\n " << armour->getType() << ", name: " << armour->getName() << ", value: " << armour->getValue();


        output << "\n";
        std::string output_str = output.str();
        return output_str;
    }
    return output.str();
}
}
bool Player::use(string name)
{
    // If the player is dead, throw a logic_error exception
    if (health_ == 0)
    {
        throw std::logic_error("Player is already dead.");
    }

    // Iterate through the inventory
    for (auto &item : items_)
    {
        // If the item has the correct name, use it and return true
        if (item->getName() == name)
        {
            item->use();
            return true;
        }
    }
    //    If no item with the correct name was found, return false
    return false;
}

std::ostream &operator<<(std::ostream &os, const Player &p)
{
    // IMPLEMENT ME
    os << p.print();
    return os;
}

// ------------------- Fighter class ----------------------

Fighter::Fighter(string name) : Player(name)
{
}

Fighter::~Fighter() {}

bool Fighter::attack(Player &other)
{
   
    if (this->health_ <= 0 || other.health_ <= 0)
    {
        std::string message = "Stop! " + other.name_ + " is already dead.";
        throw std::logic_error(message);
    }


    if (stamina_ < 10)
        return false;
    stamina_ -= 10;
   
    if (other.health_ <= 0)
        return false;
    int attack_strength = 10;
    for (const auto &weapon : weapon_in_use_)
    {
        attack_strength += weapon->value;
    }
   
    int defending_strength = 0;
    for (int i = 0; i < other.armour_in_use_.size(); i++)
    {
        Armour *armour = other.armour_in_use_[i];
        defending_strength += armour->value;
    }
    if (attack_strength > defending_strength)
    {
        other.health_ -= attack_strength - defending_strength;
        if (other.health_ <= 0)
            other.health_ = 0;
        return true;
    }
    return false;
}

// ------------------- Healer class ----------------------

Healer::Healer(std::string name) : Player(name)
{
}

Healer::~Healer() {}

bool Healer::heal(Player &other)
{

    if (other.health_ == 0 || health_ == 0)
    {
        throw std::logic_error("Stop! " + other.name_ + " is already dead.");
    }
    // If the healer does not have enough stamina, return false
    if (stamina_ < 10)
    {
        return false;
    }
    // Reduce the healer's stamina by 10 and increase the other player's health by 20
    stamina_ -= 10;
    other.health_ += 20;
    // If the other player's health exceeds 100, set it to 100
    if (other.health_ > 100)
    {
        other.health_ = 100;
    }
    return true;
}

