#include <iostream>
#include <time.h>
#include <sstream>
#include <stdint.h>
#include <SFML\Window.hpp>
#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include "Main.h"
#include "SemanticNetwork.h"
#include "SemanticNetworkLoader.h"
#include "generator/Continent.h"
#include "irand.h"
#include "Player.h"
#include "sfout.h"

using namespace main_;
using namespace semanticNetwork;
using namespace sf;

using std::vector;
using std::string;
using std::unordered_map;
using std::endl;
using std::cout;
using std::to_string;
using std::istringstream;
using std::getline;
using std::ostream;
using semanticNetwork::linkmap;
using semanticNetwork::linkvec;
using sfout_::sfout;

RenderWindow* main_::window;
Font* main_::font;
Player* main_::player;

vector<string> main_::strWhitespaceSplit(string strToSplit, bool preserveStrings) {
	int32_t charsFoundSoFar = 0;
	int32_t substringStartIndex = 0;
	vector<string> splitString;
	vector<char> delimeters {'\n', '\t', ' '};
	bool inString = false;

	if (strToSplit.find(" ") == string::npos) {
		if (!strToSplit.empty()) {
			splitString.push_back(strToSplit);
		}

		return splitString;
	}

	for (int32_t i = 0; i < (int32_t) strToSplit.length(); i++) {
		char curChar = strToSplit[i];

		if (preserveStrings && (inString || curChar == '"')) {
			if (curChar == '"') {
				if (inString) {
					int32_t substringEndIndex = i - substringStartIndex;

					string substring = strToSplit.substr(substringStartIndex, substringEndIndex);

					if (!substring.empty()) {
						splitString.push_back(substring);
					}

					substringStartIndex = i + 1;

					inString = false;
				}
				else {
					substringStartIndex = i + 1;

					inString = true;
				}
			}
		}
		else {
			if (i == ((int32_t) strToSplit.length()) - 1) {
				string substring = strToSplit.substr(substringStartIndex, strToSplit.length());

				if (!substring.empty()) {
					splitString.push_back(substring);
				}
			}
			else {
				for (char delimiter : delimeters) {
					if (curChar == delimiter) {
						int32_t substringEndIndex = i - substringStartIndex;
						string substring = strToSplit.substr(substringStartIndex, substringEndIndex);

						if (!substring.empty()) {
							splitString.push_back(substring);
						}

						substringStartIndex = i + 1;
					}
				}
			}
		}
	}

	return splitString;
}

vector<string> main_::strSplit(string strToSplit, string delimiter) {
	int32_t charsFoundSoFar = 0;
	int32_t substringStartIndex = 0;
	vector<string> splitString;

	for (int32_t i = 0; i < (int32_t) strToSplit.length(); i++) {
		char curChar = strToSplit[i];

		if (charsFoundSoFar == 0 && curChar == delimiter[0]) {
			if (delimiter.length() == 1) {
				int32_t substringEndIndex = i - substringStartIndex;

				splitString.push_back(strToSplit.substr(substringStartIndex, substringEndIndex));
				substringStartIndex = i + 1;

				charsFoundSoFar = 0;
			}
			else {
				charsFoundSoFar++;
			}
		}
		else if (curChar == delimiter[charsFoundSoFar]) {
			if (charsFoundSoFar == (int32_t) delimiter.length()) {
				int32_t substringEndIndex = i - substringStartIndex;

				splitString.push_back(strToSplit.substr(substringStartIndex, substringEndIndex));
				substringStartIndex = i + 1;

				charsFoundSoFar = 0;
			}
		}
		else if (i == ((int32_t) strToSplit.length()) - 1) {
			splitString.push_back(strToSplit.substr(substringStartIndex));
		}
	}

	return splitString;
}

void main_::runOldTests(vector<string> args) {
	SemanticNetwork semanticNetwork;

	semanticNetwork.addObjectNode("color");
	semanticNetwork.addObjectNode("red",        linkmap{ { "is_a", linkvec{ "color" } } });
	semanticNetwork.addObjectNode("yellow",     linkmap{ { "is_a", linkvec{ "color" } } });
	semanticNetwork.addObjectNode("green",      linkmap{ { "is_a", linkvec{ "color" } } });
	semanticNetwork.addObjectNode("lime green", linkmap{ { "is_a", linkvec{ "color" } } });
	

	semanticNetwork.addObjectNode("edible");

	semanticNetwork.addObjectNode("fruit", linkmap{ {"is_a", linkvec{ "edible"} } });
	semanticNetwork.addObjectNode("apple",        linkmap{ {"is_a",     linkvec{"fruit"}},
													       {"is_color", linkvec{"red", "yellow", "green"}} });
	semanticNetwork.addObjectNode("granny smith", linkmap{ {"is_a",     linkvec{"apple"}} });

	semanticNetwork.addObjectNode("unripe fig", linkmap{ {"is_a",     linkvec{"fruit"}},
														 {"not_is_a", linkvec{"edible"}},
														 {"is_color", linkvec{"lime green"}} });
	semanticNetwork.printNetwork();
	sfout << "\n";

	clock_t timeElapsed = clock();
	for (int32_t i = 0; i < 100; i++) {
		semanticNetwork.addObjectNode("stressTestNode" + to_string(i));
	}

	for (int32_t i = 100; i < 10000; i++) {
		semanticNetwork.addObjectNode("stressTestNode" + to_string(i));

		for (int32_t i2 = 0; i < 100; i2++) {
			semanticNetwork.addObjectLink("stressTestNode" + to_string(i), "is_a", "stressTestNode" + to_string(i2));
		}
	}
	
	timeElapsed = clock() - timeElapsed;
	sfout << "Insert time (ms) : " << (((float) timeElapsed) / CLOCKS_PER_SEC) * 1000 << "\n";

	timeElapsed = clock();
	sfout << "Is apple edible? "        <<  (semanticNetwork.queryParentObjectsInclusive("apple", "is_a", "edible") ? "yes" : "no") << "\n";
	sfout << "Is granny smith edible? " <<  (semanticNetwork.queryParentObjectsInclusive("granny smith", "is_a", "edible") ? "yes" : "no") << "\n";
	sfout << "Is unripe fig edible? "    << (semanticNetwork.queryParentObjectsInclusive("unripe fig", "is_a", "edible") ? "yes" : "no") << "\n";
	timeElapsed = clock() - timeElapsed;
	sfout << "Query time (ms) : " << (((float)timeElapsed) / CLOCKS_PER_SEC) * 1000 << "\n";
}

void main_::printAllData(vector<string> args, SemanticNetwork &network) {
	vector<Node*> continents = network.getObjectNodeChildren("continent");

	sfout << "There ";

	//size - 1 so we can add the last continent with "and ContinentName."
	if (continents.size() == 1)
	{
		sfout << " is " << continents.size() << " known continent: " << continents[0]->getData();
	}
	else {
		sfout << "are " << continents.size() << " known continents in the world : ";

		for (int32_t i = 0; i < continents.size() - 1; i++) {
			sfout << continents[i]->getData() << ", ";
		}
		sfout << "and " << continents[continents.size() - 1]->getData() << "." << "\n" << "\n";
	}

	for (Node* continent : continents) {
		vector<Node*> countries = network.getObjectNodes("country", "located_in", continent->getData());
		
		sfout << "Within " << continent->getData();

		if (countries.size() == 1)
		{
			sfout << " there is " << countries.size() << " country: " << countries[0]->getData() << "\n" << "\n" << "\n";
		}
		else {
			sfout << " there are " << countries.size() << " countries: ";

			for (int32_t i = 0; i < countries.size() - 1; i++) {
				sfout << countries[i]->getData() << ", ";
			}
			sfout << "and " << countries[countries.size() - 1]->getData() << "." << "\n" << "\n" << "\n";
		}

		for (Node* country : countries) {
			vector<Node*> countrySubdivisions = network.getObjectNodes("country_subdivision", "located_in", country->getData());
			vector<Node*> settlements;

			if (countrySubdivisions.empty()) {
				vector<Node*> subdivisonTypes = network.getObjectNodeChildren("country_subdivision");
				settlements = network.getObjectNodes("settlement", "located_in", country->getData());

				sfout << country->getData() << " is not divided into ";
				for (int32_t i = 0; i < subdivisonTypes.size() - 1; i++) {
					sfout << subdivisonTypes[i]->getField("plural_form").simpleField << " or ";
				}
				sfout << subdivisonTypes[subdivisonTypes.size() - 1]->getField("plural_form").simpleField << "." << "\n" << "\n";

				for (Node* settlement : settlements) {
					sfout << settlement->getData() << " is a " << settlement->getField("place_size").simpleField << " " <<
						settlement->getField("settlement_size").simpleField << "." << "\n";
				}
			}
			else {
				sfout << "Within " << country->getData() << " there ";

				vector<Node*> subdivisionTypes = network.getObjectNodeChildren("country_subdivision");

				{
					int32_t size = network.getObjectNodes(subdivisionTypes[0]->getData(), "located_in", country->getData()).size();

					if (size == 1) {
						sfout << "is ";
					}
					else {
						sfout << "are ";
					}
				}

				if (subdivisionTypes.size() == 1)
				{
					Node* type = subdivisionTypes[0];
					vector<Node*> typeInstances = network.getObjectNodes(type->getData(), "located_in", country->getData());
					int32_t size = typeInstances.size();

					sfout << size << " " << (size == 1 ? type->getData() : type->getField("plural_form").simpleField) << ".";
				}
				else {
					for (int32_t i = 0; i < subdivisionTypes.size() - 1; i++) {
						Node* type = subdivisionTypes[i];
						vector<Node*> typeInstances = network.getObjectNodes(type->getData(), "located_in", country->getData());
						int32_t size = typeInstances.size();

						sfout << size << " " << (size == 1 ? type->getData() : type->getField("plural_form").simpleField) << ", ";
					}

					{
						Node* type = subdivisionTypes[subdivisionTypes.size() - 1];
						vector<Node*> typeInstances = network.getObjectNodes(type->getData(), "located_in", country->getData());
						int32_t size = typeInstances.size();

						sfout << "and " << size << " " << (size == 1 ? type->getData() : type->getField("plural_form").simpleField) << ".";
					}
				}

				for (int32_t i = 0; i < subdivisionTypes.size(); i++) {
					Node* type = subdivisionTypes[i];
					vector<Node*> typeInstances = network.getObjectNodes(type->getData(), "located_in", country->getData());

					if (!typeInstances.empty()) {
						sfout << "\n" << "The " << type->getField("plural_form").simpleField << " are: ";

						for (int32_t i = 0; i < typeInstances.size() - 1; i++) {
							sfout << typeInstances[i]->getData() << ", ";
						}

						sfout << "and " << typeInstances[typeInstances.size() - 1]->getData() << "\n";
					}
				}

				for (int32_t i = 0; i < subdivisionTypes.size(); i++) {
					Node* type = subdivisionTypes[i];
					vector<Node*> typeInstances = network.getObjectNodes(type->getData(), "located_in", country->getData());

					for (int32_t i = 0; i < typeInstances.size(); i++) {
						settlements = network.getObjectNodes("settlement", "located_in", typeInstances[i]->getData());

						for (Node* settlement : settlements) {
							sfout << settlement->getData() << " is a " << settlement->getField("place_size").simpleField << " " <<
								settlement->getField("settlement_size").simpleField << "." << "\n";
						}
					}
				}
			}

			sfout << "\n" << "\n" << "\n";
		}

		sfout << "\n" << "\n" << "\n";
	}
}

vector<string> main_::argvToVector(int argc, char** argv) {
	string curArg = "";
	vector<string> args;

	for (int32_t curArgIndex = 0; curArgIndex < argc; curArgIndex++) {
		char* rawCurArg = argv[curArgIndex];

		for (int32_t argCharIndex = 0; ; argCharIndex++) {
			char curChar = rawCurArg[argCharIndex];

			if (curChar == '\0') {
				break;
			}
			else {
				curArg += curChar;
			}
		}
	}

	return args;
}

int main_::main_(vector<string> args) {
	//runOldTests(args);

	VideoMode rawDesktopMode = VideoMode::getDesktopMode();
	RenderWindow rawWindow(VideoMode(rawDesktopMode.width - 100, rawDesktopMode.height - 100), "DndAi");
	main_::window = &rawWindow;
	
	font = new Font();
	font->loadFromFile(R"(D:\non college related\c plus plus\visual studio\DndAi\res\DwarfFortressVan.ttf)");

	SemanticNetworkLoader loader;
	SemanticNetwork network;
	Node* playerNode;
	string playerName;

	srand(0);
	generator::Continent gen(&network);

	loader.loadFromFile(network, "<Main.cpp>", R"(D:\non college related\c plus plus\visual studio\DndAi\res\nodes\main.txt)");
	
	sfout << sfout_::Area("error") << sfout_::Point(1000, 0) << sfout_::Color(255, 0, 0, 255) << sfout_::Color(205, 205, 205, 255, false);
	sfout << sfout_::Area("countrySubdivision") << sfout_::Point(50, 0) << sfout_::Color(0, 0, 0, 255) << sfout_::Color(205, 205, 205, 255, false);
	sfout << sfout_::Area("country") << sfout_::Point(300, 0) << sfout_::Color(0, 0, 0, 255) << sfout_::Color(205, 205, 205, 255, false);
	sfout << sfout_::Area("continent") << sfout_::Point(550, 0) << sfout_::Color(0, 0, 0, 255) << sfout_::Color(205, 205, 205, 255, false);
	sfout << sfout_::Area("body");

	int32_t continentCount = 1 + irand(10);
	for (int32_t i = 0; i < continentCount; i++) {
		gen.generate();
		sfout << sfout_::clear << "generating continent " << i << "/" << continentCount;
	}

	sfout << sfout_::clear;

	player = new Player(&network);
	playerNode = player->getNode();

	//commented out so i don't get sick of hearing the same music
	/*Music titleMusic;
	titleMusic.openFromFile(R"(D:\non college related\c plus plus\visual studio\DndAi\res\music\teller-of-the-tales-by-kevin-macleod-from-filmmusic-io.ogg)");
	titleMusic.play();*/

	sfout << "What is your name?" << "\n\n> ";
	main_::getline(playerName);
	sfout << "\n";

	playerNode->setSimpleField("name", playerName);
	player->chooseStartingLocation();

	sfout << "\nYou are " << playerNode->getField("name").simpleField << ". ";
	player->printLocation();
	sfout << "\n";

	/*sfout << "(debug) tavern node: ";
	Node* tavernNode = network.getObjectNode(playerNode->getField("located_in").simpleField);
	sfout << network.nodeToString(tavernNode) << "\n";*/
	
	sfout << "What do you do next?";
	sfout << "\n";

	//sfout << "(debug) printing out world info next: " << "\n";
	////system("PAUSE");
	//network.printNetwork();

	while (window->isOpen()) {
		Event event;

		while (window->pollEvent(event)) {
			if (event.type == Event::Closed) {
				window->close();
			}
		}

		string command;
		sfout << "\n> ";
		main_::getline(command);
		sfout << "\n\n";
		player->respondToCommand(command);

		renderSfOut();
	}

	delete player;
	delete font;

	return 0;
}

void main_::getline(string& output) {
	static const int32_t ENTER = 13;
	static const int32_t BACKSPACE = 8;

	string startText = sfout.getCurrentAreaText();

	while (window->isOpen()) {
		Event event;

		while (window->pollEvent(event)) {
			if (event.type == Event::Closed) {
				window->close();
			}
			else if (event.type == Event::TextEntered) {
				char character = static_cast<char> (event.text.unicode);

				if (character == BACKSPACE) {
					if (!output.empty()) {
						output = output.substr(0, output.length() - 1);
						sfout << sfout_::clear;
						sfout << startText << output;
					}
				}
				else if (character == ENTER) {
					return;
				}
				else if (event.text.unicode < 128) {
					sfout << character;
					output += character;
				}
			}
		}

		renderSfOut();
	}
}

void main_::renderSfOut() {
	window->clear();

	static float paddingPixels = 4;
	static float halfPadding = paddingPixels / 2;

	for (auto instructionPair : sfout.getInstructions()) {
		sfout_::Instruction instruction = instructionPair.second;
		sfout_::Color textColor			= instruction.textColor;
		sfout_::Color backgroundColor	= instruction.backgroundColor;
		sfout_::Point position			= instruction.position;

		float width = (float) (instruction.textToDraw.size());
		float height = 16 + paddingPixels;

		if (width != 0) {
			width = paddingPixels + (16 * (width/2));
		}

		sf::RectangleShape background(Vector2f(width, height));
		background.setPosition(instruction.position.x - halfPadding, halfPadding + instruction.position.y);
		background.setFillColor(Color(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a));
		
		window->draw(background);

		Text outText(instruction.textToDraw, *font, 16);
		outText.setPosition(position.x, position.y);
		outText.setFillColor(Color(textColor.r, textColor.g, textColor.b, textColor.a));
		
		window->draw(outText);
	}

	window->display();
}

int main(int argc, char* argv[]) {
	return main_::main_(argvToVector(argc, argv));
}