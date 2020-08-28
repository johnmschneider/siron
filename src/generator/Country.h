#ifndef COUNTRY_H
	#define COUNTRY_H
	#include "Generator.h"
	#include "..\SemanticNetwork.h"

	namespace generator {
		class Country : public Generator {
			private:
				semanticNetwork::Node* continentLocatedIn;

				semanticNetwork::Node* generateName(semanticNetwork::SemanticNetwork* network);
				void generatePlaceSize(semanticNetwork::SemanticNetwork* network, semanticNetwork::Node* country);
				void generateProvinces(semanticNetwork::SemanticNetwork* network, semanticNetwork::Node* country);

			public:
				Country(semanticNetwork::SemanticNetwork* network, semanticNetwork::Node* continentLocatedIn);

				void generate();
		};
	}

#endif
