#include "CGAParser.h"

namespace cga {

CGAExtrudeRule::CGAExtrudeRule(double height, const std::string& output_name) : height(height) {
	this->output_name = output_name;
}

std::string CGAExtrudeRule::to_string() {
	std::ostringstream oss;
	oss << "extrude(" << height << ") " << output_name;
	return oss.str();
}

CGAOffsetRule::CGAOffsetRule(double offsetDistance, const std::string& output_name) : offsetDistance(offsetDistance) {
	this->output_name = output_name;
}

std::string CGAOffsetRule::to_string() {
	std::ostringstream oss;
	oss << "offset(" << offsetDistance << ") " << output_name;
	return oss.str();
}

CGASplitRule::CGASplitRule(const std::string& splitAxis) : splitAxis(splitAxis) {
}

std::string CGASplitRule::to_string() {
	std::ostringstream oss;
	oss << "split(" << splitAxis << ")";
	return oss.str();
}

}
