#ifndef SETTLEMENT_PLACE_H
	#define SETTLEMENT_PLACE_H
	#include "Generator.h"
	#include "..\SemanticNetwork.h"

	namespace generator {
		class SettlementPlace : public Generator {
			private:
				semanticNetwork::Node* settlementLocatedIn;
				semanticNetwork::Node* settlementPlace;

				semanticNetwork::Node* getName(semanticNetwork::SemanticNetwork* network);

			public:
				SettlementPlace(semanticNetwork::SemanticNetwork* network, semanticNetwork::Node* settlementLocatedIn);

				void generate();
		};
	}

#endif