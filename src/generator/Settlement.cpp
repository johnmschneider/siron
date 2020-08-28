#include <unordered_map>
#include "Settlement.h"
#include "SettlementPlace.h"
#include "..\irand.h"
#include "..\Main.h"

using namespace generator;
using namespace semanticNetwork;
using main_::strWhitespaceSplit;

using std::vector;
using std::string;
using std::unordered_map;

Settlement::Settlement(SemanticNetwork* network, Node* parentPlace) : Generator(network), parentPlace(parentPlace) {
	
}

void Settlement::generate() {
	SemanticNetwork* network = getNetwork();
	Node* settlement = generateName(network);

	network->addObjectLink(settlement->getData(), "is_a", "settlement");
	settlement->setSimpleField("located_in", parentPlace->getData());

	generatePlaceSize(network, settlement);
	generateSettlementSize(network, settlement);
}

Node* Settlement::generateName(SemanticNetwork* network) {
	Node*					generator = network->getObjectNode("city_generator");
	Node::FieldGetter::Info	rawNameFrags = generator->getField("name_fragments");
	vector<string>			nameFrags = strWhitespaceSplit(rawNameFrags.simpleField, true);

	static int				maxSyllablesPerWord = 3;
	string					cityName = nameFrags[irand(nameFrags.size() - 1)];
	int						syllablesThisWord = 1;
	int						wordStop = irand(2, maxSyllablesPerWord);
	int						totalSyllables = 2 + irand((maxSyllablesPerWord - 1) * 2);
	int						totalWords = 1;

	for (int i = 1; i < totalSyllables; i++) {
		if (syllablesThisWord % wordStop == 0 && totalWords < 2) {
			cityName += "_" + nameFrags[irand(nameFrags.size() - 1)];

			syllablesThisWord = 0;
			wordStop = irand(2, maxSyllablesPerWord);
			totalWords++;
		}
		else {
			cityName += nameFrags[irand(nameFrags.size() - 1)];
		}

		syllablesThisWord++;
	}

	network->addObjectNode(cityName);
	return network->getObjectNode(cityName);
}

void Settlement::generatePlaceSize(SemanticNetwork* network, Node* settlementNode) {
	Node* placeSizesNode = network->getObjectNode("place_size");
	string rawPlaceSizes = placeSizesNode->getField("sizes").simpleField;
	vector<string> placeSizes = strWhitespaceSplit(rawPlaceSizes, true);

	settlementNode->setSimpleField("place_size", placeSizes[irand(placeSizes.size() - 1)]);
}

void Settlement::generateSettlementSize(SemanticNetwork* network, Node* settlementNode) {
	Node* placeSizesNode = network->getObjectNode("settlement_size");
	string rawPlaceSizes = placeSizesNode->getField("sizes").simpleField;
	vector<string> placeSizes = strWhitespaceSplit(rawPlaceSizes, true);

	settlementNode->setSimpleField("settlement_size", placeSizes[irand(placeSizes.size() - 1)]);
}