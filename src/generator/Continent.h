#ifndef CONTINENT_H
	#define CONTINENT_H
	#include <vector>
	#include <string>
	#include "Generator.h"
	#include "..\SemanticNetwork.h"

	namespace generator {
		class Continent : public Generator {
			private:
				semanticNetwork::Node* generateName(semanticNetwork::SemanticNetwork* network);
				void generatePlaceSize(semanticNetwork::SemanticNetwork* network, semanticNetwork::Node* countryNode);
				void generateCountries(semanticNetwork::SemanticNetwork* network, semanticNetwork::Node* countryNode);

			public:
				Continent(semanticNetwork::SemanticNetwork* network);

				void generate();
		};
	}

#endif