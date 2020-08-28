#include <iostream>
#include <stdint.h>
#include "sfout.h"
#include "Main.h"

using namespace sfout_;
using std::string;
using std::to_string;
using std::unordered_map;

SfOut sfout_::sfout;
Clear sfout_::clear;

Point::Point() {

}

Point::Point(float x, float y) : x(x), y(y) {

}

Area::Area() {

}

Area::Area(string areaName) : areaName(areaName) {

}

Color::Color() {

}

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool isTextColor) : r(r), g(g), b(b), a(a), isTextColor(isTextColor) {
	
}

Color::Color(sf::Color color, bool isTextColor) : Color(color.r, color.g, color.b, color.a, isTextColor) {

}

Instruction::Instruction() {
	
}

Instruction::Instruction(Point position, string textToDraw, Color textColor, Color backgroundColor) :
	position(position), textToDraw(textToDraw), textColor(textColor), backgroundColor(backgroundColor) {

}

const int32_t StreamInstruction::CLEAR = 0;

Clear::Clear() {
	id = StreamInstruction::CLEAR;
}




SfOut::SfOut() {
	//the body is where the main content is
	lastArea = "body";
	instructions[lastArea] = Instruction(Point(10, 20), "", Color(255, 255, 255, 255), Color(0, 0, 0, 255));
}

SfOut::~SfOut() {

}

SfOut& SfOut::operator<< (const std::string &newOutput) {
	instructions[lastArea].textToDraw += newOutput;
	std::cout << newOutput;
	main_::renderSfOut();
	return *this;
}

SfOut& SfOut::operator<< (float output) {
	return ((*this) << to_string(output));
}

SfOut& SfOut::operator<< (char* output) {
	return((*this) << string(output));
}

SfOut& SfOut::operator<< (char output) {
	char rawStr[2] = { output, '\0' };
	return ((*this) << rawStr);
}

SfOut& SfOut::operator<< (int output) {
	return ((*this) << to_string(output));
}

SfOut& SfOut::operator<< (size_t output) {
	return ((*this) << to_string(output));
}

SfOut& SfOut::operator<< (Area area) {
	lastArea = area.areaName;
	ensureAreaExists();
	return *this;
}

SfOut& SfOut::operator<< (StreamInstruction instruction) {
	if (instruction.id == StreamInstruction::CLEAR) {
		instructions[lastArea].textToDraw = "";
	}

	return *this;
}

SfOut& SfOut::operator<< (Point position) {
	instructions[lastArea].position = position;

	return *this;
}

SfOut& SfOut::operator<< (Color color) {
	if (color.isTextColor) {
		instructions[lastArea].textColor = color;
	}
	else {
		instructions[lastArea].backgroundColor = color;
	}

	return *this;
}

void SfOut::ensureAreaExists() {
	if (instructions.find(lastArea) == instructions.end()) {
		instructions[lastArea] = Instruction(Point(0, 0), "", Color(255, 255, 255, 255), Color(0, 0, 0, 255));
	}
}

unordered_map<string, Instruction> SfOut::getInstructions() {
	return instructions;
}

string SfOut::getArea() {
	return lastArea;
}

string SfOut::getCurrentAreaText() {
	return instructions[lastArea].textToDraw;
}