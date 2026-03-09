#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

std::ifstream file;
char currentChar;
struct Token {
	int row, col;
	std::string kind, value;
};

int currentRow = 1;
int currentCol = 1 ;
const std::string keywords[] = { "bool", "int", "if", "then", "else", "end", "program", "while", "print", "not", "false", "true", "mod", "and", "or", "do"};

Token currentToken;
bool isKeyWord(const std::string& word) {
	for (std::string s : keywords) {
		if (s == word) return true;
	}
	return false;
}


void setCurrentToken(const std::string& lexeme, int tokenRow, int tokenCol) {
	if (lexeme == "end_of_text") {
		currentToken = { tokenRow, tokenCol, lexeme, "__" };
	}
	else if (isKeyWord(lexeme)) {
		currentToken = { tokenRow, tokenCol, lexeme, "__" };
	}
	else if (isdigit(lexeme[0])) {
		currentToken = { tokenRow, tokenCol, "NUM", lexeme };
	}
	else if (isalpha(lexeme[0])) {
		currentToken = { tokenRow, tokenCol, "ID", lexeme };
	}
	

}
void next() {
	std::string lexeme;
	
	while (file.get(currentChar)) {
		int tokenRow = currentRow;
		int tokenCol = currentCol;

		if (currentChar == '\n') {
			currentCol = 1;
			++currentRow;
			continue;
		}




		if (isspace(currentChar)) {
			++currentCol;
			continue;
		}

		if (currentChar == '/' && file.peek() == '/') {
			std::string discard;
			std::getline(file, discard);
			currentCol = 1;
			++currentRow;
			continue;
		}


		if (isdigit(currentChar)) {
			lexeme += currentChar;
			while (isdigit(file.peek())) {
				file.get(currentChar);
				lexeme += currentChar;
				++currentCol;
			}
			setCurrentToken(lexeme, tokenRow, tokenCol);
			break;
		}

		else if (isalpha(currentChar) || currentChar == '_') {
			lexeme += currentChar;

			while (isdigit(file.peek())  || isalpha(file.peek()) || file.peek() == '_') {
				file.get(currentChar);
				lexeme += currentChar;
				++currentCol;
			}

			setCurrentToken(lexeme, tokenRow, tokenCol);
			break;
		}



		else {
			switch (currentChar) {
				case '=':
					if (file.peek() == '<') {
						file.get(currentChar);
						++currentCol;
						currentToken = { tokenRow, tokenCol, "=<", "__" };
						break;
					}

					currentToken = { tokenRow, tokenCol, "=", "__" };
					break;

				case '>':
					if (file.peek() == '=') {
						file.get(currentChar);
						++currentCol;
						currentToken = { tokenRow, tokenCol, ">=", "__" };
						break;
					}

					currentToken = { tokenRow, tokenCol, ">", "__" };
					break;
				case ':':
					if (file.peek() == '=') {
						file.get(currentChar);
						++currentCol;
						currentToken = { tokenRow, tokenCol, ":=", "__" };
						break;
					}

					currentToken = { tokenRow, tokenCol, ":", "__" };
					break;
				case '!':
					if (file.peek() == '=') {
						file.get(currentChar);
						++currentCol;
						currentToken = { tokenRow, tokenCol, "!=", "__" };
						break;
					}
					std::cerr << "ERROR at line " << "(" << tokenRow << ", " << tokenCol << ")" << " lexical analyzer does not recognize !\n";
					exit(0);
					break;


				case '-': currentToken = { tokenRow, tokenCol, "-", "__" }; break;
				case '+': currentToken = { tokenRow, tokenCol, "+", "__" }; break;
				case '.': currentToken = { tokenRow, tokenCol, ".", "__" }; break;
				case ';': currentToken = { tokenRow, tokenCol, ";", "__" }; break;
				case '/': currentToken = { tokenRow, tokenCol, "/", "__" }; break;
				case '*': currentToken = { tokenRow, tokenCol, "*", "__" }; break;
				case ',': currentToken = { tokenRow, tokenCol, ",", "__" }; break;
				case '(': currentToken = { tokenRow, tokenCol, "(", "__" }; break;
				case ')': currentToken = { tokenRow, tokenCol, ")", "__" }; break;

				default:
					std::cerr << "ERROR at line (" << currentRow << ", " << currentCol << ")" << " Invalid character spotted: " << currentChar << '\n';
					exit(0);
 			}
			break;
		}

		++currentCol;
	}

	++currentCol;

}


std::string position() {return "(" + std::to_string(currentToken.row) + ", " + std::to_string(currentToken.col) + ")"; }
std::string kind() { return currentToken.kind; }
std::string value() { return currentToken.value; }

int main() {
	std::string filepath;
	//std::cout << "Enter file to read: ";
	//std::cin >> filepath;
	file.open("test.txt");

	if (!file.is_open()) {
		std::cerr << "NOT OPENED\n";
		return -1;
	}



	next();
	std::cout << position() << " " << kind() << " " << value() << "\n";
	while (kind() != "end_of_text") {
		next();
		std::cout << position() << " " << kind() << " " << value() << "\n";

	}
	return 0;
}