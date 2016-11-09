#ifndef CALCULATOR_H_INCLUDED
#define CALCULATOR_H_INCLUDED

#include <unordered_map>
#include <deque>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>



using namespace std;


class Calculator
{
public:

    Calculator();
    ~Calculator();

    void start_session();

private:


    void get_line();
    void parse_line();
    void parse_token();

    double ex_func(double input,const string& token);
    double ex_operator(double a, double b, char o);

    int differentiate(const string& token);

    void add_operator();
    void add_parenthesis();

    void parse_whitespace();


    bool is_operator(int a);
    bool is_whitespace(int a);
    bool is_parenthesis(int a);
    bool is_equal_sign(int a);

    bool is_func(const string& token);
    bool is_constant(const string& token);
    bool is_double(const string& token);
    bool is_quit(const string& token);
    bool is_variable(const string& token);
    bool is_let(const string& token);

    double get_double(const string& token);
    double get_constant(const string& token);
    double get_variable(const string& token);

    void output();

    unordered_map<string,double> variables;
    deque<string> operator_stack;
    deque<double> output_queue;

    string line;


    int pi;
    int e;

    int start_index;
    int current_index;
    int break_flag;
    int quit_flag;



};

Calculator::Calculator()
{
    e = 2.718;
    pi = 3.14169;


    start_index = 0;
    current_index = 0;

    break_flag = 0;
    quit_flag = 0;

}

Calculator::~Calculator(void)
{
}


void Calculator::start_session()
{

    while(true)
    {// get new line while true
        get_line();
        if(quit_flag == 1)
        {// quit if flag set
            break;
        }
    }
}

void Calculator::get_line()
{
    //clear previous line
    line.clear();

    if(getline(cin, line))
    {// get line if no errors
        // clear previous elements from last line and parse line
        output_queue.clear();
        operator_stack.clear();
        start_index = 0;
        current_index = 0;
        break_flag = 0;
        parse_line();
    }
    output();
    return;
}

void Calculator::parse_line()
{// parses line with delimiters

    double b = 0;
    double a = 0;
    double result = 0;

    while(current_index < line.size())
    {// loop through all the characters of the line
        if(is_operator(0))
        {// current character is an operator

            // add operator index is not touched for this op
            // add
            add_operator();

            if(!is_operator(-1) && !is_whitespace(-1) && !is_parenthesis(-1))
            {// previous token not delimiter, thus new token save end index of token

                // send token to be identified and perform any appropriate operations
                parse_token();
            }

            // Parse any whitespace
            parse_whitespace();

            // assign start index to next character, not whitespace
            start_index = current_index + 1;
        }
        else if (is_whitespace(0))
        {// delimiter is whitespace
            if(current_index != 0 && !is_operator(-1) && !is_whitespace(-1) && !is_parenthesis(-1))
            {
                parse_token();
            }

            parse_whitespace();
            start_index = current_index + 1;

        }
        else if (is_parenthesis(0))
        {// delimiter is parentheses

            if(current_index != 0 && !is_operator(-1) && !is_whitespace(-1) && !is_parenthesis(-1))
            {// new token
                parse_token();
            }

            add_parenthesis();

            parse_whitespace();
            start_index = current_index + 1;
        }

        if(break_flag == 1)
        {
            break;
        }

        //increase index
        current_index = current_index + 1;

        if(current_index >= line.size() && !is_operator(-1) && !is_whitespace(-1) && !is_parenthesis(-1))
        {// index larger than string and previous char not delimiter

            parse_token();
            break;
        }
    }

    while(!operator_stack.empty() && break_flag != 1)
    {// check to see what the operator that is to be pushed is gonna be.

            a =  output_queue.back();
            output_queue.pop_back();
            b = output_queue.back();
            output_queue.pop_back();

            result = ex_operator(a, b, operator_stack.back().at(0));
            output_queue.push_back(result);
            operator_stack.pop_back();
    }

}



void Calculator::parse_token()
{
    // make sure to check if token is a variable saved.

    // variables for either a numerical or alphabetical token
    double n;

    // get the range of token and cast it as a new string
    int l = current_index - start_index ;

    const string token =  line.substr(start_index, l);

    // find the type of token
    int t = differentiate(token);

    // variables for executing operations which might arise from the let token
    double b = 0;
    double a = 0;
    double result = 0;


    if(t == 0)
    {// token is a constant

        // get constant and push it to the stack
        n = get_constant(token);
        output_queue.push_back(n);
    }
    else if(t == 1)
    {// token is, let, token.

        // Parse whitespace after 'let' token
        parse_whitespace();
        // next character should be the first char of variable to be inserted
        current_index = current_index + 1;
        // save start of variable
        start_index = current_index;

        while(!is_operator(0) && !is_whitespace(0) && !is_parenthesis(0) && !is_equal_sign(0))
        {// iterate to the next delimiter which indicates the end of the variable
            current_index  = current_index  + 1;
        }

        // get range of variable and save it
        l = current_index - start_index;
        const string var = line.substr(start_index, l);

        // Parse any white space until equal sign is reached.
        parse_whitespace();

        // move index to equal
        current_index  = current_index  + 1;

        // parse any white space after equal
        parse_whitespace();
        //move to next index, the first char of expression
        current_index = current_index + 1;
        start_index = current_index;
        // parse the remaining line
        parse_line();
        // back of the stack is the value of the variable
        // insert variable and corresponding value into hash map
        n = output_queue.back();
        variables.insert({var, n});

        // raise flag to break and move to next line
        break_flag = 1;
    }
    else if (t == 2)
    {// token is double

        //push number onto output queue
        n =  get_double(token);
        output_queue.push_back(n);
    }
    else if(t == 3)
    {// token is function

        //push number onto output queue
        operator_stack.push_back(token);
    }
    else if(t == 4)
    {// token is quit

        // break the parsing and quit session
        break_flag = 1;
        quit_flag = 1;
    }
    else if(t == 5)
    {// token is variable

        //push number onto output queue
        n = get_variable(token);
        output_queue.push_back(n);
    }
}


int Calculator::differentiate(const string& token)
{// differentiates between the type of token
 // types: constant, let, double, function, variable

    int m = -1;

    if(is_constant(token))
    {
        m = 0;
    }
    else if(is_let(token))
    {
        m = 1;
    }
    else if(is_double(token))
    {
        m = 2;
    }
    else if(is_func(token))
    {
        m = 3;
    }
    else if(is_quit(token))
    {
        m = 4;
    }
    else if(is_variable(token))
    {
        m = 5;
    }

    return m;
}

void Calculator::add_operator()
{
    // get operator and set it as a string
    string _operator;
    stringstream ss;
    ss << line.at(current_index);
    ss >> _operator;


    if(operator_stack.empty())
    {// operator stack empty, push operator to back
        operator_stack.push_back(_operator);
    }
    else
    {// operator stack is not empty

        double result = 0;
        double a = 0;
        double b = 0;

        const string operators = ("^*/+-");
        const string parentheses = ("()");

        // find the precedence of previous operator
        size_t ii = operators.find(operator_stack.back());
        int previous_precedence = static_cast<int>(ii);

        // find precedence of new operator
        size_t i = operators.find(_operator);
        int input_precedence = static_cast<int>(i);


        if(input_precedence == 0 || operator_stack.back().at(0) == parentheses.at(0) )
        {// operator is exponent or previous operator is an open parenthesis add to top of stack
            operator_stack.push_back(_operator);
        }
        else
        {// add the operator following the shunting-yard algorithm
            while(!operator_stack.empty() && (input_precedence <= previous_precedence ))
            {// stack not empty and new token is less or equal in precedence than previous token
             // perform operation of previous token


                // get and pop the parameters of previous operator
                a =  output_queue.back();
                output_queue.pop_back();
                b = output_queue.back();
                output_queue.pop_back();

                // perform operation, push result in output_queue, and pop operator
                result = ex_operator(a, b, operator_stack.back().at(0));
                output_queue.push_back(result);
                operator_stack.pop_back();

                if(!operator_stack.empty())
                {// stack not empty find the precedence of the previous previous operator
                 // to compare for next iteration.
                  ii = operators.find(operator_stack.back());
                  previous_precedence = static_cast<int>(ii);
                }

            }

            // When there are no more operators, or the precedence of previous operator is greater
            // push operator on to stack
            operator_stack.push_back(_operator);

        }
    }
}

void Calculator::add_parenthesis()
{
    // parse char to string
    const string parentheses = ("()");
    string parenthesis;
    stringstream sss;
    sss << line.at(current_index);
    sss >> parenthesis;

    // variables for operations
    double a = 0;
    double b = 0;
    double result = 0;

    if(parenthesis.at(0) == parentheses.at(0))
    {// left parenthesis, add to top of operator stack
        operator_stack.push_back(parenthesis);
    }
    else if (parenthesis.at(0) == parentheses.at(1))
    {// right, closing parenthesis, find pair and push any operators onto output queue

        while(!operator_stack.empty() && operator_stack.back().at(0) != parentheses.at(0))
        {// push any operators unto output queue, and pop them from operator_stack, until matching parenthesis found

            a =  output_queue.back();
            output_queue.pop_back();
            b = output_queue.back();
            output_queue.pop_back();
            result = ex_operator(a, b, operator_stack.back().at(0));
            output_queue.push_back(result);

            operator_stack.pop_back();
        }
        // pop matching parenthesis
        operator_stack.pop_back();

        if(!operator_stack.empty())
        {
            if(is_func(operator_stack.back()))
            {// next operator a function, execute function.

                a =  output_queue.back();
                output_queue.pop_back();
                result = ex_func(a, operator_stack.back());
                output_queue.push_back(result);
                operator_stack.pop_back();
            }

        }

    }
}
bool Calculator::is_parenthesis(int a)
{
    const string parentheses = ("()");
    a = current_index + a;
    if(line.at(a) == parentheses.at(0) || line.at(a) == parentheses.at(1))
    {
        return true;
    }

    return false;
}

bool Calculator::is_equal_sign(int a)
{
    const string equal_sign = ("=");
    a = current_index + a;
    if(equal_sign.at(0) == line.at(a))
    {
        return true;
    }

    return false;
}

bool Calculator::is_operator(int a)
{
    const string operators = ("^*/+-");
    a = current_index + a;
    for(int i = 0; i < operators.size(); i++)
    {
        if(operators.at(i) == line.at(a))
        {
            return true;
        }
    }

    return false;
}


bool Calculator::is_whitespace(int a)
{

    const string whitespace = (" ");
    a = current_index + a;
    if (a >= line.size())
    {
        return false;
    }
    if(line.at(a) == whitespace.at(0))
    {
        return true;
    }

    return false;
}

void Calculator::parse_whitespace()
{
    while(is_whitespace(1))
    {// go through the white space
        current_index = current_index + 1;
    }
}

bool Calculator::is_let(const string& token)
{
    const string save_variable = ("let");
    if(token.at(0) == save_variable.at(0) && token.at(1) == save_variable.at(1)  && token.at(2) == save_variable.at(2)  )
    {
        return true;
    }

    return false;
}

bool Calculator::is_double(const string& token)
{
    double n;
    stringstream st(token);
    st >> n;
    if (st.fail())
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool Calculator::is_constant(const string& token)
{
    const string Pi = ("pi");
    const string ee = ("e");

    if(token.at(0) == Pi.at(0) && token.at(1) == Pi.at(1))
    {
        return true;
    }
    else if(token.at(0) == ee.at(0))
    {
        return true;
    }
        return false;
}

bool Calculator::is_variable(const string& token)
{
    unordered_map<string,double>::const_iterator got_input_variable = variables.find(token);

    if(got_input_variable == variables.end())
    {
        return false;
    }

    return true;
}

bool Calculator::is_quit(const string& token)
{
    const string quit = ("quit");

    if(token.at(0) == quit.at(0) && token.at(1) == quit.at(1) && token.at(2) == quit.at(2) && token.at(3) == quit.at(3))
    {
            return true;
    }

    return false;

}

bool Calculator::is_func(const string& token)
{
    const string logg = ("log");
    const string coss = ("cos");
    const string sinn = ("sin");

    if(token.at(0) == logg.at(0) && token.at(1) == logg.at(1) && token.at(2) == logg.at(2))
    {
            return true;
    }

    if(token.at(0) == sinn.at(0) && token.at(1) == sinn.at(1) && token.at(2) == sinn.at(2))
    {
            return true;
    }

    if(token.at(0) == coss.at(0) && token.at(1) == coss.at(1) && token.at(2) == coss.at(2))
    {
            return true;
    }

    return false;
}

double Calculator::get_double(const string& token)
{
    double n;
    stringstream st(token);
    st >> n;
    return n;
}

double Calculator::get_constant(const string& token)
{

    const string Pi = ("pi");
    const string ee = ("e");

    if(token.at(0) == Pi.at(0) && token.at(1) == Pi.at(1))
    {
        return pi;
    }
    else if(token.at(0) == ee.at(0))
    {
        return e;
    }
}

double Calculator::get_variable(const string& token)
{
    unordered_map<string,double>::const_iterator got_input_variable = variables.find(token);

    return got_input_variable->second;

}

double Calculator::ex_func(double input, const string& token)
{

    const string logg = ("log");
    const string coss = ("cos");
    const string sinn = ("sin");

    if(token.at(0) == logg.at(0) && token.at(1) == logg.at(1) && token.at(2) == logg.at(2))
    {
            return log(input);
    }

    if(token.at(0) == sinn.at(0) && token.at(1) == sinn.at(1) && token.at(2) == sinn.at(2))
    {
            return sin(input);
    }

    if(token.at(0) == coss.at(0) && token.at(1) == coss.at(1) && token.at(2) == coss.at(2))
    {
            return cos(input);
    }

}

double Calculator::ex_operator(double a, double b, char o)
{// a is left
 // b is right


    double result = 0;
    stringstream sss;
    const string operators = ("^*/+-");
    string op;
    sss << o;
    sss >> op;

    if(operators.at(0) == op.at(0))
    {
        result = pow(a, b);
    }
    else if(operators.at(1) == op.at(0))
    {
        result = b * a;
    }
    else if(operators.at(2) == op.at(0))
    {
        if(a == 0)
        {
            break_flag = 1;
        }
        else
        {
            result = b / a;
        }
    }
    else if(operators.at(3) == op.at(0))
    {
        result = b + a;
    }
    else if(operators.at(4) == op.at(0))
    {
        result = b - a;
    }

    return result;
}

void Calculator::output()
{

  for (deque<double>::const_iterator it = output_queue.begin(); it!=output_queue.end(); ++it)
    {
        cout << *it;
        cout << '\n';
    }
    return;

}


#endif // CALCULATOR_H_INCLUDED
