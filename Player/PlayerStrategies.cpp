#include "PlayerStrategies.h" // Include header file

Player* PlayerStrategy::getPlayer()
{
	return p;
}

void PlayerStrategy::setPlayer(Player* player)
{
	p = player;
}

PlayerStrategy::PlayerStrategy(Player* player) : p(player)
{
}

PlayerStrategy::PlayerStrategy(const PlayerStrategy& ps) : p(ps.p)
{
}

PlayerStrategy& PlayerStrategy::operator=(const PlayerStrategy& ps)
{
	p = ps.p;
	return *this;
}

ostream& operator<<(ostream& out, const PlayerStrategy&)
{
	return out << "Abstract Player Strategy";
}


// FROM HERE ONWARDS, EVERY METHOD THAT BELONGS TO HUMAN FOR EXAMPLE, SHOULD BE IMPLEMENTED FOR EACH STRATEGY
ostream& operator<<(ostream& out, const HumanPlayerStrategy&)
{
	return out << "Human Player Strategy";
}

HumanPlayerStrategy::HumanPlayerStrategy(Player* player) : PlayerStrategy(player)
{
}

HumanPlayerStrategy::HumanPlayerStrategy(const HumanPlayerStrategy& hps) : PlayerStrategy(hps)
{
}

HumanPlayerStrategy& HumanPlayerStrategy::operator=(const HumanPlayerStrategy& hps)
{
	p = hps.p;
	return *this;
}

void HumanPlayerStrategy::issueOrder()
{

	// Aliases
	GameEngine* game = p->getGameEngine();
	Hand* hand = p->getHand();
	Deck* deck = game->getDeck();
	OrdersList* orders = p->getOrdersList();

	// Intro message for each player
	cout << "\nIssuing orders for player " << p->getName() << ":" << endl;
	cout << "/*-------------------------------------------------------------------*/" << endl;

	cout << "#--- Deploying Phase ---#" << endl;

	// List of territories to defend
	cout << "\nTerritories to defend: (Index : Name)" << endl;
	for (auto it : toDefend()) {
		cout << it->getIndex() << " : " + it->getName() << " , Armies: " << it->getNumberOfArmies() << endl;
	}
	cout << endl;

	// While the player still has armies to deploy (reinforcement pool is not empty)
	while (p->getReinforcementPool() > 0) {

		// Announce how big the reinforcement pool is
		cout << "Player " << p->getName() << "'s number of armies left in the reinforcement pool: " << p->getReinforcementPool() << endl << endl;

		// Choose index of territory to defend
		cout << "Input the index of the defending territory you want to deploy armies to: ";
		int tIndex;
		cin >> tIndex;
		cout << endl;

		// If player owns territory (found by index)
		if (p->ownsTerritory(tIndex)) {

			// Input number of armies to deploy
			cout << "Input how many armies you wish to deploy to that territory: ";
			int deployNo;
			cin >> deployNo;
			cout << endl;

			if (deployNo <= p->getReinforcementPool()) {

				// Negative input
				if (deployNo <= 0) {
					cout << "Cannot input a negative number!" << endl;
				}

				// Non-negative input that is less or equal to number of armies available
				else {
					orders->addOrders(new Deploy(p, deployNo, game->getMap()->getTerritoryByIndex(tIndex), game));
					p->setReinforcementPool(p->getReinforcementPool() - deployNo);
					cout << deployNo << " armies will been deployed!" << endl;
				}
			}
			else {
				cout << "Insufficient number of armies available in reinforcement pool, you can deploy at most " << p->getReinforcementPool() << " armies!" << endl;
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

		// Option 1 (DEFENSE)
		if (ans == 1) {

			// List of territories to defend
			cout << "\nTerritories to defend: (Index : Name)" << endl;
			for (auto it : toDefend()) {
				cout << it->getIndex() << " : " + it->getName() << " , Armies: " << it->getNumberOfArmies() << endl;
			}
			cout << endl;

			// Accept input for source territory's index (where armies originate)
			cout << "Input the souce territory's index: ";
			int sourceIndex;
			cin >> sourceIndex;
			cout << endl;

			// Check if player owns this territory or if it doesn't exist
			if (p->ownsTerritory(sourceIndex)) {

				// Accept input for target territory's index (where armies go to)
				cout << "Input the target territory's index: ";
				int targetIndex;
				cin >> targetIndex;
				cout << endl;

				// Check if player owns this territory or if it doesn't exist
				if (p->ownsTerritory(targetIndex)) {

					// Accept input for number of armies to move
					cout << "Finally, input the number of armies you wish to move: ";
					int army;
					cin >> army;
					cout << endl;

					// Convert the territory index's to territory pointers
					Territory* source = game->getMap()->getTerritoryByIndex(sourceIndex);
					Territory* target = game->getMap()->getTerritoryByIndex(targetIndex);

					// Check if input is valid (source territory has enough armies)
					if (army <= source->getNumberOfArmies()) {
						cout << "Advancing from " << source->getName() << " to "
							<< target->getName() << " " << army << " armies!" << endl;
						orders->addOrders(new Advance(p, army, source, target, game));
					}

					// Input number of armies to move is bigger than number of armies in source territory
					else {
						cout << "Wrong input: You do not have this many armies in the source territory!" << endl;
					}
				}

				// Player does not own this territory or it does not exist
				else {
					cout << "Wrong input: You do not own this territory or it does not exist!" << endl;
				}
			}

			// Player does not own this territory or it does not exist
			else {
				cout << "Wrong input: You do not own this territory or it does not exist!" << endl;
			}
		}

		// Option 2 (ATTACK)
		else if (ans == 2) {

			// List of territories to defend
			cout << "\nTerritories to defend: (Index : Name)" << endl;
			for (auto it : toDefend()) {
				cout << it->getIndex() << " : " + it->getName() << " , Armies: " << it->getNumberOfArmies() << endl;
			}
			cout << endl;

			// List of territories to attack
			cout << "\nTerritories to attack: (Index : Name)" << endl;
			for (auto it : toAttack()) {
				cout << it->getIndex() << " : " + it->getName() << " , Armies: " << it->getNumberOfArmies() << endl;
			}
			cout << endl;

			// Accept input for source territory's index (where armies originate)
			cout << "Input the souce territory's index: ";
			int sourceIndex;
			cin >> sourceIndex;
			cout << endl;

			// Check if player owns this territory or if it doesn't exist
			if (p->ownsTerritory(sourceIndex)) {


				// Accept input for target territory's index (where armies go to)
				cout << "Input the target territory's index: ";
				int targetIndex;
				cin >> targetIndex;
				cout << endl;

				// Convert source and targert territories from index to a territory pointer
				Territory* source = game->getMap()->getTerritoryByIndex(sourceIndex);
				Territory* target = game->getMap()->getTerritoryByIndex(targetIndex);

				// Check if target is adjacent to any of the player's owned territories
				bool targetCanBeAttacked = false;
				for (auto it : toAttack()) {
					if (it == target) {
						targetCanBeAttacked = true;
					}
				}

				// If the target territory can be attacked
				if (targetCanBeAttacked) {

					// Accept input for number of armies to move
					cout << "Finally, input the number of armies you wish to move: ";
					int army;
					cin >> army;
					cout << endl;

					// Check if input is valid (source territory has enough armies)
					if (army <= source->getNumberOfArmies()) {
						cout << "Advancing from " << source->getName() << " to "
							<< target->getName() << " " << army << " armies!" << endl;
						//Takes care of what happens if diplomacy was used last turn
						Advance* advance = new Advance(p, army, source, target, game);
						for (auto it : p->cannotAttack) {
							if (it == target->getOwner()) {
								advance->cannotBeAttacked = true;
							}
						}
						orders->addOrders(advance);
					}

					// Input number of armies to move is bigger than number of armies in source territory
					else {
						cout << "Wrong input: You do not have this many armies in the source territory!" << endl;
					}
				}

				// Target territory cannot be attacked
				else {
					cout << "Wrong input: The target territory is not adjacent to one you own or it does not exist!" << endl;
				}
			}

			// Player does not own this territory or it does not exist
			else {
				cout << "Wrong input: You do not own this territory or it does not exist!" << endl;
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

	// All advance orders have been issued at this point!

	//NOW CLEARING CANNOTATTACK VECTOR For Negotiate order
	p->cannotAttack.clear();

	// Now playing a card, Player plays one card per turn

	cout << "\n#--- Card Playing Phase ---#" << endl << endl;

	// Check if player has any cards in hand
	if (hand->getSize() > 0) {

		// Print the player's hand
		cout << "Printing all the cards in player's hand" << endl;
		if (hand->getSize() > 0) {
			for (int i = 0; i < hand->getSize(); i++) {
				cout << "Card index : " << i << ", Type: " << hand->getCardInHand(i)->getCardTypeName() << endl;
			}
		}

		// Ask input for the index of the card to play
		cout << "Select the index of the card you want to use : ";

		int cardIndex;
		cin >> cardIndex;
		cout << endl;

		// If input index is wrong, ask for input again
		while (cardIndex < 0 || cardIndex >= hand->getSize()) {
			cout << "Wrong index selected, please input another index : ";
			cin >> cardIndex;
			cout << endl;
		}

		// Convert input index to a card pointer
		Card* card = hand->getCardInHand(cardIndex);

		string cardName = card->getCardTypeName();

		// Cases for each type of card to be played and its required input
		if (cardName == "Bomb") {
			cout << "Bomb card selected:" << endl;

			cout << "Which territory should be bombed? Input the index from the list: " << endl;

			// List of territories to attack
			cout << "\nTerritories to attack: (Index : Name)" << endl;
			for (auto it : toAttack()) {
				cout << it->getIndex() << " : " + it->getName() << " , Armies: " << it->getNumberOfArmies() << endl;
			}
			cout << endl;

			// Input enemy territory index
			int territoryIndex;
			cin >> territoryIndex;
			cout << endl;

			// Convert territory index to territory pointer
			Territory* enemyT = game->getMap()->getTerritoryByIndex(territoryIndex);

			card->play(cardIndex, 0, p, nullptr, nullptr, enemyT, game);

			cout << "Bomb order will be issued!";
		}
		else if (cardName == "Reinforcement") {
			cout << "Reinforcement card selected --> Will be played immediately below: " << endl;
			card->play(cardIndex, 0, p, nullptr, nullptr, nullptr, game);
		}
		else if (cardName == "Blockade") {
			cout << "Blockade card selected:" << endl;

			cout << "Which territory should receive a blockade? Input the index from the list: " << endl;

			// List of territories to defend
			cout << "\nTerritories to defend: (Index : Name)" << endl;
			for (auto it : toDefend()) {
				cout << it->getIndex() << " : " + it->getName() << " , Armies: " << it->getNumberOfArmies() << endl;
			}
			cout << endl;

			// Input enemy territory index
			int territoryIndex;
			cin >> territoryIndex;
			cout << endl;

			// Convert territory index to territory pointer
			Territory* ownT = game->getMap()->getTerritoryByIndex(territoryIndex);

			card->play(cardIndex, 0, p, nullptr, nullptr, ownT, game);

			cout << "Blockade order will be issued!";
		}
		else if (cardName == "Airlift") {
			cout << "Airlift card selected:" << endl;

			cout << "Where should the airlift start? Input the index from the list: " << endl;

			// List of territories to defend
			cout << "\nTerritories to defend: (Index : Name)" << endl;
			for (auto it : toDefend()) {
				cout << it->getIndex() << " : " + it->getName() << " , Armies: " << it->getNumberOfArmies() << endl;
			}
			cout << endl;

			// Input enemy territory index
			int territoryIndex;
			cin >> territoryIndex;
			cout << endl;

			// Convert territory index to territory pointer
			Territory* ownT = game->getMap()->getTerritoryByIndex(territoryIndex);

			cout << "Where should the airlift head to? Input the index from the list: " << endl;

			// List of territories to defend
			cout << "\nTerritories to defend: (Index : Name)" << endl;
			for (auto it : toDefend()) {
				cout << it->getIndex() << " : " + it->getName() << " , Armies: " << it->getNumberOfArmies() << endl;
			}
			cout << endl;

			// Input enemy territory index
			int territoryIndex2;
			cin >> territoryIndex2;
			cout << "Input no of armies: " << endl;
			int army; cin >> army;
			cout << endl;

			// Convert territory index to territory pointer
			Territory* otherOwnT = game->getMap()->getTerritoryByIndex(territoryIndex2);


			card->play(cardIndex, army, p, nullptr, ownT, otherOwnT, game);

			cout << "Airlift order will be issued!";
		}
		else if (cardName == "Diplomacy") {
			cout << "Diplomacy card selected:" << endl;

			cout << "Who should Diplomacy be used on? Input the index from the list: " << endl;

			// List of players in the game
			cout << "\nList of players in the game: (Index : Name)" << endl;
			for (int i = 0; i < game->players.size(); i++) {
				cout << i << " : " + game->players.at(i)->getName() << endl;
			}
			cout << endl;

			// Input other player's index
			int playerIndex;
			cin >> playerIndex;
			cout << endl;

			Player* otherP = game->players.at(playerIndex);

			card->play(cardIndex, 0, p, otherP, nullptr, nullptr, game);

			cout << "Diplomacy order will be issued!";
		}
	}

	// Player has no cards in their hand
	else {
		cout << "This player has no cards in their hand, skipping the card playing phase!" << endl;
	}

	cout << "\n#--- Card Playing Phase OVER ---#" << endl;
	cout << "/*-------------------------------------------------------------------*/" << endl;
}

vector<Territory*> HumanPlayerStrategy::toAttack()
{
	vector<Territory*> attackableTerritories;

	for (Territory* t : p->getOwnedTerritories()) {
		for (Territory* a : t->getAdjacentTerritories()) {
			// Excluding owned territories
			if (p->ownsTerritory(a)) {
				// Add territory to attack list
				attackableTerritories.push_back(a);
			}
		}
	}

	// Remove duplicates
	for (Territory* t : attackableTerritories) {
		attackableTerritories.erase(unique(attackableTerritories.begin(), attackableTerritories.end()), attackableTerritories.end());
	}
	return attackableTerritories;
}

vector<Territory*> HumanPlayerStrategy::toDefend() {
	return p->getOwnedTerritories();
}

