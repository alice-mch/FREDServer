#include "Parser/calculator.h"

namespace calculator
{

template<typename T>
T ExpressionParser<T>::eval(const std::string& expr, std::map<std::string, int>& tempMap)
{
    T result = 0;
    index_ = 0;
    if(!tempMap.empty())
        vars_ = tempMap;
    expr_ = expr;
    insertVars();//put variables into equation
    try
    {
      result = parseExpr();
      if (!isEnd())
        unexpected();
    }
    catch (const calculator::error&)
    {
      while(!stack_.empty())
        stack_.pop();
      throw;
    }
    return result;
}

template<typename T>
T ExpressionParser<T>::eval(char c, std::map<std::string, int>& tempMap)
{
  std::string expr(1, c);
  return eval(expr, tempMap);
}

template<typename T>
T ExpressionParser<T>::pow(T x, T n)
{
  T res = 1;

  while (n > 0)
  {
    if ((int)n % 2 != 0)
    {
      res *= x;
      n -= 1;
    }
    n /= 2;

    if (n > 0)
      x *= x;
  }

  return res;
}

template<typename T>
T ExpressionParser<T>::checkZero(T value) const
{
  if (value == 0)
  {
    std::string divOperators("/%");
    std::size_t division = expr_.find_last_of(divOperators, index_ - 2);
    std::ostringstream msg;
    msg << "Parser error: division by 0";
    if (division != std::string::npos)
      msg << " (error token is \""
          << expr_.substr(division, expr_.size() - division)
          << "\")";
    throw calculator::error(expr_, msg.str());
  }
  return value;
}

template<typename T>
T ExpressionParser<T>::calculate(T v1, T v2, const Operator& op, T cond) const
{
  switch (op.op)
  {
    case OPERATOR_TERNARY_Q:      return (!!cond)*v1 + (!cond)*v2;
    case OPERATOR_BITWISE_OR:     return (int)v1 | (int)v2;
    case OPERATOR_BITWISE_XOR:    return (int)v1 ^ (int)v2;
    case OPERATOR_BITWISE_AND:    return (int)v1 & (int)v2;
    case OPERATOR_EQUAL:          return v1 == v2;
    case OPERATOR_GREATER:        return v1 > v2;
    case OPERATOR_GREATER_EQUAL:  return v1 >= v2;
    case OPERATOR_LESS:           return v1 < v2;
    case OPERATOR_LESS_EQUAL:     return v1 <= v2;
    case OPERATOR_BITWISE_SHL:    return (int)v1 << (int)v2;
    case OPERATOR_BITWISE_SHR:    return (int)v1 >> (int)v2;
    case OPERATOR_ADDITION:       return v1 + v2;
    case OPERATOR_SUBTRACTION:    return v1 - v2;
    case OPERATOR_MULTIPLICATION: return v1 * v2;
    case OPERATOR_DIVISION:       return (double)v1 / checkZero(v2);
    case OPERATOR_MODULO:         return (int)v1 % (int)checkZero(v2);
    case OPERATOR_POWER:          return pow(v1, v2);
    case OPERATOR_EXPONENT:       return v1 * pow(10, v2);
    default:                      return 0;
  }
}

template<typename T>
bool ExpressionParser<T>::isEnd() const
{
  return index_ >= expr_.size();
}

template<typename T>
char ExpressionParser<T>::getCharacter() const
{
  if (!isEnd())
    return expr_[index_];
  return 0;
}

template<typename T>
void ExpressionParser<T>::expect(const std::string& str)
{
  if (expr_.compare(index_, str.size(), str) != 0)
    unexpected();
  index_ += str.size();
}

template<typename T>
void ExpressionParser<T>::unexpected() const
{
  std::ostringstream msg;
  msg << "Syntax error: unexpected token \""
      << expr_.substr(index_, expr_.size() - index_)
      << "\" at index "
      << index_;
  throw calculator::error(expr_, msg.str());
}

template<typename T>
void ExpressionParser<T>::eatSpaces()
{
  while (std::isspace(getCharacter()) != 0)
    index_++;
}

template<typename T>
typename ExpressionParser<T>::Operator ExpressionParser<T>::parseOp()
{
  eatSpaces();
  switch (getCharacter())
  {
    case ':': index_++;     return Operator(OPERATOR_TERNARY_S,       2, 'L');
    case '?': index_++;     return Operator(OPERATOR_TERNARY_Q,       3, 'L');
    case '|': index_++;     return Operator(OPERATOR_BITWISE_OR,      4, 'L');
    case '^': index_++;     return Operator(OPERATOR_BITWISE_XOR,     5, 'L');
    case '&': index_++;     return Operator(OPERATOR_BITWISE_AND,     6, 'L');
    case '=': index_++;     return Operator(OPERATOR_EQUAL,           7, 'L');
    case '<': index_++; if (getCharacter() != '=' && getCharacter() != '<')
                        {
                            return Operator(OPERATOR_LESS,            8, 'L');
                        }
                        else if (getCharacter() == '=')
                        {
                            index_++;
                            return Operator(OPERATOR_LESS_EQUAL,      8, 'L');
                        }
                        else if (getCharacter() == '<')
                        {
                            index_++;
                            return Operator(OPERATOR_BITWISE_SHL,     9, 'L');
                        }
    case '>': index_++; if (getCharacter() != '=' && getCharacter() != '>')
                          return Operator(OPERATOR_GREATER,           8, 'L');
                        else if (getCharacter() == '=')
                        {
                            index_++;
                            return Operator(OPERATOR_GREATER_EQUAL,   8, 'L');
                        }
                        else if (getCharacter() == '>')
                        {
                            index_++;
                            return Operator(OPERATOR_BITWISE_SHR,     9, 'L');
                        }
    case '+': index_++;     return Operator(OPERATOR_ADDITION,       10, 'L');
    case '-': index_++;     return Operator(OPERATOR_SUBTRACTION,    10, 'L');
    case '/': index_++;     return Operator(OPERATOR_DIVISION,       20, 'L');
    case '%': index_++;     return Operator(OPERATOR_MODULO,         20, 'L');
    case '*': index_++; if (getCharacter() != '*')
                            return Operator(OPERATOR_MULTIPLICATION, 20, 'L');
              index_++;     return Operator(OPERATOR_POWER,          30, 'R');
    case 'e': index_++;     return Operator(OPERATOR_EXPONENT,       40, 'R');
    case 'E': index_++;     return Operator(OPERATOR_EXPONENT,       40, 'R');
    default :               return Operator(OPERATOR_NULL,            0, 'L');
  }
}

template<typename T>
T ExpressionParser<T>::toInteger(char c)
{
  if (c >= '0' && c <= '9') return c -'0';
  if (c >= 'a' && c <= 'f') return c -'a' + 0xa;
  if (c >= 'A' && c <= 'F') return c -'A' + 0xa;
  T noDigit = 0xf + 1;
  return noDigit;
}

template<typename T>
T ExpressionParser<T>::getInteger() const
{
  return toInteger(getCharacter());
}

template<typename T>
T ExpressionParser<T>::parseDecimal()
{
  T value = 0;
  for (T d; (d = getInteger()) <= 9; index_++)
    value = value * 10 + d;
  return value;
}

template<typename T>
T ExpressionParser<T>::parseDouble()
{
    size_t newIndex;
    T value;
    value = stod(expr_.substr(index_), &newIndex);
    index_ += newIndex;
    return value;
}

template<typename T>
T ExpressionParser<T>::parseHex()
{
  index_ = index_ + 2;
  T value = 0;
  for (T h; (h = getInteger()) <= 0xf; index_++)
    value = value * 0x10 + h;
  return value;
}

template<typename T>
bool ExpressionParser<T>::isHex() const
{
  if (index_ + 2 < expr_.size())
  {
    char x = expr_[index_ + 1];
    char h = expr_[index_ + 2];
    return (std::tolower(x) == 'x' && toInteger(h) <= 0xf);
  }
  return false;
}

template<typename T>
T ExpressionParser<T>::parseValue()
{
  T val = 0;
  eatSpaces();

  switch (getCharacter())
  {
    case '0': if (isHex())
                val = parseHex();
              else              
                val = parseDouble();
              break;
    case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9':
              val = parseDouble();
              break;
    case '(': index_++;
              val = parseExpr();
              eatSpaces();
              if (getCharacter() != ')')
              {
                if (!isEnd())
                  unexpected();
                throw calculator::error(expr_, "Syntax error: `)' expected at end of expression");
              }
              index_++; break;
    case '?': index_++;
              val = parseExpr();
              eatSpaces();
              if (getCharacter() != ':')
              {
                if (!isEnd())
                  unexpected();
                throw calculator::error(expr_, "Syntax error: `:' expected at end of ternary operator");
              }
    case '~': index_++; val = ~(int)parseValue(); break;
    case '+': index_++; val =  parseValue(); break;
    case '-': index_++; val =  parseValue() * static_cast<T>(-1);
              break;
    default : if (!isEnd())
                unexpected();
              throw calculator::error(expr_, "Syntax error: value expected at end of expression");
  }
  return val;
}

template<typename T>
T ExpressionParser<T>::parseExpr()
{
  int first, second, third;
  stack_.push(OperatorValue(Operator(OPERATOR_NULL, 0, 'L'), 0));
  // first parse value on the left
  T value = parseValue();

  while (!stack_.empty())
  {
    // parse an operator (+, -, *, ...)
    Operator op(parseOp());

    while (op.precedence  < stack_.top().getPrecedence() || (
           op.precedence == stack_.top().getPrecedence() &&
           op.associativity == 'L'))
    {
      // end reached
      if (stack_.top().isNull())
      {
        stack_.pop();
        return value;
      }
      // do the calculation ("reduce"), producing a new value
      if (op.op == OPERATOR_TERNARY_S)
      {
          op.precedence = 0;
          third = parseExpr();
          second = value;
          first = stack_.top().value;
          value = calculate(second, third, stack_.top().op, first);
      }
      else
      {
          value = calculate(stack_.top().value, value, stack_.top().op);
      }
      stack_.pop();
    }

    // store on stack_ and continue parsing ("shift")
    stack_.push(OperatorValue(op, value));
    // parse value on the right
    value = parseValue();
  }
  return 0;
}

/*template<typename T>
std::map<const std::string&, int> ExpressionParser<T>::setVariable(const std::string name, int value)
{
    vars_[name] = value;
    return vars_;
}*/

template<typename T>
void ExpressionParser<T>::insertVars()
{
  std::vector<std::pair<string, int> > vars_(this->vars_.begin(), this->vars_.end());
  std::sort(vars_.begin(), vars_.end(), [](const std::pair<std::string, int> elem1, const std::pair<std
::string, int> elem2) -> bool
  {
     return elem1.first.length() > elem2.first.length();
  });

  for(auto it=vars_.begin(); it!=vars_.end(); ++it)
  {
     size_t pos;
     size_t lenToReplace = it->first.length();
     while((pos = expr_.find(it->first)) != std::string::npos)
     {
         expr_.replace(pos, lenToReplace, std::to_string(it->second));
     }

  }
  vars_.clear();
}

}
