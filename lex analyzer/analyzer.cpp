#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
// List of all keywords in the language
const std::string keywords[] = { "bool", "int", "if", "then", "else", "end", "program", "while", "print", "not", "false", "true", "mod", "and", "or", "do" };


class LexicalAnalyzer {
private:
	// Global file stream used to read input
	std::ifstream file;

	// Stores the current character being processed
	char currentChar;

	// Token structure storing position, type, and value
	struct Token {
		int row, col;
		std::string lexeme, kind, value;
	};

	// Track current position in file
	int currentRow = 1;
	int currentCol = 1;

	// Stores the current token recognized by the lexer
protected:
	Token currentToken;
public:

	LexicalAnalyzer(const std::string& filepath) {
		file.open(filepath);
		if (!file.is_open()) {
			throw std::runtime_error("COULD NOT FIND FILE: " + filepath);
		}
	}

	// Checks whether a given word is a keyword
	bool isKeyWord(const std::string& word) {
		for (const std::string& s : keywords) {
			if (s == word) return true;
		}
		return false;
	}

	// Sets the current token based on the lexeme type
	void setCurrentToken(const std::string& lexeme, int tokenRow, int tokenCol) {
		// If lexeme is a keyword, its kind is the keyword itself
		if (isKeyWord(lexeme)) {
			currentToken = { tokenRow, tokenCol, lexeme, lexeme, "" };
		}
		// If lexeme starts with digit, it is a number
		else if (isdigit(lexeme[0])) {
			currentToken = { tokenRow, tokenCol, lexeme, "NUM", lexeme };
		}
		// If lexeme starts with letter, it is an identifier
		else if (isalpha(lexeme[0])) {
			currentToken = { tokenRow, tokenCol, lexeme, "ID", lexeme };
		}
	}

	// Reads the next token from the input file
	void next() {

		std::string lexeme;

		// Read character by character from file
		while (file.get(currentChar)) {

			// Store starting position of token
			int tokenRow = currentRow;
			int tokenCol = currentCol;

			// Handle newline
			if (currentChar == '\n') {
				currentCol = 1;
				++currentRow;
				continue;
			}

			// Skip whitespace
			if (isspace(currentChar)) {
				++currentCol;
				continue;
			}

			// Handle comments starting with //
			if (currentChar == '/' && file.peek() == '/') {
				std::string discard;
				std::getline(file, discard);
				currentCol = 1;
				++currentRow;
				continue;
			}

			// Handle numbers (integer literals)
			if (isdigit(currentChar)) {
				lexeme += currentChar;
				++currentCol;

				while (isdigit(file.peek())) {
					file.get(currentChar);
					lexeme += currentChar;
					++currentCol;
				}
				setCurrentToken(lexeme, tokenRow, tokenCol);
				return;
			}

			// Handle identifiers and keywords
			else if (isalpha(currentChar)) {
				lexeme += currentChar;
				++currentCol;

				while (isdigit(file.peek()) || isalpha(file.peek()) || file.peek() == '_') {
					file.get(currentChar);
					lexeme += currentChar;
					++currentCol;
				}

				setCurrentToken(lexeme, tokenRow, tokenCol);
				return;
			}

			// Handle operators and special symbols
			else {
				switch (currentChar) {

					// Relational operator handling
				case '=':
					++currentCol;
					if (file.peek() == '<') {
						file.get(currentChar);
						++currentCol;
						currentToken = { tokenRow, tokenCol, "=<", "" };
					}
					else {
						currentToken = { tokenRow, tokenCol, "=", "" };
					}
					return;

				case '>':
					++currentCol;
					if (file.peek() == '=') {
						file.get(currentChar);
						++currentCol;
						currentToken = { tokenRow, tokenCol, ">=", "" };
					}
					else {
						currentToken = { tokenRow, tokenCol, ">", "" };
					}
					return;

				case ':':
					++currentCol;
					if (file.peek() == '=') {
						file.get(currentChar);
						++currentCol;
						currentToken = { tokenRow, tokenCol, ":=", "" };
					}
					else {
						currentToken = { tokenRow, tokenCol, ":", "" };
					}
					return;

				case '!':
					++currentCol;
					if (file.peek() == '=') {
						file.get(currentChar);
						++currentCol;
						currentToken = { tokenRow, tokenCol, "!=", "" };
						return;
					}
					else {
						std::cerr << "ERROR on line " << "(" << tokenRow << ", " << tokenCol << ")" << " lexical analyzer does not recognize !\n";
						exit(0);
					}
					return;

					// Single-character tokens
				case '-': currentCol++; currentToken = { tokenRow, tokenCol, "-", "-", "" }; return;
				case '+': currentCol++; currentToken = { tokenRow, tokenCol, "+", "+", "" }; return;
				case '.': currentCol++; currentToken = { tokenRow, tokenCol, ".", ".", "" }; return;
				case ';': currentCol++; currentToken = { tokenRow, tokenCol, ";", ";" "" }; return;
				case '/': currentCol++; currentToken = { tokenRow, tokenCol, "/", "/", "" }; return;
				case '*': currentCol++; currentToken = { tokenRow, tokenCol, "*", "*", "" }; return;
				case '<': currentCol++; currentToken = { tokenRow, tokenCol, "<", "<", "" }; return;
				case ',': currentCol++; currentToken = { tokenRow, tokenCol, ",", ",", "" }; return;
				case '(': currentCol++; currentToken = { tokenRow, tokenCol, "(", "(", "" }; return;
				case ')': currentCol++; currentToken = { tokenRow, tokenCol, ")", ")", "" }; return;

					// Handle invalid characters
				default:
					std::cerr << "ERROR on line (" << tokenRow << ", " << tokenCol << ")" << " Invalid character spotted: " << currentChar << '\n';
					exit(0);
				}
			}
		}

		// End-of-file token
		currentToken = { currentRow, currentCol, "end-of-text", "" };
	}

	// Returns token position as string
	std::string position() { return "(" + std::to_string(currentToken.row) + ", " + std::to_string(currentToken.col) + ")"; }

	// Returns token kind
	std::string kind() { return currentToken.kind; }

	// Returns token value
	std::string value() { return currentToken.value; }


};
class Parser : LexicalAnalyzer {


public:

	Parser(const std::string& filepath) : LexicalAnalyzer(filepath) {
		next();
		program();

	}

	void program() {
		match("program");
		identifier();
		match(":");
		body();
		match(".");

		std::cout << "SUCCESS! Code syntactically Correct!\n";
	}

	void identifier() {
		if (currentToken.kind == "ID") {
			next();
			return;
		}

		report("**identifier** e.g name, total, PI");
	}

	void body() {
		declarations();
		statements();
	}
	void declarations() {
		if (currentToken.lexeme == "bool" || currentToken.lexeme == "int") {
			next();
			if (currentToken.kind != "ID") report("**identifier** e.g int a, bool a");
			declaration();
		}
		/*	else {
				report("**declaration type** e.g int, bool");
			}*/

	}

	void declaration() {

		while (currentToken.kind == "ID") {
			next();
			if (currentToken.lexeme != ",") break;
			next();
		}
		match(";");

		if (currentToken.lexeme == "bool" || currentToken.lexeme == "int") {
			next();
			if (currentToken.kind != "ID") report("**identifier** e.g int a, bool a");
			declaration();
		}
	}

	void statements() {
		if (currentToken.lexeme == "print") {
			next();
			expression();
		}
		else if (currentToken.kind == "ID") {
			next();
			match(":=");
			expression();
		}
		else if (currentToken.lexeme == "if") {
			next();
			expression();
			match("then");
			body();

			if (currentToken.lexeme == "else") {
				next();
				body();
			}
			match("end");
		}

		else if (currentToken.lexeme == "while") {
			next();
			expression();
			match("do");
			body();
			match("end");
		}
		else {
			report("**statement** such as as assignment, conditional, iterative, or print statement");
		}
	}

	void expression() {
		simpleExpression();
		if (currentToken.lexeme == "<" || currentToken.lexeme == "=<"
			|| currentToken.lexeme == "=" || currentToken.lexeme == "!="
			|| currentToken.lexeme == ">=" || currentToken.lexeme == ">") {
			next();
			simpleExpression();
		}
	}

	void simpleExpression() {
		term();
		while (currentToken.lexeme == "+" || currentToken.lexeme == "-"
			|| currentToken.lexeme == "or") {
			next();
			term();
		}
	}

	void term() {
		factor();
		while (currentToken.lexeme == "*" || currentToken.lexeme == "/"
			|| currentToken.lexeme == "mod" || currentToken.lexeme == "and") {
			next();
			factor();
		}
	}
	void factor() {
		if (currentToken.lexeme == "-" || currentToken.lexeme == "not") {
			next();
		}

		if (currentToken.kind == "ID" || currentToken.lexeme == "false" || currentToken.lexeme == "true" || currentToken.kind == "NUM") {
			next();
			return;
		}
		if (currentToken.lexeme == "(") {
			next();
			expression();
			match(")");
			next();
			return;
		}

		report("**expression** e.g -total, not total, 3.14");
	}
	void match(const std::string& s) {
		if (currentToken.lexeme == s) {
			next();
			return;
		}

		report(s);
	}

	void report(const std::string& s) {

		std::cerr << "Syntax Error on line (" << currentToken.row << ", " << currentToken.col << ") " <<
			"expected: " << s << '\n' << currentToken.lexeme << "\n^";
		exit(0);
	}
};
// Main function to simulate parser behavior


int main() {
	//DO NOT HARDCODE TEXT FILE
		//DO NOT HARDCODE TEXT FILE
		//DO NOT HARDCODE TEXT FILE
		//DO NOT HARDCODE TEXT FILE
		//DO NOT HARDCODE TEXT FILE
	Parser parser("test.txt");

	//std::string filepath;

	//// Prompt user for input file name
	//std::cout << "Enter input file name: ";
	//std::cin >> filepath;

	//file.open(filepath);

	//if (!file.is_open()) {
	//	std::cerr << "ERROR: Could not open file: " << filepath << "\n";
	//	return -1;
	//}

	//// Read first token
	//next();
	//std::cout << position() << " " << kind() << " " << value() << "\n";

	//// Continue until end-of-text
	//while (kind() != "end-of-text") {
	//	next();
	//	std::cout << position() << " " << kind() << " " << value() << "\n";
	//}

	return 0;
}