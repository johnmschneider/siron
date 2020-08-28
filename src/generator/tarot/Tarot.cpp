#include "Tarot.h"

using namespace generator::tarot;
using namespace semanticNetwork;
using std::vector;

Tarot::Tarot(SemanticNetwork* network) : Generator(network) {
	generatedCards = node_vector();
}

void Tarot::setSpreadCount(int32_t numOfCards) {
	this->cardCount = numOfCards;
}

void Tarot::generate() {

	generatedCards.clear();
	
	for (int32_t i = 0; i < cardCount; i++) {
		Node* arcanaType;
		Node* card;
	}
}