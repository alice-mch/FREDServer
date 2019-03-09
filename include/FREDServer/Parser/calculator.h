///
/// @file      calculator.hpp
/// @brief     calculator::eval(const std::string&) evaluates an integer
///            arithmetic expression and returns the result. If an error
///            occurs a calculator::error exception is thrown.
///            <https://github.com/kimwalisch/calculator>
/// @author    Kim Walisch, <kim.walisch@gmail.com>
/// @copyright Copyright (C) 2013-2018 Kim Walisch
/// @license   BSD 2-Clause, https://opensource.org/licenses/BSD-2-Clause
/// @version   1.4
///
/// == Supported operators ==
///
/// OPERATOR    NAME                     ASSOCIATIVITY    PRECEDENCE
///
/// |           Bitwise Inclusive OR    Left               4
/// ^           Bitwise Exclusive OR    Left               5
/// &           Bitwise AND             Left               6
/// <<          Shift Left              Left               9
/// >>          Shift Right             Left               9
/// +           Addition                Left              10
/// -           Subtraction             Left              10
/// *           Multiplication          Left              20
/// /           Division                Left              20
/// %           Modulo                  Left              20
/// **          Raise to power          Right             30
/// e, E        Scientific notation     Right             40
/// ~           Unary complement        Left              99
///
/// The operator precedence has been set according to (uses the C and
/// C++ operator precedence): https://en.wikipedia.org/wiki/Order_of_operations
/// Operators with higher precedence are evaluated before operators
/// with relatively lower precedence. Unary operators are set to have
/// the highest precedence, this is not strictly correct for the power
/// operator e.g. "-3**2" = 9 but a lot of software tools (Bash shell,
/// Microsoft Excel, GNU bc, ...) use the same convention.
///
/// == Examples of valid expressions ==
///
/// "65536 >> 15"                       = 2
/// "2**16"                             = 65536
/// "(0 + 0xDf234 - 1000)*3/2%999"      = 828
/// "-(2**2**2**2)"                     = -65536
/// "(0 + ~(0xDF234 & 1000) *3) /-2"    = 817
/// "(2**16) + (1 << 16) >> 0X5"        = 4096
/// "5*-(2**(9+7))/3+5*(1 & 0xFf123)"   = -109221
///
/// == About the algorithm used ==
///
/// calculator::eval(std::string&) relies on the ExpressionParser
/// class which is a simple C++ operator precedence parser with infix
/// notation for integer arithmetic expressions.
/// ExpressionParser has its roots in a JavaScript parser published
/// at: http://stackoverflow.com/questions/28256/equation-expression-parser-with-precedence/114961#114961
/// The same author has also published an article about his operator
/// precedence algorithm at PerlMonks:
/// http://www.perlmonks.org/?node_id=554516
///

#ifndef CALCULATOR_HPP
#define CALCULATOR_HPP

#include <stdexcept>
#include <string>
#include <sstream>
#include <stack>
#include <cstddef>
#include <cctype>
#include <map>

namespace calculator
{

/// calculator::eval() throws a calculator::error if it fails
/// to evaluate the expression string.
///
class error : public std::runtime_error
{
public:
  error(const std::string& expr, const std::string& message)
    : std::runtime_error(message),
      expr_(expr)
  { }
#if __cplusplus < 201103L
  ~error() throw() { }
#endif
  std::string expression() const
  {
    return expr_;
  }
private:
  std::string expr_;
};

template <typename T>
class ExpressionParser
{
public:
  /// Evaluate an integer arithmetic expression and return its result.
  /// @throw error if parsing fails.
  ///
  T eval(const std::string& expr, std::map<std::string, int>& tempMap);

  /// Get the integer value of a character.
  T eval(char c, std::map<std::string, int> &tempMap);

  //std::map<const std::string&, int> setVariable(const std::string name, int value);


private:
  enum
  {
    OPERATOR_NULL,
    OPERATOR_TERNARY_S,      /// :
    OPERATOR_TERNARY_Q,      /// ?
    OPERATOR_BITWISE_OR,     /// |
    OPERATOR_BITWISE_XOR,    /// ^
    OPERATOR_BITWISE_AND,    /// &
    OPERATOR_EQUAL,          /// =
    OPERATOR_LESS,           /// <
    OPERATOR_LESS_EQUAL,     /// <=
    OPERATOR_GREATER,        /// >
    OPERATOR_GREATER_EQUAL,  /// >=
    OPERATOR_BITWISE_SHL,    /// <<
    OPERATOR_BITWISE_SHR,    /// >>
    OPERATOR_ADDITION,       /// +
    OPERATOR_SUBTRACTION,    /// -
    OPERATOR_MULTIPLICATION, /// *
    OPERATOR_DIVISION,       /// /
    OPERATOR_MODULO,         /// %
    OPERATOR_POWER,          /// **
    OPERATOR_EXPONENT        /// e, E
  };

  struct Operator
  {
    /// Operator, one of the OPERATOR_* enum definitions
    int op;
    int precedence;
    /// 'L' = left or 'R' = right
    int associativity;
    Operator(int opr, int prec, int assoc) :
      op(opr),
      precedence(prec),
      associativity(assoc)
    { }
  };

  struct OperatorValue
  {
    Operator op;
    T value;
    OperatorValue(const Operator& opr, T val) :
      op(opr),
      value(val)
    { }
    int getPrecedence() const
    {
      return op.precedence;
    }
    bool isNull() const
    {
      return op.op == OPERATOR_NULL;
    }
  };

  /// Expression string
  std::string expr_;
  /// Current expression index, incremented whilst parsing
  std::size_t index_;
  /// The current operator and its left value
  /// are pushed onto the stack if the operator on
  /// top of the stack has lower precedence.
  std::stack<OperatorValue> stack_;
  /// list of variables and corresponding values
  std::map<std::string, int> vars_;

  /// Exponentiation by squaring, x^n.
  static T pow(T x, T n);

  T checkZero(T value) const;

  T calculate(T v1, T v2, const Operator& op, T cond = 0) const;

  bool isEnd() const;

  /// Returns the character at the current expression index or
  /// 0 if the end of the expression is reached.
  ///
  char getCharacter() const;

  /// Parse str at the current expression index.
  /// @throw error if parsing fails.
  ///
  void expect(const std::string& str);

  void unexpected() const;

  /// Eat all white space characters at the
  /// current expression index.
  ///
  void eatSpaces();

  void insertVars();

  /// Parse a binary operator at the current expression index.
  /// @return Operator with precedence and associativity.
  ///
  Operator parseOp();

  static T toInteger(char c);

  T getInteger() const;

  T parseDecimal();

  T parseDouble();

  T parseHex();

  bool isHex() const;

  /// Parse an integer value at the current expression index.
  /// The unary `+', `-' and `~' operators and opening
  /// parentheses `(' cause recursion.
  ///
  T parseValue();

  /// Parse all operations of the current parenthesis
  /// level and the levels above, when done
  /// return the result (value).
  ///
  T parseExpr();
};

template <typename T>
inline T eval(const std::string& expression, std::map<std::string, int>& tempMap)
{
  ExpressionParser<T> parser;
  return parser.eval(expression, tempMap);
}

template <typename T>
inline T eval(char c, std::map<std::string, int>& tempMap)
{
  ExpressionParser<T> parser;
  return parser.eval(c, tempMap);
}

inline double eval(const std::string& expression, std::map<std::string, int>& tempMap)
{
  return eval<double>(expression, tempMap);
}

inline double eval(char c, std::map<std::string, int>& tempMap)
{
  return eval<double>(c, tempMap);
}

/*template <typename T>
inline std::map<const std::string, int> setVariable(const std::string name, T value)
{
  ExpressionParser<T> paFREDServerrser;
  return parser.setVariable(name, value);
}*/

} // namespace calculator
#include "../../src/Parser/calculator.cpp"
#endif
