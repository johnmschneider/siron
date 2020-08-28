#ifndef SFOUT_H
	#define SFOUT_H
	#include <stdint.h>
	#include <string>
	#include <unordered_map>
	#include <vector>
	#include <SFML\Graphics.hpp>

	namespace sfout_ {
		class Point {
			public:
				float x, y;

				Point();
				Point(float x, float y);
		};

		class Area {
			public:
				std::string areaName;

				Area();
				Area(std::string areaName);
		};

		class StreamInstruction {
			public:
				static const int32_t CLEAR;
				int32_t id;
		};

		class Clear : public StreamInstruction {
			public:
				Clear();
		};

		class Color {
			public:
				uint8_t r, g, b, a;
				bool isTextColor;

				Color();
				Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool isTextColor = true);
				Color(sf::Color, bool isTextColor = true);
		};

		class Instruction {
			public:
				Point position;
				std::string textToDraw;
				Color textColor;
				Color backgroundColor;
				
				Instruction();
				Instruction(Point position, std::string textToDraw, Color textColor, Color backgroundColor);
		};

		class SfOut {
			private:
				std::unordered_map<std::string, Instruction> instructions;
				std::string lastArea;
				void ensureAreaExists();

			public:
				SfOut();
				~SfOut();

				SfOut& operator<< (const std::string &newOutput);
				SfOut& operator<< (float output);
				SfOut& operator<< (char* output);
				SfOut& operator<< (char output);
				SfOut& operator<< (int output);
				SfOut& operator<< (size_t output);
				SfOut& operator<< (Area area);
				SfOut& operator<< (StreamInstruction instruction);
				SfOut& operator<< (Point position);
				SfOut& operator<< (Color textColor);

				std::unordered_map<std::string, Instruction> getInstructions();
				std::string getArea();
				std::string getCurrentAreaText();
		};

		extern SfOut sfout;
		extern Clear clear;
	}
#endif