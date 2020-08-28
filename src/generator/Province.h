#ifndef PROVINCE_H
	#define PROVINCE_H
	#include <string>
	#include "Generator.h"
	#include "..\SemanticNetwork.h"

	namespace generator {
		class Province : public Generator {
			private:
				semanticNetwork::Node* country;

				semanticNetwork::Node* generateName(semanticNetwork::SemanticNetwork* network);
				void generatePlaceSize(semanticNetwork::SemanticNetwork* network, semanticNetwork::Node* provinceNode);
				void generateSettlements(semanticNetwork::SemanticNetwork* network, semanticNetwork::Node* provinceNode);

			public:
				Province(semanticNetwork::SemanticNetwork* network, semanticNetwork::Node* country);

				void generate();
		};
	}

#endif