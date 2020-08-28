#ifndef PLAYER_H
	#define PLAYER_H
	#include <string>
	#include "SemanticNetwork.h"

	class Player {
		private:
			semanticNetwork::SemanticNetwork* network;
			semanticNetwork::Node* player;

			void printStartOfLocationMessage(bool arriving, semanticNetwork::Node* place, std::string placeName);
			void printSettlementPlace(std::string settlementName);
			void printSettlement(std::string country);
			void printLocationError();

		public:
			Player();
			Player(semanticNetwork::SemanticNetwork* network);

			void setPlacePlayerIsIn(semanticNetwork::Node* placePlayerIsIn);
			void chooseStartingLocation();
			semanticNetwork::Node* getNode();
			void printLocation(bool arriving = false);
			void respondToCommand(std::string command);
	};

#endif
