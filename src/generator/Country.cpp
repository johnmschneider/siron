#include <vector>
#include <string>
#include <unordered_map>
#include <stdint.h>
#include "Country.h"
#include "Province.h"
#include "Settlement.h"
#include "..\Main.h"
#include "..\irand.h"

using namespace generator;
using namespace semanticNetwork;
using main_::strWhitespaceSplit;
using std::vector;
using std::string;
using std::unordered_map;

Country::Country(SemanticNetwork* network, Node* continentLocatedIn) : Generator(network), continentLocatedIn(continentLocatedIn) {

}

void Country::generate() {
	SemanticNetwork* network = getNetwork();
	Node* country = generateName(network);

	network->addObjectLink(country->getData(), "is_a", "country");
	country->setSimpleField("located_in", continentLocatedIn->getData());

	generatePlaceSize(network, country);
	generateProvinces(network, country);

	vector<Node*> provinces = network->getObjectNodes("province", "located_in", country->getData());

	string description = string("the ") + country->getField("place_size").simpleField + " " +
		"country of " + country->getData() + ". It has ";

	if (provinces.size() == 1) {
		description += string("1 province: ") + provinces[0]->getData();
	}
	else if (provinces.empty()) {
		vector<Node*> settlements = network->getObjectNodes("settlement", "located_in", country->getData());
		description += "no provinces. The major ";
		
		if (settlements.size() == 1) {
			description += string("settlement is ") + settlements[0]->getData();
		}
		else {
			description += "settlements are ";

			for (int32_t i = 0; i < settlements.size() - 1; i++) {
				description += settlements[i]->getData(), ", ";
			}
			description += string(" and ") + settlements[settlements.size() - 1]->getData() + ".";
		} 
	}
	else {
		description += std::to_string(provinces.size()) + " provinces: ";

		for (int32_t i = 0; i < provinces.size() - 1; i++) {
			description += provinces[i]->getData() + ", ";
		}
		description += "and " + provinces[provinces.size() - 1]->getData() + ".";
	}

	country->setSimpleField("description", description);
}

Node* Country::generateName(SemanticNetwork* network) {
	Node*					generator = network->getObjectNode("country_generator");
	Node::FieldGetter::Info	rawNameFrags = generator->getField("name_fragments");
	vector<string>			nameFrags = strWhitespaceSplit(rawNameFrags.simpleField, true);

	static int32_t			maxSyllablesPerWord = 4;
	string					countryName = nameFrags[irand(nameFrags.size() - 1)];
	int32_t					syllablesThisWord = 1;
	int32_t					wordStop = irand(2, maxSyllablesPerWord);
	int32_t					totalSyllables = 2 + irand(maxSyllablesPerWord);
	int32_t					totalWords = 1;

	for (int32_t i = 1; i < totalSyllables; i++) {
		if (syllablesThisWord % wordStop == 0 && totalWords < 2) {
			countryName += "_" + nameFrags[irand(nameFrags.size() - 1)];

			syllablesThisWord = 0;
			wordStop = irand(2, maxSyllablesPerWord);
			totalWords++;
		}
		else {
			countryName += nameFrags[irand(nameFrags.size() - 1)];
		}

		syllablesThisWord++;
	}

	network->addObjectNode(countryName);
	Node* country = network->getObjectNode(countryName);

	return country;
}

void Country::generatePlaceSize(SemanticNetwork* network, Node* countryNode) {
	Node* placeSizesNode = network->getObjectNode("place_size");
	string rawPlaceSizes = placeSizesNode->getField("sizes").simpleField;
	vector<string> placeSizes = strWhitespaceSplit(rawPlaceSizes, true);

	countryNode->setSimpleField("place_size", placeSizes[irand(placeSizes.size() - 1)]);
}

void Country::generateProvinces(SemanticNetwork* network, Node* country) {
	Province gen(network, country);

	//chance of not generating any provinces
	int32_t count = irand(10);

	if (count == 1) {
		//it doesn't make sense to have just one province
		count = 2;
	}

	for (int32_t i = 0; i < count; i++) {
		gen.generate();
	}

	if (count == 0) {
		Settlement gen(network, country);

		for (int32_t i = 0; i < 1 + irand(10); i++) {
			gen.generate();
		}
	}
}