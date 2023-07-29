#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <cmath>

// Pads [source] to [length] by adding spaces at the end.
std::string padRight(const std::string& source, int length) {
	int additionalSpacesLenght = length - source.length();
	if (additionalSpacesLenght < 0) {
		additionalSpacesLenght = 0;
	}
	return source + std::string(additionalSpacesLenght, ' ');
}

// Wraps a block of text into lines no longer than [length].
std::string wrapText(const std::string& text, int length, int hangingIndent = 0);

// Wraps a block of text into lines no longer than [length],
// starting at the [start] column, and returns the result as a list of strings.
std::vector<std::string> wrapTextAsLines(const std::string& text, int start = 0, int length = 0);

bool isWhitespace(const std::string& text, int index) {
	char rune = text[index];
	return rune >= 0x0009 && rune <= 0x000D ||
					rune == 0x0020 ||
					rune == 0x0085 ||
					rune == 0x1680 ||
					rune == 0x180E ||
					(rune >= 0x2000 && rune <= 0x200A) ||
					rune == 0x2028 ||
					rune == 0x2029 ||
					rune == 0x202F ||
					rune == 0x205F ||
					rune == 0x3000 ||
					rune == 0xFEFF;
}

std::string wrapText(const std::string& text, int length, int hangingIndent) {
	if (length == 0) return text;
	std::vector<std::string> splitText;
	size_t start = 0, end;
	while ((end = text.find('\n', start)) != std::string::npos) {
		splitText.push_back(text.substr(start, end - start));
		start = end + 1;
	}
	if (start < text.length()) {
		splitText.push_back(text.substr(start));
	}

	std::stringstream result;
	for (const auto& line : splitText) {
		size_t trimmedStart = line.find_first_not_of(' ');
		std::string trimmedText = trimmedStart == std::string::npos ? "" : line.substr(trimmedStart);
		std::string leadingWhitespace = trimmedStart == std::string::npos ? line : line.substr(0, trimmedStart);

		std::vector<std::string> notIndented;
		if (hangingIndent != 0) {
			size_t firstLineLength = length - leadingWhitespace.length();
			std::vector<std::string> firstLineWrap = wrapTextAsLines(trimmedText, 0, firstLineLength);
			notIndented.push_back(firstLineWrap[0]);
			trimmedText = trimmedText.substr(notIndented[0].length());
			if (firstLineWrap.size() > 1) {
				size_t otherLineLength = length - leadingWhitespace.length() - hangingIndent;
				std::vector<std::string> otherLineWrap = wrapTextAsLines(trimmedText, 0, otherLineLength);
				notIndented.insert(notIndented.end(), otherLineWrap.begin(), otherLineWrap.end());
			}
		} else {
			notIndented = wrapTextAsLines(trimmedText, 0, length - leadingWhitespace.length());
		}

		std::string hangingIndentString;
		for (const auto& line : notIndented) {
			if (line.find_first_not_of(' ') != std::string::npos) {
				std::string resultLine = hangingIndentString + leadingWhitespace + line;
				result << resultLine << "\n";
			}
			if (hangingIndentString.empty()) {
				hangingIndentString = std::string(hangingIndent, ' ');
			}
		}
	}

	return result.str();
}

std::vector<std::string> wrapTextAsLines(const std::string& text, int start, int length) {
	// assert(start >= 0);

	std::vector<std::string> result;
	int effectiveLength = std::max(length - start, 10);
	size_t currentLineStart = 0;
	int lastWhitespace = -1;
	for (size_t i = 0; i < text.length(); ++i) {
		if (isWhitespace(text, i)) lastWhitespace = i;

		if (i - currentLineStart >= effectiveLength) {
			if (lastWhitespace != -1) i = lastWhitespace;

			std::string line = text.substr(currentLineStart, i - currentLineStart);
			size_t trimmedEnd = line.find_last_not_of(' ');
			line = line.substr(0, trimmedEnd + 1);
			result.push_back(line);

			while (isWhitespace(text, i) && i < text.length()) {
				++i;
			}

			currentLineStart = i;
			lastWhitespace = -1;
		}
	}
	std::string lastLine = text.substr(currentLineStart);
	size_t trimmedEnd = lastLine.find_last_not_of(' ');
	lastLine = lastLine.substr(0, trimmedEnd + 1);
	result.push_back(lastLine);

	return result;
}
