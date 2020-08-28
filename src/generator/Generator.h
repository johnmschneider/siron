#ifndef GENERATOR_GENERATOR_H
	#define GENERATOR_GENERATOR_H
	#include "..\SemanticNetwork.h"

	namespace generator {
		class Generator {
			private:
				semanticNetwork::SemanticNetwork* network;

			public:
				Generator(semanticNetwork::SemanticNetwork* network);

				virtual void generate() = 0;

				semanticNetwork::SemanticNetwork* getNetwork();
		};
	}
#endif