#include <string>
#include <vector>
#include "SettlementPlace.h"
#include "..\Main.h"
#include "..\irand.h"

using std::string;
using std::vector;
using namespace generator;
using namespace semanticNetwork;
using main_::strWhitespaceSplit;

SettlementPlace::SettlementPlace(SemanticNetwork* network, Node* settlementLocatedIn) : Generator(network), 
	settlementLocatedIn(settlementLocatedIn) {

}

void SettlementPlace::generate() {
	SemanticNetwork* network = getNetwork();
	settlementPlace = getName(network);

	network->addObjectLink(settlementPlace->getData(), "is_a", "tavern");

	vector<Node*> placeAtmospheres = network->getObjectNodeChildren("place_atmosphere");
	settlementPlace->setNodeField("place_atmosphere", placeAtmospheres[irand(placeAtmospheres.size() - 1)]);

	settlementPlace->setSimpleField("located_in", settlementLocatedIn->getData());
	settlementPlace->setSimpleField("abstract", "false");
	settlementPlace->setSimpleField("bar.cupboard.stored_items", "white_wine red_wine beer");

	Node* bar;
	network->addObjectNode(settlementPlace->getData() + ".bar");
	bar = network->getObjectNode(settlementPlace->getData() + ".bar");
	network->addObjectLink(bar->getData(), "is_a", "bar");
	bar->setSimpleField("located_in", settlementPlace->getData());

	settlementPlace->setNodeField("tavern.bar", bar);
}

Node* SettlementPlace::getName(SemanticNetwork* network) {
	Node*		   generator   = network->getObjectNode("tavern_generator");
	vector<string> firstNames  = strWhitespaceSplit(generator->getField("name_start").simpleField, true);
	vector<string> middleNames = strWhitespaceSplit(generator->getField("name_middle").simpleField, true);
	vector<string> endNames    = strWhitespaceSplit(generator->getField("name_end").simpleField, true);
	string		   placeName   = firstNames[irand(firstNames.size() - 1)] + "_" + middleNames[irand(middleNames.size() - 1)] + "_" + endNames[irand(endNames.size() - 1)];

	network->addObjectNode(placeName);
	return network->getObjectNode(placeName);
}