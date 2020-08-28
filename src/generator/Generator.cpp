#include "Generator.h"
using namespace generator;
using namespace semanticNetwork;

Generator::Generator(SemanticNetwork* network) : network(network) {

}

SemanticNetwork* Generator::getNetwork() {
	return this->network;
}