#pragma once

#include <string>
#include <stack>
#include <vector>
#include <list>

using namespace std;

const char STRING_DELIMITER_AS_CHAR = ' ';

class PostFixConverter {
private:
	/*
		Helper function to determine precedence of operator
	*/
	// refer to https://www.tutorialspoint.com/Convert-Infix-to-Postfix-Expression#:~:text=To%20convert%20infix%20expression%20to,maintaining%20the%20precedence%20of%20them.
	int precedence(char c) {
		if (c == '/' || c == '*' || c == '%') {
			return 2;
		}
		else if (c == '+' || c == '-') {
			return 1;
		}
		else {
			return 0;
		}
	}

	int precedence(string c) {
		if (c == "/"  || c == "*" || c == "%") {
			return 2;
		}
		else if (c == "+" || c == "-" ) {
			return 1;
		}
		else {
			return 0;
		}
	}

	vector<string> splitStringByDelimiter(string s, string delimiter) {
		size_t pos = 0;
		string token;
		vector<string> out;
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			if (token.size() != 0) {
				out.push_back(token);
			}
			s.erase(0, pos + delimiter.length());
		}
		out.push_back(s);
		return out;
	}

	bool isAlphanumeric(string s) {
		for (char c : s) {
			if (!isalnum(c)) {
				return false;
			}
		}
		return true;
	}

public:
	// Stolen from Aryan, thank you Aryan <3
	string convertInfixToPostFix(string infixString) {
		stack<string> st; // to store the ordering of the operators

		const string DELIMITER = " ";

		vector<string> postFixString;

		// split by space into strings
		vector<string> splitStrings = this->splitStringByDelimiter(infixString, DELIMITER);


		for (string token : splitStrings) {
			if (this->isAlphanumeric(token)) {
				postFixString.push_back(token);
			}
			else if (token == "(") {
				st.push(token);
			}
			else if (token == ")") { // get all the operands inside the brackets
				while (!st.empty() && st.top() != "(") {
					postFixString.push_back(st.top());
					st.pop();
				}
				st.pop(); // remove the ')'
			}
			else {
				while (!st.empty() && this->precedence(token) <= this->precedence(st.top())) {
					postFixString.push_back(st.top());
					st.pop();
				}
				st.push(token);
			}
		}

		while (!st.empty()) {
			postFixString.push_back(st.top());
			st.pop();
		}

		// convert back to a string, with a delimiter
		string out;
		for (int i = 0; i < postFixString.size(); i++) {
			string s = postFixString[i];
			out += s;
			if (i != postFixString.size() - 1) {
				out += DELIMITER;
			}
		}
		return out;
	};
};