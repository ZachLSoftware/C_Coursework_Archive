#ifndef GAME_H_
#define GAME_H_

#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <vector>
using std::string;

class Player; // forward declaration so that Object can refer to Player

// ------------------ Object and its subclasses -------------------

class Object
{
	friend class Player;

public:
	// Default constructor, just to make this release version compilable.
	// If your implementation is correct this should be removed
	// Object();
	// this ^^ above default constructor should not be needed. you should keep it commented out

	// Constructor, specifying the name and the value of an object.
	Object(string name, int value);

	// Destructor
	virtual ~Object();

	// Return the name of the object.
	string getName() const;

	// Return the value of the object.
	int getValue() const;

	// Return a string giving details of the object, as specified in
	// the assignment webpage.
	string print() const;

	string getType() const
	{
		return type_;
	};
	virtual void applyEffect() = 0;

	

	Player *owner_;
	std::string name;
	string type;
	std::string type_;
	int value;
	std::vector<std::unique_ptr<Object>> items_;
private:

	// Use this object.
	// This function should not be public; it only makes sense to be
	// called from within a Player object (the player who owns it)
	virtual void use();
	// {
	// 	this->applyEffect();
	// }

	// virtual void applyEffect() = 0;

protected:
	// You probably want to have something like this.
	// For technical reason it may be easier to just use a raw pointer
	// Player* owner_;
	// std::string name;
	// string type;
	// std::string type_;
	// // int value;

	// TODO: add any other protected/private member variables

	// Overloaded output stream redirection operator, printing the object to the
	// given output stream
	friend std::ostream &operator<<(std::ostream &os, const Object &o);

	// You can "friend" other classes
};

class Food : public Object
{
	friend class Player;

public:
	// Constructor
	Food(string name, int value);

	// Destructor
	~Food();

	std::string getType() const
	{
		return "Food";
	}

	// Add any member functions if needed
	void use();

	

private:
	//   Player* owner;
	// Add any member variables if needed
	void applyEffect() override

	{
		
	// int max_increase = 100 - this->owner_->stamina_;
    // this->owner_->stamina_ += std::min(value_, max_increase);
    
	}
};

class Weapon : public Object
{
	friend class Player;

public:
	// Constructor
	Weapon(string name, int value);

	// Destructor
	~Weapon();

	std::string getType() const
	{
		return "Weapon";
	}

	// Add any member functions if needed
	void use();


private:
	//  Player* owner;
	// string weapon_;

	// Add any member variables if needed
	void applyEffect() override
	{
		// Apply the effect of the weapon to the player who owns it
		// this->owner->weaponInUse = this;
	}
};

class Armour : public Object
{
	friend class Player;

public:
	// Constructor
	Armour(string name, int value);

	// Destructor
	~Armour();

	std::string getType() const
	{
		return "Armour";
	}

	// Add any member functions if needed

	virtual void use(Player &player);

private:
	// Add any member variables if needed
	void applyEffect() override
	{
		// Apply the effect of the armour to the player who owns it
	}
};

// ----------------- Player and its subclasses --------------------

class Player
{
public:
	// Default constructor, just to make this release version compilable.
	// If your implementation is correct this should be removed
	Player();

	// Constructor, specifying the name of the player
	// Set the health and stamina both to 100
	Player(string name);

	// Destructor
	~Player();

	// Return the name of the player
	string getName() const;

	// Returns the type of the player
	virtual std::string getType() const = 0;

	// Return the current health of the player
	int getHealth() const;

	// Return the current stamina of the player
	int getStamina() const;

	// checks if player is dead
	bool isDead() const;

	// Add the object pointed to by the unique pointer to the
	// inventory of objects this player carries.
	// As this is a unique pointer, it has to be "moved" into this
	// function using std::move().
	void pickup(std::unique_ptr<Object> obj);

	// Return a string listing all objects this player owns, in
	// the format specified in the assignment page
	string getInventory() const;

	// Return a string that contains all details of a player, as
	// specified in the assignment page
	string print() const;

	// Use the object with the given name. If the player does not
	// own any object of this name, return false and do nothing,
	// otherwise return true.
	bool use(string name);
	std::vector<std::unique_ptr<Object>>& getItems() { 
		return items_; }

	string name_;
	int health_;
	int stamina_;
	// Weapon* weapon_in_use_;
	// Armour* armour_in_use_;
	std::vector<Weapon *> weapon_in_use_;
	std::vector<Armour *> armour_in_use_;
	std::vector<std::unique_ptr<Object>> items_;
	std::vector<std::unique_ptr<Object>> inventory;

private:
	// TODO: add any protected or private member variables
	// std::
	// string name_;
	// int health_;
	// int stamina_;
	// Weapon* weapon_in_use_;
	// Armour* armour_in_use_;
	// std::vector<std::unique_ptr<Object>> items_;
	// std::vector<std::unique_ptr<Object>> inventory;
	//     std::unique_ptr<Weapon> weapon;
	//   std::vector<std::unique_ptr<Armour>> armours;

	// Overloaded stream redirection operator, that prints the contents of
	// the player to the given output stream
	friend std::ostream &operator<<(std::ostream &os, const Player &p);

	// You can "friend" other classes
	friend class Fighter;
	friend class Healer;
};

class Fighter : public Player
{
	friend class Player;

public:
	// Constructor
	Fighter(string name);

	// Destructor
	~Fighter();

	// Attack the specified player, following the rules in the
	// assignment page
	bool attack(Player &other);

	// Add any other functions if needed

	// Returns Fighter Type
	std::string getType() const override
	{
		return "Fighter";
	}

	int attack_strength;
	int defend_strength;

private:
	// add any member variables if needed
};

class Healer : public Player
{
	friend class Player;

public:
	// Constructor
	Healer(string name);

	// Destructor
	~Healer();

	// Heal the specified player, following the rules in the
	// assignment page
	bool heal(Player &other);

	// Returns Healer Type
	std::string getType() const override
	{
		return "Healer";
	}

private:
	// add any member variables if needed
};

#endif /* GAME_H_ */