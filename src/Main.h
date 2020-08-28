#ifndef MAIN_H
	#define MAIN_H
	#include <vector>
	#include <string>
	#include <memory>
	#include <iostream>
	#include <SFML\Window.hpp>
	#include <SFML\Graphics.hpp>
	#include <SFML\System.hpp>
	#include <stdint.h>
	#include "SemanticNetwork.h"
	#include "Player.h"

	namespace main_ {
		void printAllData(std::vector<std::string> args, semanticNetwork::SemanticNetwork &network);
		void runOldTests(std::vector<std::string> args);

		std::vector<std::string> argvToVector(int argc, char** argv);
		int main_(std::vector<std::string> args);

		std::vector<std::string> strWhitespaceSplit(std::string strToSplit, bool preserveStrings = false);
		std::vector<std::string> strSplit(std::string strToSplit, std::string delimiter);

		template<typename T>
		bool isValueInVector(std::vector<T> &vec, T value) {
			return indexOf<T>(vec, value) != vec.end();
		}

		template<typename T>
		typename std::vector<T>::iterator indexOf(std::vector<T> &vec, T value) {
			return std::find(vec.begin(), vec.end(), value);
		}

		void renderSfOut();
		void getline(std::string& output);

		extern sf::RenderWindow* window;
		extern sf::Font* font;
		extern Player* player;
	};

#endif