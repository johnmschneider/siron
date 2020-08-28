#ifndef SETTLEMENT_H
	#define SETTLEMENT_H
	#include "Generator.h"
	#include "..\SemanticNetwork.h"

	namespace generator {
		class Settlement : public Generator {
			private:
				semanticNetwork::Node* parentPlace;

				semanticNetwork::Node* generateName(semanticNetwork::SemanticNetwork* network);
				void generatePlaceSize(semanticNetwork::SemanticNetwork* network, semanticNetwork::Node* settlementNode);
				void generateSettlementSize(semanticNetwork::SemanticNetwork* network, semanticNetwork::Node* settlementNode);

			public:
				Settlement(semanticNetwork::SemanticNetwork* network, semanticNetwork::Node* parentPlace);

				void generate();
		};
	}
#endif