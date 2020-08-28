#ifndef SEMANTIC_NETWORK_H
	#define SEMANTIC_NETWORK_H
	#include <vector>
	#include <string>
	#include <unordered_map>

	namespace semanticNetwork {
		using linkmap = std::unordered_map<std::string, std::vector<std::string>>;
		using linkvec = std::vector<std::string>;

		class Link;

		class Node {
			private:
				std::string data;
				bool isThisWordNode;
				std::vector<Link*> links;
				std::unordered_map<std::string, std::string> simpleFields;
				std::unordered_map<std::string, Node*> nodeFields;

			public:
				class FieldGetter {
					public:
						class Info {
							public:
								bool const wasSuccessful;
								std::string const simpleField;
								Node* nodeField;

								Info(bool wasSuccessful, std::string simpleField, Node* nodeField) : wasSuccessful(wasSuccessful), simpleField(simpleField),
									nodeField(nodeField) {

								}
						};
				};

				Node(std::string simpleFields, bool isWordNode, std::vector<Link*> linkedNodes);
				Node(std::string simpleFields, bool isWordNode);

				~Node();

				static std::string const NULL_SIMPLE_FIELD;

				std::string getData();

				void addLink(std::string linkName, Node* nodeToLinkTo);
				Link* getLink(std::string linkName);

				std::vector<Link*> getNodeLinks();

				std::vector<Node*> getLinkedNodes(std::string linkName);

				bool isWordNode();

				std::unordered_map<std::string, std::string> getSimpleFields();
				std::unordered_map<std::string, Node*> getNodeFields();

				FieldGetter::Info getField(std::string fieldName);
				void setSimpleField(std::string fieldName, std::string value);
				void setNodeField(std::string fieldName, Node* value);
		};

		/**
		* A link is one-to-many (for instance, is_a is in relation to one node, but can have multiple answers)
		*/
		class Link {
			private:
				std::string linkName;
				std::vector<Node*> linkedNodes;

			public:
				Link(std::string linkName, std::vector<Node*> links);
				Link(std::string linkName);
				~Link();

				std::string getLinkName();
				std::vector<Node*> getLinkedNodes();

				void addLinkedNode(Node* nodeToLink);
		};

		class SemanticNetwork {
			private:
				std::vector<Node*> nodes;

				void addNode(std::string nodeData, linkmap nodeLinks, bool isWordNode);
				void addLink(bool isWord, std::string startNodeData, std::string linkName, std::string endNodeData);
				void addLinkWithValidStartNode(bool isWord, Node* startNode, std::string linkName, std::string endNodeData);
				
				bool query(bool isQueryingWord, std::string startNodeData, std::string linkName, std::string expectedEndNodeData);
				Node* findFirstParentNodeWithLinkInclusive(bool isQueryingWord, std::string startNodeData, std::string linkName, std::string expectedEndNodeData);
				Node* rawFindFirstParentNodeWithLinkInclusive(bool isQueryingWord, Node* startNode, std::string linkName, std::string expectedEndNodeData);

				int getNodeIndex(bool isQueryingWord, std::string nodeData);
				Node* getNode(bool isQueryingWord, std::string nodeData);
				bool endingNodeExists(Node* startNode, std::string linkName, std::string expectedEndNodeData);
				Node* findLinkedNode(std::vector<Node*> linkedNodes, std::string expectedEndNodeData);

				std::vector<Node*> getNodeChildren(bool isQueryingWord, std::string parentNodeData);

			public:
				SemanticNetwork();
				~SemanticNetwork();

				void addWordNode(std::string nodeData, linkmap nodeLinks);
				void addWordNode(std::string nodeData);
				void addObjectNode(std::string nodeData, linkmap nodeLinks);
				void addObjectNode(std::string nodeData);

				void addWordLink(std::string startNodeData, std::string linkName, std::string endNodeData);
				void addObjectLink(std::string startNodeData, std::string linkName, std::string endNodeData);

				bool queryWord(std::string startNodeData, std::string linkName, std::string exceptedEndNodeData);
				bool queryObject(std::string startNodeData, std::string linkName, std::string exceptedEndNodeData);

				bool queryParentWordsInclusive(std::string startNodeData, std::string linkName, std::string expectedEndNodeData);
				bool queryParentObjectsInclusive(std::string startNodeData, std::string linkName, std::string expectedEndNodeData);

				Node* getWordNode(std::string nodeData);
				Node* getObjectNode(std::string nodeData);

				std::vector<Node*> getWordNodeChildren(std::string parentNodeData);
				std::vector<Node*> getObjectNodeChildren(std::string parentNodeData);

				std::vector<Node*> getObjectNodes(std::string nodeType, std::string linkName, std::string endNodeData);

				void printNetwork();
				std::string nodeToString(Node* node);
				void assimilateNodes(SemanticNetwork* otherNetwork);
		};
	}

#endif