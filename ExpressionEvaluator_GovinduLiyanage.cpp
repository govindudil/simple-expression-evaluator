//
// Created by Govindu Liyanage on 3/15/2023.
//
#include <iostream>
#include <stack>
#include <string>
#include <cctype>

using namespace std;

/*
    This function takes a character as input and returns an integer value that represents the precedence of the operator.
    * Input
        * op - the operator as a character
    * Output
        * integer with precedence value of the operator

    According to the task, the precedence values are set such that the operators are evaluated from left to right.
    This function can be updated as shown below if normal maths rules needs to be followed.
        if (op == '+' || op == '-' || op == '*' || op == '/') {
            return 1;
        } else if (op == '*' || op == '/') {
            return 2;
        } else {
            return 0;
        }
 */
int precedence(char op) {
    // Check if the given operator is one of the four basic arithmetic operators (+, -, *, /)
    if (op == '+' || op == '-' || op == '*' || op == '/') {
        return 1;   // If the operator matches any of these, return 1 to indicate that the operator has a higher precedence
    } else {
        return 0;   // Otherwise, return 0 to indicate that the operator has a lower or equal precedence
    }
}

/*
    This function takes a reference to a stack of integers and a character as input, and returns a boolean value.
    * Input
        * values - reference to a stack of integers
        * op - the operator as a character
    * Output
        * bool stating the calculation was successfully or not
 */
bool calculate(stack<int>& values, char op) {
    // Check if there are at least two elements in the stack
    if (values.size() < 2) {
        return false; // If there are not, return false
    }

    // Pop the top two values off the stack and store them in val2 and val1, respectively
    int val2 = values.top();
    values.pop();
    int val1 = values.top();
    values.pop();

    // Perform the appropriate calculation based on the given operator and store the result in the variable named "result"
    int result;
    if (op == '+') {
        result = val1 + val2;
    } else if (op == '-') {
        result = val1 - val2;
    } else if (op == '*') {
        result = val1 * val2;
    } else if (op == '/' ) {
        if (val2 == 0) {    // Check if the second value is 0 to avoid a divide-by-zero error
            return false;
        }
        result = val1 / val2;
    } else {
        return false;   // If the operator is invalid, return false
    }

    // Push the result back onto the stack, and return true to indicate that the calculation was successful
    values.push(result);
    return true;
}

/*
    This function takes a pointer to a string of characters, a reference to an empty string, and an integer counter as input, and returns a boolean value.
    * Input
        * expression - pointer to a string of characters
        * line - reference to an empty string where the extracted token will be stored
        * counter - integer counter variable to track the number of characters extracted
    * Output
        * bool stating whether a token was successfully extracted or not
*/
bool extractTokens(const char *expression, std::string& line, int& counter) {
    bool flag = false;
    counter = 0; // Initialize the counter to 0.

    string oldToken = line; // Store the previous token for validation purposes.
    line.clear(); // Clear the current string to remove any previous content.

    // Skip any initial white spaces.
    while (*expression && isspace(*expression)) {
        ++expression;
        counter++;
    }

    // Handle the '+' and '-' operators separately as they can represent positive or negative signs.
    if (*expression == '+' || *expression == '-') {
        line.push_back(*expression); // Add the operator to the string.
        ++expression;
        counter++;

        // Check if the previous token was an operator, if not return true to indicate a valid operator token.
        if (oldToken.length() > 1 || (oldToken.length() > 0 && *oldToken.c_str() != '+' && *oldToken.c_str() != '-' && *oldToken.c_str() != '*' && *oldToken.c_str() != '/' && *oldToken.c_str() != '(')) {
            return true;    // Return true to indicate a valid token.
        }

        // Handle the case of white spaces between the operator and the next token assuming it's possible to have white spaces between positive/negative sign and the number
        while (*expression && isspace(*expression)) {
            ++expression;
            counter++;
        }
    }

    // Capture other operators ('*', '/', '(', and ')').
    if (*expression == '*' || *expression == '/' || *expression == '(' || *expression == ')') {
        line.push_back(*expression); // Add the operator character to the string.
        counter++;
        return true;    // Return true to indicate a valid token.
    }

    // Iterate over the expression and split the tokens (numbers).
    while (*expression) {
        flag = true;    // Set the flag to true as the token is valid.
        if (!isdigit(*expression)) { // If a non digit is found, exit the loop.
            break;
        }
        line.push_back(*expression); // Add the digit character to the string.
        ++expression;
        counter++;
    }

    return flag; // Return true to indicate a valid token, false otherwise.
}

/*
    This function takes an arithmetic expression as a char pointer and a reference to an integer as input, and returns a boolean value.
    * Input
        * expression - the arithmetic expression as a char pointer
        * result - reference to an integer to store the result of the expression evaluation
    * Output
        * bool stating whether the expression was evaluated successfully or not
*/
bool evaluate(const char *expression, int &result) {
    // Sanitize the input data
    result = 0;
    if (expression == nullptr) {
        return false;
    }

    // Create two stacks to hold operands and operators
    stack<int> values;
    stack<char> ops;

    string token;
    int counter;

    // Iterate through every token of the expression
    while (extractTokens(expression, token, counter)) {

        // If the token is a number convert to integer and push to operands stack
        if (isdigit(token[0])) {
            int value = stoi(token);
            values.push(value);

            // If the token is an open parenthesis push into the operators stack
        } else if (token[0] == '(') {
            ops.push(token[0]);

            // If the token is a close parenthesis
            // * pop the operators from the operators stack one at a time upto the last open parenthesis
            // * calculate a value by popping the top two operands in the operands stack
            // * push the value calculated back to the operands stack
        } else if (token[0] == ')') {
            while (!ops.empty() && ops.top() != '(') {
                char op = ops.top();
                ops.pop();
                if (!calculate(values, op)) {
                    return false;
                }
            }
            ops.pop();

            // If the token is '+' or '-'
        } else if (token[0] == '+' || token[0] == '-') {

            // Validate the token is an operator or a positive/negative sign
            if (token.length() > 1) {
                int value = stoi(token);
                values.push(value);

                // If the token is an operator push to the operators stack considering the precedence.
                // * if the topmost operator needs to be executed before
                //   * pop the operator from the stack
                //   * calculate a value by popping the top two operands in the operands stack
                //   * push the value calculated back to the operands stack
                // * push the new operator to the operators stack after all the proceeding operations are completed
            } else {
                while (!ops.empty() && precedence(token[0]) <= precedence(ops.top())) {
                    char op = ops.top();
                    ops.pop();
                    if (!calculate(values, op)) {
                        return false;
                    }
                }
                ops.push(token[0]);
            }

            // When the token is '*' or '/' push to the operators stack considering the precedence.
            // * if the topmost operator needs to be executed before
            //   * pop the operator from the stack
            //   * calculate a value by popping the top two operands in the operands stack
            //   * push the value calculated back to the operands stack
            // * push the new operator to the operators stack after all the proceeding operations are completed
        } else if (token[0] == '*' || token[0] == '/'){
            while (!ops.empty() && precedence(token[0]) <= precedence(ops.top())) {
                char op = ops.top();
                ops.pop();
                if (!calculate(values, op)) {
                    return false;
                }
            }
            ops.push(token[0]);

            // Invalid entry
        } else {
            return false;
        }
        expression += counter;
    }

    // After completing reading the expression
    // * pop all the remaining operators from the operators stack one at a time
    // * calculate a value by popping the top two operands in the operands stack
    // * push the value calculated back to the operands stack
    while (!ops.empty()) {
        char op = ops.top();
        ops.pop();
        if (!calculate(values, op)) {
            return false;
        }
    }

    // Validate the operands stack has exactly one value at the end
    if (values.size() == 1) {
        result = values.top();
        return true;
    } else {
        return false;
    }
}

int main() {
    // Prompt the user to enter an arithmetic expression and store it in a string variable called input.
    string input;
    cout << "Enter an arithmetic expression: ";
    getline(cin, input);

    // Convert the input string for evaluation purposes.
    const char* expression = input.c_str();
    int result;

    // Call the evaluate function and store the result in a boolean variable called state.
    bool state = evaluate(expression, result);

    // Print the result of the expression evaluation.
    cout << "Result => " << result << ":" << std::boolalpha << state << endl;

    // End the main function and return 0 to indicate successful program execution.
    return 0;
}