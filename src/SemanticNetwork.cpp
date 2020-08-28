#include <iostream>
#include <stdint.h>
#include "SemanticNetwork.h"
#include "Main.h"
#include "sfout.h"

using std::vector;
using std::string;
using std::unordered_map;
using std::endl;
using sfout_::sfout;
using std::pair;
using std::to_string;
using main_::strWhitespaceSplit;
using namespace semanticNetwork;
using namespace sfout_;

Link::Link(string linkName, vector<Node*> links) {
	this->linkName = linkName;
	this->linkedNodes = links;
}

Link::Link(string linkName) : Link(linkName, vector<Node*>()) {
	
}

Link::~Link() {

}

string Link::getLinkName() {
	return linkName;
}

vector<Node*> Link::getLinkedNodes() {
	return this->linkedNodes;
}

void Link::addLinkedNode(Node* nodeToLink) {
	this->linkedNodes.push_back(nodeToLink);
}




string const Node::NULL_SIMPLE_FIELD = "";

Node::Node(string data, bool isWordNode, vector<Link*> linkedNodes) {
	this->data = data;
	this->isThisWordNode = isWordNode;
	this->links = linkedNodes;
}

Node::Node(string simpleFields, bool isWordNode) : Node(simpleFields, isWordNode, vector<Link*>()) {

}

Node::~Node() {
	for (int32_t i = 0; i < this->links.size(); i++) {
		delete links.at(i);
		links.at(i) = nullptr;
	}
}

string Node::getData() {
	return this->data;
}

void Node::addLink(string linkName, Node* nodeToLinkTo) {
	Link* link = getLink(linkName);

	if (link == nullptr) {
		link = new Link(linkName);
		this->links.push_back(link);
	}

	link->addLinkedNode(nodeToLinkTo);

	if (linkName == "is_a") {
		//inherit fields
		
		vector<Node*> curParents;

		curParents.push_back(nodeToLinkTo);

		for (int32_t i = 0; ; i ++) {
			if (i >= curParents.size()) {
				break;
			}

			Node* curParent = curParents[i];
			
			//	inheret simple fields
			for (auto field : curParent->getSimpleFields()) {
				if (getField(field.first).wasSuccessful == false) {
					Node::FieldGetter::Info rawAntiFields = getField("not_has_a");
					bool noMatchingAntiField = true;

					if (rawAntiFields.wasSuccessful) {
						vector<string> simpleAntiFields = strWhitespaceSplit(rawAntiFields.simpleField, true);

						for (string antiField : simpleAntiFields) {
							if (antiField == field.first) {
								noMatchingAntiField = false;
								break;
							}
						}
					}

					if (noMatchingAntiField) {
						setSimpleField(field.first, field.second);
					}
				}
			}

			//	inherit node fields
			for (auto field : curParent->getNodeFields()) {
				if (getField(field.first).wasSuccessful == false) {
					Node::FieldGetter::Info rawAntiFields = getField("not_has_a");
					bool noMatchingAntiField = true;

					if (rawAntiFields.wasSuccessful) {
						vector<string> simpleAntiFields = strWhitespaceSplit(rawAntiFields.simpleField, true);

						for (string antiField : simpleAntiFields) {
							if (antiField == field.first) {
								noMatchingAntiField = false;
								break;
							}
						}
					}

					if (noMatchingAntiField) {
						setNodeField(field.first, field.second);
					}
				}
			}

			vector<Node*> newParents = curParent->getLinkedNodes("is_a");
			for (Node* node : newParents) {
				curParents.push_back(node);
			}
		}

	}
}

Link* Node::getLink(string linkName) {
	for (int32_t i = 0; i < this->links.size(); i++) {
		Link* curLink = links.at(i);

		if (curLink->getLinkName() == linkName) {
			return curLink;
		}
	}

	return nullptr;
}

vector<Link*> Node::getNodeLinks() {
	return links;
}

vector<Node*> Node::getLinkedNodes(string linkName) {

	for (int32_t i = 0; i < links.size(); i++) {
		Link* curLink = links[i];

		if (curLink->getLinkName() == linkName) {
			return curLink->getLinkedNodes();
		}
	}

	return vector<Node*>();
}

bool Node::isWordNode() {
	return isThisWordNode;
}

void Node::setSimpleField(string fieldName, string value) {
	simpleFields[fieldName] = value;
}

void Node::setNodeField(string fieldName, Node* value) {
	nodeFields[fieldName] = value;
}

Node::FieldGetter::Info Node::getField(string fieldName) {
	auto it = simpleFields.find(fieldName);
	auto it2 = nodeFields.find(fieldName);

	if (it != simpleFields.end()) {
		if (it2 != nodeFields.end()) {
			return Node::FieldGetter::Info(true, simpleFields[fieldName], nodeFields[fieldName]);
		}
		else {
			return Node::FieldGetter::Info(true, simpleFields[fieldName], nullptr);
		}
	}
	else if (it2 != nodeFields.end()) {
		return Node::FieldGetter::Info(true, NULL_SIMPLE_FIELD, nodeFields[fieldName]);
	}
	
	return Node::FieldGetter::Info (false, NULL_SIMPLE_FIELD, nullptr);
}

unordered_map<string, string> Node::getSimpleFields() {
	return simpleFields;
}

unordered_map<string, Node*> Node::getNodeFields() {
	return nodeFields;
}




SemanticNetwork::SemanticNetwork() {

}

SemanticNetwork::~SemanticNetwork() {
	for (int32_t i = 0; i < this->nodes.size(); i++) {
		delete nodes.at(i);
		nodes.at(i) = nullptr;
	}
}

void SemanticNetwork::addWordNode(string nodeData, linkmap nodeLinks) {
	addNode(nodeData, nodeLinks, true);
}

void SemanticNetwork::addWordNode(string nodeData) {
	addWordNode(nodeData, linkmap());
}

void SemanticNetwork::addObjectNode(string nodeData, linkmap nodeLinks) {
	addNode(nodeData, nodeLinks, false);
}

void SemanticNetwork::addObjectNode(string nodeData) {
	addObjectNode(nodeData, linkmap());
}

void SemanticNetwork::addNode(string nodeData, linkmap nodeLinks, bool isWordNode) {

	for (Node* node : nodes) {
		if (node->getData() == nodeData) {
			int32_t nodesWithSameName = 0;

			sfout << Area("error") /*<< clear*/ << "Trying to add second node named \"" << nodeData << "\"" << "        ";

			for (int32_t i = 2; ; i++) {
				if (getNodeIndex(isWordNode, nodeData + "_" + to_string(i)) != -1) {
					nodesWithSameName++;
				}
				else {
					break;
				}
			}

			nodeData += "_" + to_string(2 + nodesWithSameName);
			sfout << "Renamed it to \"" << nodeData << "\"";
			sfout << Area("body");
			break;
		}
	}

	Node* node = new Node(nodeData, isWordNode);

	nodes.push_back(node);

	if (!nodeLinks.empty()) {
		for (auto &it : nodeLinks) {
			string linkName = it.first;
			vector<string> endNodes = it.second;

			for (auto &linkedNodeData2 : endNodes) {
				addLink(isWordNode, nodeData, linkName, linkedNodeData2);
			}
		}
	}
}

void SemanticNetwork::addWordLink(string startNodeData, string linkName, string endNodeData) {
	addLink(true, startNodeData, linkName, endNodeData);
}

void SemanticNetwork::addObjectLink(string startNodeData, string linkName, string endNodeData) {
	addLink(false, startNodeData, linkName, endNodeData);
}

void SemanticNetwork::addLink(bool isWord, string startNodeData, string linkName, string endNodeData) {
	int32_t startNodeIndex = getNodeIndex(isWord, startNodeData);

	if (startNodeIndex != -1) {
		Node* startNode = nodes[startNodeIndex];
		addLinkWithValidStartNode(isWord, startNode, linkName, endNodeData);
	}
	else {
		sfout << Area("error") << "SemanticNetwork.addLink: start node == " << startNodeData << ", ending node \"" + endNodeData + "\" not found" << "\n";
		sfout << Area("body");
		system("PAUSE");
		exit(1);
	}
}

void SemanticNetwork::addLinkWithValidStartNode(bool isWord, Node* startNode, string linkName, string endNodeData) {
	if (linkName == "has_a") {
		startNode->setSimpleField(endNodeData, Node::NULL_SIMPLE_FIELD);
	}
	else {
		int32_t endNodeIndex = getNodeIndex(isWord, endNodeData);

		if (endNodeIndex != -1) {
			Node* endNode = nodes[endNodeIndex];
			Link* link = startNode->getLink(linkName);

			if (link != nullptr) {
				link->addLinkedNode(endNode);
			}
			else {
				startNode->addLink(linkName, endNode);
			}
		}
		else {
			sfout << Area("error") << "SemanticNetwork.addLinkWithValidStartNode: start node == " << startNode->getData() << ", ending node \"" + endNodeData + "\" not found" << "\n";
			sfout << Area("body");
			system("PAUSE");
			exit(1);
		}
	}
}

int32_t SemanticNetwork::getNodeIndex(bool isQueryingWord, string nodeData) {

	for (int32_t i = 0; i < nodes.size(); i++) {
		Node* curNode = nodes.at(i);

		bool isWord = curNode->isWordNode();

		if (curNode->getData() == nodeData &&
		   (isQueryingWord ? isWord : isWord == false)) {
			return i;
		}
	}

	return -1;
}

bool SemanticNetwork::queryWord(string startNodeData, string linkName, string expectedEndNodeData) {
	return query(true, startNodeData, linkName, expectedEndNodeData);
}

bool SemanticNetwork::queryObject(string startNodeData, string linkName, string expectedEndNodeData) {
	return query(false, startNodeData, linkName, expectedEndNodeData);
}

bool SemanticNetwork::query(bool isQueryingWord, string startNodeData, string linkName, string expectedEndNodeData) {
	int32_t startNodeIndex = getNodeIndex(isQueryingWord, startNodeData);

	if (startNodeIndex != -1) {
		Node* startNode = nodes[startNodeIndex];

		return endingNodeExists(startNode, linkName, expectedEndNodeData);
	}
	else {
		sfout << Area("error") << "SemanticNetwork.query: starting node \"" + startNodeData + "\" not found" << "\n";
		sfout << Area("body");
		system("PAUSE");
		exit(1);
	}
}

bool SemanticNetwork::queryParentWordsInclusive(string startNodeData, string linkName, string expectedEndNodeData) {
	return findFirstParentNodeWithLinkInclusive(true, startNodeData, linkName, expectedEndNodeData);
}

bool SemanticNetwork::queryParentObjectsInclusive(string startNodeData, string linkName, string expectedEndNodeData) {
	return findFirstParentNodeWithLinkInclusive(false, startNodeData, linkName, expectedEndNodeData);
}

Node* SemanticNetwork::findFirstParentNodeWithLinkInclusive(bool isQueryingWord, string startNodeData, string linkName, string expectedEndNodeData) {
	int32_t startNodeIndex = getNodeIndex(isQueryingWord, startNodeData);

	if (startNodeIndex != -1) {
		Node*					startNode       = nodes[startNodeIndex];
		Link*					link			= startNode->getLink(linkName);
		Node::FieldGetter::Info fieldGetter     = startNode->getField(linkName);
		Link*					antiLink        = startNode->getLink("not_" + linkName);
		Node::FieldGetter::Info antiFieldGetter = startNode->getField("not_" + linkName);

		if (link != nullptr) {
			Node* endNode = findLinkedNode(link->getLinkedNodes(), expectedEndNodeData);

			if (endNode != nullptr) {
				return startNode;
			}
		}

		if (fieldGetter.wasSuccessful) {
			for (string value : strWhitespaceSplit(fieldGetter.simpleField, true)) {
				if (value == expectedEndNodeData) {
					return startNode;
				}
			}
		}

		if (antiLink != nullptr) {
			Node* endNode = findLinkedNode(antiLink->getLinkedNodes(), expectedEndNodeData);

			if (endNode != nullptr) {
				return nullptr;
			}
		}

		if (antiFieldGetter.wasSuccessful) {
			for (string value : strWhitespaceSplit(antiFieldGetter.simpleField, true)) {
				if (value == expectedEndNodeData) {
					return nullptr;
				}
			}
		}

		return rawFindFirstParentNodeWithLinkInclusive(isQueryingWord, startNode, linkName, expectedEndNodeData);
	}
	else {
		sfout << Area("error") << "SemanticNetwork.findFirstParentNodeWithLinkInclusive: starting node \"" + startNodeData + "\" not found" << "\n";
		sfout << Area("body");
		system("PAUSE");
		exit(1);
	}
}

Node* SemanticNetwork::rawFindFirstParentNodeWithLinkInclusive(bool isQueryingWord, Node* startNode, string linkName, string expectedEndNodeData) {
	vector<Node*> nodesToVisit;
	vector<Node*> parents = startNode->getLinkedNodes("is_a");

	for (auto &parentNode : parents) {
		nodesToVisit.push_back(parentNode);
	}

	while (!nodesToVisit.empty()) {
		for (auto &curNode : nodesToVisit) {
			Link* curLink = curNode->getLink(linkName);
			Link* antiLink = curNode->getLink("not_" + linkName);
			Node::FieldGetter::Info fieldGetter = curNode->getField(linkName);
			Node::FieldGetter::Info antiFieldGetter = curNode->getField("not_" + linkName);

			//	links
			if (curLink != nullptr) {
				for (Node* linkedNode : curLink->getLinkedNodes()) {
					if (linkedNode->getData() == expectedEndNodeData) {
						return curNode;
					}
				}
			}
			
			if (fieldGetter.wasSuccessful) {
				vector<string> values = strWhitespaceSplit(fieldGetter.simpleField, true);

				for (string value : values) {
					if (value == expectedEndNodeData) {
						return curNode;
					}
				}
			}


			//	antiLinks
			if (antiLink != nullptr) {
				for (Node* linkedNode : antiLink->getLinkedNodes()) {
					if (linkedNode->getData() == expectedEndNodeData) {
						return nullptr;
					}
				}
			}

			if (antiFieldGetter.wasSuccessful) {
				vector<string> values = strWhitespaceSplit(antiFieldGetter.simpleField, true);

				for (string value : values) {
					if (value == expectedEndNodeData) {
						return nullptr;
					}
				}
			}
		}

		vector<Node*> newNodesToVisit;

		for (auto &curNode : nodesToVisit) {
			Link* parentLink = curNode->getLink("is_a");

			if (parentLink != nullptr) {
				vector<Node*> parentNodes = parentLink->getLinkedNodes();

				for (auto &curParentNode : parentNodes) {
					newNodesToVisit.push_back(curParentNode);
				}
			}
		}

		nodesToVisit = newNodesToVisit;
	}

	return nullptr;
}

Node* SemanticNetwork::getWordNode(string nodeData) {
	return getNode(true, nodeData);
}

Node* SemanticNetwork::getObjectNode(string nodeData) {
	return getNode(false, nodeData);
}

Node* SemanticNetwork::getNode(bool isQueryingWord, string nodeData) {
	int32_t nodeIndex = getNodeIndex(isQueryingWord, nodeData);

	if (nodeIndex != -1) {
		return nodes[nodeIndex];
	}
	else {
		return nullptr;
	}
}

bool SemanticNetwork::endingNodeExists(Node* startNode, string linkName, string expectedEndNodeData) {
	if (linkName == "has_a") {
		return startNode->getField(expectedEndNodeData).wasSuccessful;
	}
	else {
		vector<Node*> linkedNodes = startNode->getLinkedNodes(linkName);

		if (!linkedNodes.empty()) {
			if (findLinkedNode(linkedNodes, expectedEndNodeData) != nullptr) {
				return true;
			}
		}
	}

	return false;
}

Node* SemanticNetwork::findLinkedNode(vector<Node*> linkedNodes, string expectedEndNodeData) {
	for (auto &it : linkedNodes) {
		if (it->getData() == expectedEndNodeData) {
			return it;
		}
	}

	return nullptr;
}

vector<Node*> SemanticNetwork::getWordNodeChildren(string parentNodeData) {
	return getNodeChildren(true, parentNodeData);
}

vector<Node*> SemanticNetwork::getObjectNodeChildren(string parentNodeData) {
	return getNodeChildren(false, parentNodeData);
}

vector<Node*> SemanticNetwork::getNodeChildren(bool isQueryingWord, string parentNodeData) {
	int32_t nodeIndex = getNodeIndex(isQueryingWord, parentNodeData);
	vector<Node*> children;

	if (nodeIndex != -1) {
		for (auto &it : nodes) {
			if (query(isQueryingWord, it->getData(), "is_a", parentNodeData)) {
				children.push_back(it);
			}
		}
	}

	return children;
}

vector<Node*> SemanticNetwork::getObjectNodes(string nodeType, string linkName, string endNodeData) {
	vector<Node*> nodesFound;

	for (Node* node : nodes) {
		Link* link = node->getLink(linkName);
		Node::FieldGetter::Info fieldGetter = node->getField(linkName);

		if (node->getData() == "lavn_throperoth") {
			int32_t stop = 0;
		}

		if (link != nullptr) {
			for (Node* linkedNode : link->getLinkedNodes()) {
				if (linkedNode->getData() == endNodeData) {
					if (queryParentObjectsInclusive(node->getData(), "is_a", nodeType)) {
						nodesFound.push_back(node);
						break;
					}
				}
			}
		}
		else if (fieldGetter.wasSuccessful) {
			vector<string> values = strWhitespaceSplit(fieldGetter.simpleField, true);
			vector<Node*> referencedNodes;

			for (string value : values) {
				Node* node = getNode(false, value);
				referencedNodes.push_back(node);
			}

			for (Node* fieldNode : referencedNodes) {
				if (fieldNode->getData() == endNodeData) {
					if (node->getData() == nodeType || queryParentObjectsInclusive(node->getData(), "is_a", nodeType)) {
						nodesFound.push_back(node);
						break;
					}
				}
			}
		}
	}

	return nodesFound;
}

void SemanticNetwork::printNetwork() {
	string output = "";

	for (auto node : nodes) {
		output += "\n" + nodeToString(node);
	}

	sfout << output;
}

string SemanticNetwork::nodeToString(Node* node) {
	vector<Link*> nodeLinks = node->getNodeLinks();
	string output = node->getData();
	unordered_map<string, string> simpleFields = node->getSimpleFields();
	unordered_map<string, Node*> nodeFields = node->getNodeFields();

	if (!simpleFields.empty()) {
		output += "\n    has_a -> ";

		for (auto &field : simpleFields) {
			output += field.first + " = \"" + field.second + "\", ";
		}
	}

	if (!nodeFields.empty()) {
		output += "\n    has_a ";

		for (auto &field : nodeFields) {
			output += field.first + "\n" +
				"        ";

			unordered_map<string, string> simpleFields2 = field.second->getSimpleFields();

			if (!simpleFields2.empty()) {
				output += "\n        has_a -> ";

				for (auto &field2 : simpleFields2) {
					output += field2.first + " = \"" + field2.second + "\", ";
				}
			}

			for (auto &nodeLink : field.second->getNodeLinks()) {
				output += "\n        " + nodeLink->getLinkName() + " -> ";

				for (auto &linkedNode : nodeLink->getLinkedNodes()) {
					output += linkedNode->getData() + ", ";
				}
			}
		}

		if (!nodeLinks.empty()) {
			//separate nodeFields from links
			output += "\n";
		}
	}

	for (auto &nodeLink : nodeLinks) {
		output += "\n    " + nodeLink->getLinkName() + " -> ";

		for (auto &linkedNode : nodeLink->getLinkedNodes()) {
			output += linkedNode->getData() + ", ";
		}
	}

	return output;
}

void SemanticNetwork::assimilateNodes(SemanticNetwork* otherNetwork) {
	for (auto &node : otherNetwork->nodes) {
		nodes.push_back(node);
	}
}