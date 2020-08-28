#include <vector>
#include <stdint.h>
#include "generator/SettlementPlace.h"
#include "Player.h"
#include "irand.h"
#include "Main.h"
#include "sfout.h"

using namespace semanticNetwork;
using namespace sfout_;
using std::vector;
using std::string;
using main_::strWhitespaceSplit;
using main_::indexOf;
using semanticNetwork::Node;

Player::Player() {
	//this is so we can have a reference to a player object in the main_ namespace
}

Player::Player(SemanticNetwork* network) : network(network) {
	player = network->getObjectNode("player");
}

void Player::setPlacePlayerIsIn(Node* placePlayerIsIn) {
	player->setSimpleField("located_in", placePlayerIsIn->getData());

	if (placePlayerIsIn->getField("are_sub_places_generated").simpleField == "false") {
		generator::SettlementPlace gen(network, placePlayerIsIn);

		for (int32_t i = 0; i < 1 + irand(10); i++) {
			gen.generate();
		}

		string newInfoLearned = player->getField("information_known").simpleField + " " + placePlayerIsIn->getData();
		vector<Node*> placesGenerated = network->getObjectNodes("settlement_place", "located_in", placePlayerIsIn->getData());
		for (Node* place : placesGenerated) {
			newInfoLearned += " " + place->getData();
		}

		player->setSimpleField("information_known", newInfoLearned);

		placePlayerIsIn->setSimpleField("are_sub_places_generated", "true");
	}
}

void Player::chooseStartingLocation() {
	vector<Node*> continents     = network->getObjectNodeChildren("continent");
	Node*		  continent      = continents[irand(continents.size() - 1)];
	vector<Node*> countries	     = network->getObjectNodes("country", "located_in", continent->getData());
	Node*		  country	     = countries[irand(countries.size() - 1)];
	vector<Node*> subdivisions   = network->getObjectNodes("country_subdivision", "located_in", country->getData());
	Node*		  subdivision    = (subdivisions.size() == 0 ? nullptr : subdivisions[irand(subdivisions.size() - 1)]);
	vector<Node*> settlements;
	Node*		  settlement;
	vector<Node*> settlementPlaces;
	Node*		  settlementPlace;
	string		  newInfoLearned;

	if (subdivision == nullptr) {
		settlements = network->getObjectNodes("settlement", "located_in", country->getData());
		settlement = settlements[irand(settlements.size() - 1)];

		if (settlements.size() != 1) {
			for (int32_t i = 0; i < settlements.size() - 1; i++) {
				newInfoLearned += settlements[i]->getData() + " ";
			}

			newInfoLearned += settlements[settlements.size() - 1]->getData();
		}
		else {
			newInfoLearned += settlements[0]->getData();
		}
	}
	else {
		settlements = network->getObjectNodes("settlement", "located_in", subdivision->getData());
		settlement = settlements[irand(settlements.size() - 1)];

		for (int32_t i = 0; i < subdivisions.size() - 1; i++) {
			newInfoLearned += subdivisions[i]->getData() + " ";
		}

		newInfoLearned += subdivisions[subdivisions.size() - 1]->getData();
	}

	newInfoLearned += " " + continent->getData() + " " + country->getData();

	//generate settlementName places
	setPlacePlayerIsIn(settlement);

	settlementPlaces = network->getObjectNodes("settlement_place", "located_in", settlement->getData());

	for (int32_t i = 0; ; i++) {
		if (i >= settlementPlaces.size()) {
			break;
		}

		if (settlementPlaces[i]->getField("abstract").simpleField == "true") {
			settlementPlaces.erase(settlementPlaces.begin() + i);
			i--;
		}
	}

	settlementPlace = settlementPlaces[irand(settlementPlaces.size() - 1)];
	setPlacePlayerIsIn(settlementPlace);
	player->setSimpleField("information_known", player->getField("information_known").simpleField + " " + newInfoLearned + " information_known");
}

Node* Player::getNode() {
	return player;
}

void Player::printLocation(bool arriving) {
	string placeName = player->getField("located_in").simpleField;
	Node* place		 = network->getObjectNode(placeName);
	string parent	 = place->getField("located_in").simpleField;

	printStartOfLocationMessage(arriving, place, placeName);

	if (network->queryParentObjectsInclusive(parent, "is_a", "settlement")) {
		printSettlementPlace(parent);
	}
	else if (network->queryParentObjectsInclusive(parent, "is_a", "province")){
		printSettlement(parent);
	}
	else {
		printLocationError();
	}

	sfout << Area("body");
}

void Player::printStartOfLocationMessage(bool arriving, Node* place, string placeName) {
	auto atmosphere = place->getField("place_atmosphere");

	if (arriving) {
		sfout << "You arrive in the ";
	}
	else {
		sfout << "You are in the ";
	}

	if (atmosphere.wasSuccessful) {
		sfout << atmosphere.nodeField->getData() << " ";
	}

	sfout << place->getLink("is_a")->getLinkedNodes()[0]->getData() << " \"" << placeName << "\", in ";
}

void Player::printSettlementPlace(string settlementName) {
	Node*  settlement = network->getObjectNode(settlementName);

	sfout << "the " << settlement->getField("place_size").simpleField << " " << settlement->getField("settlement_size").simpleField << " of " << settlementName << ".";

	string		  parentPlaceName = settlement->getField("located_in").simpleField;
	Node*		  parentPlace = network->getObjectNode(parentPlaceName);
	vector<Node*> countrySubdivisionTypes = network->getObjectNodeChildren("country_subdivision");
	bool		  parentFound = false;

	for (Node* divisionType : countrySubdivisionTypes) {
		if (network->queryParentObjectsInclusive(parentPlaceName, "is_a", divisionType->getData())) {

			string countryName = parentPlace->getField("located_in").simpleField;
			Node*  country = network->getObjectNode(countryName);
			string continentName = country->getField("located_in").simpleField;

			sfout << Area("countrySubdivision") << clear << divisionType->getData() << ": " << parentPlaceName << Area("country") <<
				clear << "country: " << countryName << Area("continent") << clear <<
				"continent: " << continentName;

			parentFound = true;
			break;
		}
	}

	if (parentFound = false) {
		if (network->queryParentObjectsInclusive(parentPlaceName, "is_a", "country")) {
			string continentName = parentPlace->getField("located_in").simpleField;

			sfout << "in the country " << parentPlaceName << " of the continent " << continentName << ".";
			sfout << Area("country") << clear << "country: " << parentPlaceName << Area("continent") << clear << "continent: " << continentName;
		}
		else if (network->queryParentObjectsInclusive(parentPlaceName, "is_a", "continent")) {
			sfout << "in the continent of " << parentPlaceName << ".";
			sfout << Area("continent") << clear << "continent: " << parentPlaceName;
		}
		else {
			printLocationError();
		}
	}

	sfout << Area("body") << "\n";
}

void Player::printSettlement(string provinceName) {
	Node*  province      = network->getObjectNode(provinceName);
	string countryName   = province->getField("located_in").simpleField;
	Node*  country       = network->getObjectNode(countryName);
	string continentName = country->getField("located_in").simpleField;

	sfout << "the province of " << provinceName << ".\n";

	sfout << Area("countrySubdivision") << clear << "province: " << provinceName << Area("country") <<
		clear << "country: " << countryName << Area("continent") << clear <<
		"continent: " << continentName;
}

void Player::printLocationError() {
	sfout << sfout_::Area("error") << "Player::printLocation: invalid parent location of settlement\n";
	system("PAUSE");
	exit(1);
}

void Player::respondToCommand(string command) {
	if (command.find("walk to") != string::npos) {
		string whereToWalkTo = command.substr(8);
		Node* locationParentNode = network->getObjectNode(player->getField("located_in").simpleField);
		string locationParent_dot_name;
		bool foundLocationToWalkTo = false;

		while (locationParentNode != nullptr) {
			locationParent_dot_name = locationParentNode->getData() + "." + whereToWalkTo;

			if (locationParentNode->getField(whereToWalkTo).wasSuccessful) {
				sfout << "You walk to the " << whereToWalkTo << ".\n";
				foundLocationToWalkTo = true;
				break;
			}
			else if (locationParentNode->getField(locationParent_dot_name).wasSuccessful) {
				sfout << "You walk to the " << whereToWalkTo << ".\n";
				foundLocationToWalkTo = true;
				break;
			}
			else {
				vector<Node*> locations = network->getObjectNodes(whereToWalkTo, "located_in", locationParentNode->getData());

				if (!locations.empty()) {
					Node* location		= locations[0];
					string locationName = location->getData();
					string parentName   = locations[0]->getField("located_in").simpleField;

					foundLocationToWalkTo = true;
					
					if (network->queryParentObjectsInclusive(parentName, "is_a", "settlement_place")) {
						sfout << "You walk to the " << locationName << ".\n";
					}
					else {
						sfout << "You begin walking to " << locationName << ".\n";
						setPlacePlayerIsIn(locations[0]);
						printLocation(true);
					}

					break;
				}

				Node::FieldGetter::Info rawParent = locationParentNode->getField("located_in");
				
				if (rawParent.wasSuccessful) {
					locationParentNode = network->getObjectNode(rawParent.simpleField);
				} 
				else {
					//location not found
					locationParent_dot_name = locationParentNode->getData() + "." + whereToWalkTo;
					locationParentNode = nullptr;
				}
			}
		}

		if (!foundLocationToWalkTo) {
			sfout << sfout_::Area("error") << sfout_::clear << "Could not find location \"" << whereToWalkTo << "\" (" << locationParent_dot_name << ")";
			sfout << sfout_::Area("body");
		}
	}
	else if (command.find("recall") != string::npos) {
		vector<string> thingsPlayerKnows = strWhitespaceSplit(player->getField("information_known").simpleField);
		string recallThis;
		bool descriptionFound = true;
		Node* recallThisNode;

		if (command.length() > 6) {
			recallThis = command.substr(7);
		}
		else {
			recallThis = "information_known";
		}

		if (recallThis.empty()) {
			recallThis = "information_known";
		}

		auto index	   = indexOf(thingsPlayerKnows, recallThis);
		recallThisNode = network->getObjectNode(recallThis);

		if (index != thingsPlayerKnows.end()) {
			auto rawField = player->getField(recallThis);

			if (rawField.wasSuccessful) {
				if (rawField.simpleField == Node::NULL_SIMPLE_FIELD) {
					descriptionFound = false;
				}
				else {
					vector<string> fields = strWhitespaceSplit(rawField.simpleField, true);

					sfout << "You know:\n    ";

					for (int32_t i = 0; i < fields.size() - 1; i++) {
						sfout << fields[i] << ", ";
					}
					sfout << "and " << fields[fields.size() - 1] << ".";
				}
			}
			else {
				auto rawDescription = recallThisNode->getField("description");

				if (rawDescription.wasSuccessful && rawDescription.simpleField != Node::NULL_SIMPLE_FIELD) {
					sfout << rawDescription.simpleField << "\n";
				}
				else {
					descriptionFound = false;
				}
			}
		}
		else {
			sfout << "You have no knowledge about \"" << recallThis << "\".";
		}

		if (!descriptionFound) {
			sfout << sfout_::Area("error") << sfout_::clear << "\"" << recallThisNode->getData() << "\" has no description attached. Dumping raw data.";
			sfout << sfout_::Area("body");
			sfout << "You know:\n    " << network->nodeToString(recallThisNode);
		}

		sfout << "\n";
	}
	else if (command.find("exit") != string::npos) {
		main_::window->close();
	}
	else {
		sfout << sfout_::Area("error") << sfout_::clear << "Unrecognized command \"" << command << "\"";
		sfout << sfout_::Area("body");
	}
}