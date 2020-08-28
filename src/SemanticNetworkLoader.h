#ifndef SEMANTIC_NETWORK_LOADER_H
	#define SEMANTIC_NETWORK_LOADER_H
	#include <string>
	#include <vector>
	#include "SemanticNetwork.h"

	namespace semanticNetwork {
		using string_vector = std::vector<std::string>;
		using string_vector_vector = std::vector<string_vector>;

		class NodeTemplate {
			public:
				std::string const node;
				string_vector_vector const links;
				string_vector_vector const simpleFields;
				string_vector_vector const nodeFields;

				NodeTemplate(std::string node, string_vector_vector links, string_vector_vector simpleFields, string_vector_vector nodeFields);
		};

		class LineHandlerArgs {
			public:
				std::string*						  line;
				SemanticNetwork*					  loadImportsInto;
				std::string							  thisAbsoluteFilePath;
				string_vector*						  nodesToAdd;
				std::vector<string_vector_vector>*	  linksToAdd;
				std::vector<string_vector_vector>*	  simpleFieldsToAdd;
				std::vector<string_vector_vector>*	  nodeFieldsToAdd;

				LineHandlerArgs();
		};

		class SemanticNetworkLoader {
			private:
				static std::vector<std::string> importedFiles;
				std::string baseDirectory;

				std::vector<NodeTemplate> parseNodeTemplateLanguage      (string_vector rawFileContents, SemanticNetwork &loadImportsInto, std::string parentFile);

				std::string				  getBaseDirectoryFromFilePath   (std::string filePath);
				std::string				  getAbsoluteFilePath			 (std::string relativeFilePath);

				void					  handleLines					 (string_vector rawFileContents, LineHandlerArgs& args);
				bool					  entireLineIsBlankOrCommentedOut(std::string* linePtr);
				bool					  commentWasEscaped				 (int32_t indexOfComment, std::string* line);
				void					  handleEscapedComment			 (int32_t indexOfComment, std::string* line);
				void					  handleLine					 (LineHandlerArgs& args);
				void					  handleImport					 (LineHandlerArgs& args);
				
				void					  handleNodeWithLinks			 (int32_t indexOfColon, LineHandlerArgs& args);
				void					  handleNodeWithoutLinks		 (int32_t indexOfSemicolon, LineHandlerArgs& args);
				
				void					  handleSettingOfField			 (int32_t indexOfDoubleQuote, LineHandlerArgs& args);
				void					  handleSettingOfSimpleField	 (LineHandlerArgs& args);
				void					  handleSettingOfComplexField	 (LineHandlerArgs& args);
				
				void					  handleFieldDeclaration		 (int32_t indexOfDoubleQuote, LineHandlerArgs& args);
				void					  handleComplexFieldDeclaration	 (LineHandlerArgs& args);
				void					  handleSimpleFieldDeclaration	 (LineHandlerArgs& args);

				void					  handleLink					 (LineHandlerArgs& args);

				void					  addLinksToNode				 (NodeTemplate& nodeTemplate, SemanticNetwork& loadInto);
				void					  addSimpleFieldsToNode			 (NodeTemplate& nodeTemplate, SemanticNetwork& loadInto);
				void					  addComplexFieldsToNode		 (NodeTemplate& nodeTemplate, SemanticNetwork& loadInto);

				std::vector<std::string>  loadFileIntoVector			 (std::string parentFile, std::string filePath);

			public:
				void loadFromFile(SemanticNetwork &loadInto, std::string parentFile, std::string absoluteFilePath);
		};
	}
#endif