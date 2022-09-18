#pragma once

#include <string>
#include <stack>

using namespace std;

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

public:
	// Stolen from Aryan, thank you Aryan <3
	string convertInfixToPostFix(string infixString) {
		stack<char> st; // to store the ordering of the operators

		string postFixString;

		string::iterator it;

		for (it = infixString.begin(); it != infixString.end(); it++) {
			char c = *it;
			if (isalnum(c)) {
				postFixString += c;
			}
			else if (c == '(') {
				st.push(c);
			}
			else if (c == ')') { // get all the operands inside the brackets
				while (!st.empty() && st.top() != '(') {
					postFixString += st.top();
					st.pop();
				}
				st.pop(); // remove the ')'
			}
			else {
				while (!st.empty() && this->precedence(c) <= this->precedence(st.top())) {
					postFixString += st.top();
					st.pop();
				}
				st.push(c);
			}
		}

		while (!st.empty()) {
			postFixString += st.top();
			st.pop();
		}
		return postFixString;
	};
};