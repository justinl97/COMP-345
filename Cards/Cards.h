#pragma once
#include <string>

using std::string;

class Card {
    public:
        Card();
        void play();
};

// --- children of the class Card, each with their respective constructors ---
class Bomb : public Card {
    public:
        Bomb();
};

class Reinforcement : public Card {
    public:
        Reinforcement();
};

class Blockade : public Card {
    public:
        Blockade();
};

class Airlift : public Card {
    public:
        Airlift();
};

class Diplomacy : public Card {
    public:
        Diplomacy();
};
// -------------------------------

class Deck {
    public:
        Deck();
        Card getCard();
        void setSize(int sizeDeck);
        int getSize();
        Card* draw();

    private:
        int sizeDeck;
        Card* cards[];
};

class Hand {
    public:
        Hand();
        void hand(Card* ptrCard);
        void print() const;
        void setSize(int sizeHand);
        int getSize();
    
    private:
        int sizeHand;
        Card* cardsInHand[];
};