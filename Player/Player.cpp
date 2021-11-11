#include "Player.h" // Including the appropriate header file

// Default constructor requires a hand
Player::Player(Hand* hand) : name("Unnamed"), hand(hand), reinforcementPool(0) {
    this->ordersList = new OrdersList(this,*new vector<Orders*>);
}

// Constructor with player name and hand
//Player::Player(string name, Hand* hand) : name(name), hand(hand), reinforcementPool(0) {
//    this->ordersList = new OrdersList(this,*new vector<Orders*>);
//}


// Copy constructor
Player::Player(const Player& player)
{
	name = player.name;
	this->hand = new Hand(*(player.hand));
	territoriesOwned = player.territoriesOwned;
	ordersList = new OrdersList(*(player.ordersList));
	reinforcementPool = player.reinforcementPool;
    gameEngine = player.gameEngine; // Cannot create new map
}

// Destructor
Player::~Player()
{
	delete hand; // Delete pointer to hand
    delete getOrdersList(); //Delete allocated ordersList in constructor
	//delete ordersList; // Delete pointer to ordersList
	this->ordersList = nullptr;
	// All cleared vectors need their content deleted, depending on how they are initialized (see driver)
	territoriesOwned.clear();
	gameEngine = nullptr;
}

// Assignment operator
Player& Player::operator=(const Player& player)
{
	name = player.name;
	this->hand = new Hand(*(player.hand)); // Deep copy of hand is made by calling new
	territoriesOwned = player.territoriesOwned;
	ordersList = new OrdersList(*(player.ordersList));
	reinforcementPool = player.reinforcementPool;
    gameEngine = player.gameEngine;
	return *this;
}

// Add territory to own
void Player::addOwnedTerritory(Territory* territory) {
	territoriesOwned.push_back(territory);
    territory->setOwner(this);
}

// Return a list of owned territories
vector<Territory*> Player::toDefend() // Returns a list of territories that are to be defended
{
	return territoriesOwned;
}

// Return a list of territories to attack
vector<Territory*> Player::toAttack() // Returns a list of territories that are to be attacked
{
	vector<Territory*> adjacentTerritories;
	for (Territory* t : territoriesOwned) {
		for (Territory* a : t->getAdjacentTerritories()) {
			adjacentTerritories.push_back(a);
			// Doesn't take care of copies/repeats in territories
		}
	}
	return adjacentTerritories;
}

// Create a new order and add to order list
void Player::issueOrder() // Creates an Order object and puts it in the player�s list of orders
{
	
	// Intro message for each player
	cout << "\nIssuing orders for player " << getName() << ":" << endl;
	cout << "/*-------------------------------------------------------------------*/" << endl;

	/*
	The player issues deploy orders on its own territories that are in the list returned by toDefend(). As long
	as the player has armies still to deploy (see startup phase and reinforcement phase), it will issue a deploy
	order and no other order. Once it has deployed all its available armies, it can proceed with other kinds of
	orders. 
	*/

	cout << "#--- Deploying Phase ---#" << endl;

	// List of territories to defend
	cout << "\nTerritories to defend: (Index : Name)" << endl;
	for (auto it : toDefend()) {
		cout << it->getIndex() << " : " + it->getName() << endl;
	}
	cout << endl;
	
	// While the player still has armies to deploy (reinforcement pool is not empty)
	while (getReinforcementPool() > 0) {

		// Announce how big the reinforcement pool is
		cout << "Player " << getName() << "'s number of armies left in the reinforcement pool: " << getReinforcementPool() << endl << endl;

		// Choose index of territory to defend
		cout << "Input the index of the defending territory you want to deploy armies to: ";
		int tIndex;
		cin >> tIndex;
		cout << endl;

		// If player owns territory (found by index)
		if (ownsTerritory(tIndex)) {

			// Input number of armies to deploy
			cout << "Input how many armies you wish to deploy to that territory: ";
			int deployNo;
			cin >> deployNo;
			cout << endl;

			if (deployNo <= getReinforcementPool()) {
				
				// Negative input
				if (deployNo <= 0) {
					cout << "Cannot input a negative number!" << endl; 
				}

				// Non-negative input that is less or equal to number of armies available
				else {
					ordersList->addOrders(new Deploy(this, deployNo, gameEngine->getMap()->getTerritoryByIndex(tIndex), gameEngine));
					setReinforcementPool(getReinforcementPool() - deployNo);
					cout << deployNo << " armies have been deployed!" << endl;
				}
			}
			else {
				cout << "Insufficient number of armies available in reinforcement pool, you can deploy at most " << getReinforcementPool() << " armies!" << endl;
			}
		}

		// If player does not own territory or it does not exist
		else {
			cout << "Wrong input: You do not own this territory or it does not exist" << endl;
		}
	}

	cout << "\n#--- Deploying Phase OVER ---#" << endl;
	cout << "/*-------------------------------------------------------------------*/" << endl;

	// All deploy orders have been issued at this point!

	// Now issuing advance orders

	cout << "\n#--- Advancing Phase ---#" << endl;

	// Advance orders are optional, ask user whether they want to issue any
	cout << "Do you wish to issue any advance orders?" << endl
		<< "Reply with \"y\" if you wish to do so, any other input will be assumed to be a no : ";

	string answer;
	cin >> answer;
	cout << endl;

	// "y" as input
	while (answer == "y") {

		// Two options to choose from
		cout << "Do you wish to :" << endl;
		cout << "1. (DEFENSE) : Move armies from their own territories to defend another?" << endl;
		cout << "OR" << endl;
		cout << "2. (ATTACK) : Move armies from their own territories to a neighboring enemy territory to attack?" << endl;
		cout << "Reply with either number 1 or 2 : ";
		int ans;
		cin >> ans;
		cout << endl;

		// Option 1
		if (ans == 1) {

			// List of territories to defend
			cout << "\nTerritories to defend: (Index : Name)" << endl;
			for (auto it : toDefend()) {
				cout << it->getIndex() << " : " + it->getName() << endl;
			}
			cout << endl;

			// ?
			cout << "Input the territory index you want to advance FROM: ";
			int sourceIndex;
			cin >> sourceIndex;
			cout << endl;
			if (ownsTerritory(sourceIndex)) { // Index is being passed here, it should take in a territory pointer.
				cout << "Now input the territory index you want to advance TO: ";
				int targetIndex;
				cin >> targetIndex;
				cout << endl;
				if (ownsTerritory(targetIndex)) { // Index is being passed here, it should take in a territory pointer.
					cout << "Finally, input the number of armies you wish to move: ";
					int army;
					cin >> army;
					cout << endl;
					Territory* source = gameEngine->getMap()->getTerritoryByIndex(sourceIndex);
					Territory* target = gameEngine->getMap()->getTerritoryByIndex(targetIndex);
					if (army <= source->getNumberOfArmies()) {
						cout << "Adding advance order to ordersList" << endl;
						cout << "Advance from " << source->getName() << " to "
							<< target->getName() + " added successfully" << endl;
						ordersList->addOrders(new Advance(this, army, source, target, gameEngine)); // Order pointer is being passed when it should take in an order reference
					}
					else {
						cout << "Wrong input, try again(Not enough army in source territory)" << endl;
					}
				}
				else {
					cout << "Wrong input, try again(You do not own target territory)" << endl;
				}
			}
			else {
				cout << "Wrong input, try again(You do not own source territory)" << endl;
			}
		}

		// Option 2
		else if (ans == 2) {
			
			// List of territories to attack
			cout << "\nTerritories to attack: (Index : Name)" << endl;
			for (auto it : toAttack()) {
				cout << it->getIndex() << " : " + it->getName() << endl;
			}
			cout << endl;
			
			// ?
			cout << "Input the territory index you want to advance FROM: ";
			int sourceIndex;
			cin >> sourceIndex;
			cout << endl;
			if (ownsTerritory(sourceIndex)) { // Index is being passed here, it should take in a territory pointer.
				cout << "Now input the territory index you want to advance TO: ";
				int targetIndex;
				cin >> targetIndex;
				cout << endl;

				bool targetCanBeAttacked = false;

				Territory* target = gameEngine->getMap()->getTerritoryByIndex(targetIndex);

				for (auto it : toAttack()) { if (it == target) { targetCanBeAttacked = true; } }
				if (targetCanBeAttacked) {
					cout << "Finally, input the number of armies you wish to move: ";
					int army;
					cin >> army;
					cout << endl;
					Territory* source = gameEngine->getMap()->getTerritoryByIndex(sourceIndex);
					if (army <= source->getNumberOfArmies()) {
						cout << "Adding advance order to ordersList" << endl;
						cout << "Advance from " << source->getName() << " to "
							<< target->getName() + "added successfully" << endl;
						ordersList->addOrders(new Advance(this, army, source, target , gameEngine)); // Order pointer is being passed when it should take in an order reference
					}
					else {
						cout << "Wrong input, try again(Not enough army in source territory)" << endl;
					}
				}
				else {
					cout << "Wrong input, try again(Target territory not in toAttack() list)" << endl;
				}
			}
		}

		// Wrong input (not 1 or 2)
		else {
			cout << "Wrong input, try again (Input only 1 or 2)" << endl;
		}

		// Issue another advance order
		cout << "\nDo you wish to issue another advance order? " << endl;
		cout << "Reply with \"y\" if you wish to do so, any other input will be assumed to be a no : ";
		cin >> answer;
		cout << endl;
	}

	cout << "\n#--- Advancing Phase OVER ---#" << endl;
	cout << "/*-------------------------------------------------------------------*/" << endl;

	// All advance orders are done

	cout << "Now that all advance orders are dealt with, moving to card orders " << endl;

	cout << "Printing all the cards in player's hand" << endl;

	//Need to test if the cards are properly displayed

	cout << getHand(); //Printing number of cards in hand


	//Need to ask Iris about how index in hand works(Does it start with zero?)

	for (int i = 0; i < getHand()->getSize(); i++) {
		cout << "Card index : " << i << endl << getHand()->getCardInHand(i);
	}

	cout << "Select the index of the card you want to use : ";

	/////////////////////////////////////////
	//Need to rewrite Card::play() method
	int index; cin >> index; cout << endl;
    while(index<0 ||index > hand->getSize()){
        cout << "Wrong index selected, please input another index : ";
        cin >> index;cout<< endl;
    }
    Card* cardToBePlayed = getHand()->getCardInHand(index);
    if(cardToBePlayed->getCardTypeName() == "Bomb"){
        cout << "Bomb card selected, which territory index do you wish to bomb? Select from toAttack() list: " << endl;
        int territoryIndex; cin >> index; cout << endl;
        Territory* target = gameEngine->getMap()->getTerritoryByIndex(index);
        cout << "Playing bomb card" << endl;
        cardToBePlayed->play(getHand(),index,getGameEngine()->getDeck(),this, nullptr, this->ordersList, nullptr,target,getGameEngine());
        cout << "Bomb card successfully played";
    }
    // ^ Same thing can be done for every card type -- > Just something I thought up instead of changing the play method
    //---Abhay
}

bool Player::ownsTerritory(Territory* territory) {
	for (Territory* t : territoriesOwned) {
		if (t == territory) {return true;}
	}
	return false;
}

bool Player::ownsTerritory(int index) {
    for (Territory* t : territoriesOwned) {
        if (t->getIndex() == index) {return true;}
    }
    return false;
}

// Getters
string Player::getName() {return name;}
Hand* Player::getHand() {return hand;}
OrdersList* Player::getOrdersList() {return ordersList;}

int Player::getReinforcementPool()
{
	return reinforcementPool;
}

void Player::setReinforcementPool(int rP)
{
	reinforcementPool = rP;
}

void Player::setGameEngine(GameEngine* game)
{
	this->gameEngine = game;
}

GameEngine* Player::getGameEngine()
{
	return gameEngine;
}

// Stream insertion operator, returns player's name
ostream& operator<<(ostream& out, const Player& player) {
	return out << player.name;
}

void Player::removeOwnedTerritory(Territory * territory) {
    auto newOwnedTerritory = new vector<Territory*>();
    for(auto it: territoriesOwned){
        if(it == territory){}
        else{
            newOwnedTerritory->push_back(it);
        }
    }
    setOwnedTerritories(*newOwnedTerritory);

}

void Player::setOwnedTerritories(vector<Territory *> & newVector) {
    territoriesOwned = newVector;
}

Player::Player(string name, Hand * hand, GameEngine* gameEngine) {
    this->name = name;
    this->hand = hand;
    this->gameEngine = gameEngine;
	this->ordersList = new OrdersList(this, *new vector<Orders*>);
}
