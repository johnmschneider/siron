#include <unordered_map>
#include "Province.h"
#include "..\Main.h"
#include "..\irand.h"
#include "Settlement.h"

using namespace semanticNetwork;
using namespace generator;
using main_::strWhitespaceSplit;

using std::string;
using std::vector;
using std::unordered_map;

Province::Province(SemanticNetwork* network, Node* country) : Generator(network), country(country) {

}

void Province::generate() {
	SemanticNetwork* network = getNetwork();
	Node* province = generateName(network);

	province->setSimpleField("not_has_a", "plural_form");
	network->addObjectLink(province->getData(), "is_a", "province");
	province->setSimpleField("located_in", country->getData());

	generatePlaceSize(network, province);
	generateSettlements(network, province);
}

Node* Province::generateName(SemanticNetwork* network) {
	Node*					generator = network->getObjectNode("province_generator");
	Node::FieldGetter::Info	rawNameFrags = generator->getField("name_fragments");
	vector<string>			nameFrags = strWhitespaceSplit(rawNameFrags.simpleField, true);

	static int				maxSyllablesPerWord = 3;
	string					provinceName = nameFrags[irand(nameFrags.size() - 1)];
	int						syllablesThisWord = 1;
	int						wordStop = irand(2, maxSyllablesPerWord);
	int						totalSyllables = 2 + irand((maxSyllablesPerWord - 1) * 2);
	int						totalWords = 1;

	for (int i = 1; i < totalSyllables; i++) {
		if (syllablesThisWord % wordStop == 0 && totalWords < 2) {
			provinceName += "_" + nameFrags[irand(nameFrags.size() - 1)];

			syllablesThisWord = 0;
			wordStop = irand(2, maxSyllablesPerWord);
			totalWords++;
		}
		else {
			provinceName += nameFrags[irand(nameFrags.size() - 1)];
		}

		syllablesThisWord++;
	}

	network->addObjectNode(provinceName);
	return network->getObjectNode(provinceName);
}

void Province::generatePlaceSize(SemanticNetwork* network, Node* provinceNode) {
	Node* placeSizesNode = network->getObjectNode("place_size");
	string rawPlaceSizes = placeSizesNode->getField("sizes").simpleField;
	vector<string> placeSizes = strWhitespaceSplit(rawPlaceSizes, true);

	provinceNode->setSimpleField("place_size", placeSizes[irand(placeSizes.size() - 1)]);
}

void Province::generateSettlements(SemanticNetwork* network, Node* provinceNode) {
	Settlement gen(network, provinceNode);

	for (int i = 0; i < 1 + irand(10); i++) {
		gen.generate();
	}
}