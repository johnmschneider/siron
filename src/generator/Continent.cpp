#include <unordered_map>
#include "Continent.h"
#include "Country.h"
//#include "State.h"
#include "..\Main.h"
#include "..\frand.h"
#include "..\irand.h"

using namespace generator;
using namespace semanticNetwork;
using main_::strWhitespaceSplit;
using std::vector;
using std::unordered_map;
using std::string;

Continent::Continent(SemanticNetwork* network) : Generator(network) {

}

void Continent::generate() {
	SemanticNetwork* network = getNetwork();
	Node* continentNode = generateName(network);

	continentNode->addLink("is_a", network->getObjectNode("continent"));

	generatePlaceSize(network, continentNode);
	generateCountries(network, continentNode);
}

Node* Continent::generateName(SemanticNetwork* network) {
	Node*					generator = network->getObjectNode("continent_generator");
	Node::FieldGetter::Info	rawNameFrags = generator->getField("name_fragments");
	vector<string>			nameFrags = strWhitespaceSplit(rawNameFrags.simpleField, true);

	static int				maxSyllablesPerWord = 3;
	string					continentName = nameFrags[irand(nameFrags.size() - 1)];
	int						syllablesThisWord = 1;
	int						wordStop = irand(2, maxSyllablesPerWord);
	int						totalSyllables = 2 + irand(maxSyllablesPerWord - 2);
	int						totalWords = 1;

	for (int i = 1; i < totalSyllables; i++) {
		if (syllablesThisWord % wordStop == 0 && totalWords < 2) {
			continentName += "_" + nameFrags[irand(nameFrags.size() - 1)];

			syllablesThisWord = 0;
			wordStop = irand(2, maxSyllablesPerWord);
			totalWords++;
		}
		else {
			continentName += nameFrags[irand(nameFrags.size() - 1)];
		}

		syllablesThisWord++;
	}

	network->addObjectNode(continentName);
	return network->getObjectNode(continentName);
}

void Continent::generatePlaceSize(SemanticNetwork* network, Node* continentNode) {
	Node* placeSizesNode = network->getObjectNode("place_size");
	string rawPlaceSizes = placeSizesNode->getField("sizes").simpleField;
	vector<string> placeSizes = strWhitespaceSplit(rawPlaceSizes, true);

	continentNode->setSimpleField("place_size", placeSizes[irand(placeSizes.size() - 1)]);
}

void Continent::generateCountries(SemanticNetwork* network, Node* continentNode) {
	Country gen(network, continentNode);

	for (int i = 0; i < 1 + irand(9); i++) {
		gen.generate();
	}
}