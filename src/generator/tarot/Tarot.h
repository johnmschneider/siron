#ifndef TAROT_h
	#define TAROT_H
	#include <stdint.h>
	#include <vector>
	#include "../Generator.h"
	#include "../../SemanticNetwork.h"

	namespace generator {
		namespace tarot {
			class Tarot : public Generator {
				using node_vector = std::vector<semanticNetwork::Node*>;

				private:
					int32_t cardCount;
					node_vector generatedCards;

				public:
					Tarot(semanticNetwork::SemanticNetwork* network);

					void		setSpreadCount(int32_t numberOfCards);
					void		generate();
					node_vector getGeneratedCards();
			};
		}
	}

#endif