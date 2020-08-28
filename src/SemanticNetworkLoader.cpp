#include <fstream>
#include <vector>
#include <array>
#include <stdint.h>
#include "SemanticNetworkLoader.h"
#include "Main.h"
#include "sfout.h"

using namespace semanticNetwork;
using std::string;
using std::vector;
using std::ios;
using std::fstream;
using std::array;
using main_::strWhitespaceSplit;
using main_::isValueInVector;
using sfout_::sfout;

NodeTemplate::NodeTemplate(string node, string_vector_vector links, string_vector_vector simpleFields, string_vector_vector nodeFields) :
	node(node), links(links), simpleFields(simpleFields), nodeFields(nodeFields) {

}




LineHandlerArgs::LineHandlerArgs() {

}




vector<string> SemanticNetworkLoader::importedFiles;

string SemanticNetworkLoader::getBaseDirectoryFromFilePath(string filePath) {
	int32_t lastBackSlash = filePath.find_last_of('\\');
	int32_t lastForwardSlash = filePath.find_last_of('/');

	return filePath.substr(0, std::max(lastBackSlash, lastForwardSlash) + 1);
}

string SemanticNetworkLoader::getAbsoluteFilePath(string relativeFilePath) {

	if (relativeFilePath.find(baseDirectory) != string::npos) {
		// filepath is already absolute
		return relativeFilePath;
	}

	int32_t directoryLevelsUp = 0;
	string curSearch = relativeFilePath;
	string curDirectory = baseDirectory;

	while (true) {
		size_t index = curSearch.find("..");

		if (index == string::npos) {
			break;
		}
		else {
			//add 3 (not 2) to account for backslash following ..
			curSearch = curSearch.substr(index + 3);
			directoryLevelsUp++;
		}
	}

	for (int32_t i = 0; i < directoryLevelsUp; i++) {
		curDirectory = curDirectory.substr(0, curDirectory.find_last_of('\\') + 1);
	}

	return curDirectory + curSearch;
}

vector<NodeTemplate> SemanticNetworkLoader::parseNodeTemplateLanguage(string_vector rawFileContents, SemanticNetwork &loadImportsInto, string thisAbsoluteFilePath) {

	vector<NodeTemplate> allNodeTemplates;

	string_vector nodesToAdd;
	vector<string_vector_vector> linksToAdd;
	vector<string_vector_vector> simpleFieldsToAdd;
	vector<string_vector_vector> nodeFieldsToAdd;
	LineHandlerArgs args;

	baseDirectory = getBaseDirectoryFromFilePath(thisAbsoluteFilePath);
	importedFiles.push_back(thisAbsoluteFilePath);

	args.nodesToAdd = &nodesToAdd;
	args.linksToAdd = &linksToAdd;
	args.simpleFieldsToAdd = &simpleFieldsToAdd;
	args.nodeFieldsToAdd = &nodeFieldsToAdd;
	args.loadImportsInto = &loadImportsInto;
	args.thisAbsoluteFilePath = thisAbsoluteFilePath;

	handleLines(rawFileContents, args);

	for (int32_t i = 0; i < nodesToAdd.size(); i++) {
		string				 node		  = nodesToAdd[i];
		string_vector_vector links		  = linksToAdd[i];
		string_vector_vector simpleFields = simpleFieldsToAdd[i];
		string_vector_vector nodeFields   = nodeFieldsToAdd[i];

		NodeTemplate nodeTemplate(node, links, simpleFields, nodeFields);
		allNodeTemplates.push_back(nodeTemplate);
	}

	return allNodeTemplates;
}

void SemanticNetworkLoader::handleLines(string_vector rawFileContents, LineHandlerArgs& args) {
	args.line = &string();

	for (int32_t i = 0; i < rawFileContents.size(); i++) {
		*args.line = rawFileContents[i];

		if (!entireLineIsBlankOrCommentedOut(args.line)) {
			int32_t indexOfComment = args.line->find_first_of('\'');

			if (indexOfComment != string::npos) {
				if (commentWasEscaped(indexOfComment, args.line)) {
					handleEscapedComment(indexOfComment, args.line);
				}
				else {
					*args.line = args.line->substr(0, indexOfComment - 1);
				}
			}

			if (!args.line->empty()) {
				handleLine(args);
			}
		}
	}
}

bool SemanticNetworkLoader::entireLineIsBlankOrCommentedOut(string* linePtr) {
	string line = *linePtr;

	return linePtr->empty() || line[0] == '\'';
}

bool SemanticNetworkLoader::commentWasEscaped(int32_t indexOfComment, string* line) {
	return indexOfComment > 0 && (*line)[indexOfComment - 1] == '\\';
}

void SemanticNetworkLoader::handleEscapedComment(int32_t indexOfComment, string* line) {
	if (indexOfComment - 2 > 0) {
		*line = line->substr(0, indexOfComment - 1) + line->substr(indexOfComment, line->length());
	}
	else {
		*line = line->substr(1);
	}
}

void SemanticNetworkLoader::handleLine(LineHandlerArgs& args) {
	int32_t length;
	char char_;
	int32_t indexOfImport;
	int32_t indexOfColon;
	int32_t indexOfSemicolon;
	int32_t indexOfEquals;
	int32_t indexOfDoubleQuote;
	int32_t indexOfHasA;

	length				= args.line->length();
	char_				= (*args.line)[length - 1];
	indexOfImport		= args.line->find(R"(import)");
	indexOfColon		= args.line->find(R"(:)");
	indexOfSemicolon	= (indexOfColon != string::npos ? string::npos : args.line->find(R"(;)"));
	indexOfEquals		= (indexOfSemicolon != string::npos ? string::npos : args.line->find(R"(=)"));
	indexOfDoubleQuote	= args.line->find(R"(")");
	indexOfHasA			= args.line->find(R"(has_a)");

	if (indexOfImport != string::npos) {
		handleImport(args);
	}
	else if (indexOfColon != string::npos) {
		handleNodeWithLinks(indexOfColon, args);
	}
	else if (indexOfSemicolon != string::npos) {
		handleNodeWithoutLinks(indexOfSemicolon, args);
	}
	else if (indexOfEquals != string::npos) {
		handleSettingOfField(indexOfDoubleQuote, args);
	}
	else if (indexOfHasA != string::npos) {
		handleFieldDeclaration(indexOfDoubleQuote, args);
	}
	else {
		handleLink(args);
	}
}

void SemanticNetworkLoader::handleImport(LineHandlerArgs& args) {
	SemanticNetworkLoader importedFileLoader;
	string relativeFilePath;
	string absoluteFilePath;

	relativeFilePath = strWhitespaceSplit(*args.line, true)[1];
	absoluteFilePath = getAbsoluteFilePath(relativeFilePath);

	if (isValueInVector(importedFiles, absoluteFilePath) == false) {
		importedFileLoader.loadFromFile(*args.loadImportsInto, args.thisAbsoluteFilePath, absoluteFilePath);
	}
}

void SemanticNetworkLoader::handleNodeWithLinks(int32_t indexOfColon, LineHandlerArgs& args) {
	args.nodesToAdd			->push_back(args.line->substr(0, indexOfColon));
	args.linksToAdd			->push_back(string_vector_vector());
	args.simpleFieldsToAdd	->push_back(string_vector_vector());
	args.nodeFieldsToAdd	->push_back(string_vector_vector());
}

void SemanticNetworkLoader::handleNodeWithoutLinks(int32_t indexOfSemicolon, LineHandlerArgs& args) {
	args.nodesToAdd			->push_back(args.line->substr(0, indexOfSemicolon));
	args.linksToAdd			->push_back(string_vector_vector());
	args.simpleFieldsToAdd	->push_back(string_vector_vector());
	args.nodeFieldsToAdd	->push_back(string_vector_vector());
}

void SemanticNetworkLoader::handleSettingOfField(int32_t indexOfDoubleQuote, LineHandlerArgs& args) {
	if (indexOfDoubleQuote == string::npos) {
		handleSettingOfComplexField(args);
	}
	else {
		handleSettingOfSimpleField(args);
	}
}

void SemanticNetworkLoader::handleSettingOfSimpleField(LineHandlerArgs& args) {
	vector<string> tokens = strWhitespaceSplit(*args.line, true);
	string fieldName = tokens[0];
	string fieldValue = tokens[2];

	int32_t curNode = args.nodesToAdd->size() - 1;

	(*args.simpleFieldsToAdd)[curNode].push_back(string_vector{ fieldName, fieldValue });
}

void SemanticNetworkLoader::handleSettingOfComplexField(LineHandlerArgs& args) {
	vector<string> tokens = strWhitespaceSplit(*args.line, true);
	string fieldName	  = tokens[0];
	vector<string>			fieldValues;

	int32_t curNode  = args.nodesToAdd->size() - 1;
	int32_t curField = (*args.nodeFieldsToAdd)[curNode].size();

	(*args.nodeFieldsToAdd)[curNode].push_back(string_vector{ fieldName, tokens[2] });

	for (int32_t i2 = 3; i2 < tokens.size(); i2++) {
		(*args.nodeFieldsToAdd)[curNode][curField].push_back(tokens[i2]);
	}
}

void SemanticNetworkLoader::handleFieldDeclaration(int32_t indexOfDoubleQuote, LineHandlerArgs& args) {
	if (indexOfDoubleQuote == string::npos) {
		handleComplexFieldDeclaration(args);
	}
	else {
		handleSimpleFieldDeclaration(args);
	}
}

void SemanticNetworkLoader::handleComplexFieldDeclaration(LineHandlerArgs& args) {
	vector<string> tokens = strWhitespaceSplit(*args.line, true);

	int32_t curNode = args.nodesToAdd->size() - 1;
	int32_t curField = (*args.nodeFieldsToAdd)[curNode].size();

	for (int32_t i2 = 1; i2 < tokens.size(); i2++) {
		(*args.nodeFieldsToAdd)[curNode].push_back(string_vector{ tokens[i2], tokens[i2] });
	}
}

void SemanticNetworkLoader::handleSimpleFieldDeclaration(LineHandlerArgs& args) {
	vector<string> tokens = strWhitespaceSplit(*args.line, true);
	vector<string> fields = strWhitespaceSplit(tokens[1]);

	int32_t curNode = args.nodesToAdd->size() - 1;

	for (string field : fields) {
		(*args.simpleFieldsToAdd)[curNode].push_back(string_vector{ field, Node::NULL_SIMPLE_FIELD });
	}
}

void SemanticNetworkLoader::handleLink(LineHandlerArgs& args) {
	string_vector curLink = strWhitespaceSplit(*args.line);

	if (!curLink.empty()) {
		(*args.linksToAdd)[args.nodesToAdd->size() - 1].push_back(curLink);
	}
}

void SemanticNetworkLoader::loadFromFile(SemanticNetwork &loadInto, string parentFile, string absoluteFilePath) {
	
	vector<string> rawFileContents = loadFileIntoVector(parentFile, absoluteFilePath);
	vector<NodeTemplate> nodes;

	nodes = parseNodeTemplateLanguage(rawFileContents, loadInto, absoluteFilePath);
		
	//add all object nodes now so we can link nodes that were added later on 
	//	without a forward reference
	for (NodeTemplate nodeTemplate : nodes) {
		loadInto.addObjectNode(nodeTemplate.node);
	}
		
	for (NodeTemplate nodeTemplate : nodes) {
		addLinksToNode		  (nodeTemplate, loadInto);
		addSimpleFieldsToNode (nodeTemplate, loadInto);
		addComplexFieldsToNode(nodeTemplate, loadInto);
	}
}

vector<string> SemanticNetworkLoader::loadFileIntoVector(string parentFile, string filePath) {
	fstream file;

	file.open(filePath, ios::in);

	if (file.is_open()) {
		string line;
		vector<string> rawFileContents;

		while (getline(file, line)) {
			rawFileContents.push_back(line);
		}

		file.close();
		return rawFileContents;
	}
	else {
		sfout << sfout_::Area("error") << "SemanticNetworkLoader::loadFileIntoVector:" << "\n" << "\t in file \"" << parentFile << "\"" << "\n\n" <<
			"\tcould not open file \"" << filePath << "\"" << "\n";

		system("PAUSE");
		exit(1);
	}
}

void SemanticNetworkLoader::addLinksToNode(NodeTemplate& nodeTemplate, SemanticNetwork& loadInto) {
	for (string_vector link : nodeTemplate.links) {
		for (int32_t i = 1; i < link.size(); i++) {
			loadInto.addObjectLink(nodeTemplate.node, link[0], link[i]);
		}
	}
}

void SemanticNetworkLoader::addSimpleFieldsToNode(NodeTemplate& nodeTemplate, SemanticNetwork& loadInto) {
	for (string_vector field : nodeTemplate.simpleFields) {
		if (!field.empty()) {
			loadInto.getObjectNode(nodeTemplate.node)->setSimpleField(field[0], field[1]);
		}
	}
}

void SemanticNetworkLoader::addComplexFieldsToNode(NodeTemplate& nodeTemplate, SemanticNetwork& loadInto) {
	for (string_vector field : nodeTemplate.nodeFields) {
		string rawName = field[0];
		string newNodeName = nodeTemplate.node + "." + rawName;

		loadInto.addObjectNode(newNodeName);
		loadInto.addObjectLink(newNodeName, "is_a", rawName);
		loadInto.getObjectNode(nodeTemplate.node)->setNodeField(newNodeName, loadInto.getObjectNode(newNodeName));
	}
}