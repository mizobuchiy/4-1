import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.io.Reader;

class Interpreter {
  public static void main(String[] args) {
    (new Interpreter()).process(new InputStreamReader(System.in), System.out);
  }

  void process(Reader input, PrintStream output) throws InternalError {
    try {
      Node expression = (new Parser(input)).parse();
      output.println(expression.evaluate());
    } catch (InternalError e) {
      e.printStackTrace();
    }
  }
}

abstract class Node {
  abstract int evaluate();
}

abstract class BinaryOperationNode extends Node {
  Node leftHandSide;
  Node rightHandSide;

  BinaryOperationNode(Node leftHandSide, Node rightHandSide) {
    this.leftHandSide = leftHandSide;
    this.rightHandSide = rightHandSide;
  }
}

class AddNode extends BinaryOperationNode {
  AddNode(Node lhs, Node rhs) {
    super(lhs, rhs);
  }

  int evaluate() {
    return (leftHandSide.evaluate() + rightHandSide.evaluate());
  }
}

class SubNode extends BinaryOperationNode {
  SubNode(Node lhs, Node rhs) {
    super(lhs, rhs);
  }

  int evaluate() {
    return (leftHandSide.evaluate() - rightHandSide.evaluate());
  }
}

class MulNode extends BinaryOperationNode {
  MulNode(Node lhs, Node rhs) {
    super(lhs, rhs);
  }

  int evaluate() {
    return (leftHandSide.evaluate() * rightHandSide.evaluate());
  }
}

class DivNode extends BinaryOperationNode {
  DivNode(Node lhs, Node rhs) {
    super(lhs, rhs);
  }

  int evaluate() {
    return (leftHandSide.evaluate() / rightHandSide.evaluate());
  }
}

class ConstantNode extends Node {
  int value;

  ConstantNode(int value) {
    this.value = value;
  }

  int evaluate() {
    return (value);
  }
}

class InternalError extends Error {}

enum Token {
  PLUS {
    boolean isPlusOrMinus() {
      return (true);
    }

    Node createBinaryOperator(Node lhs, Node rhs) {
      return (new AddNode(lhs, rhs));
    }
  },
  MINUS {
    boolean isPlusOrMinus() {
      return (true);
    }

    Node createBinaryOperator(Node lhs, Node rhs) {
      return (new SubNode(lhs, rhs));
    }
  },
  ASTERISK {
    boolean isAsteriskOrSlash() {
      return (true);
    }

    Node createBinaryOperator(Node lhs, Node rhs) {
      return (new MulNode(lhs, rhs));
    }
  },
  SLASH {
    boolean isAsteriskOrSlash() {
      return (true);
    }

    Node createBinaryOperator(Node lhs, Node rhs) {
      return (new DivNode(lhs, rhs));
    }
  },
  INTEGER,
  EOF;

  boolean isPlusOrMinus() {
    return (false);
  }

  boolean isAsteriskOrSlash() {
    return (false);
  }

  Node createBinaryOperator(Node lhs, Node rhs) {
    throw(new InternalError());
  }
}

class LexicalAnalyzer {
  private Reader input;
  private Token currentToken;
  private int integerValue;
  private int nextChar;

  LexicalAnalyzer(Reader input) throws InternalError {
    this.input = input;
    readNextChar();
  }

  Token currentToken() {
    return (currentToken);
  }

  int integerValue() {
    return (integerValue);
  }

  void readNextToken() throws InternalError {
    if (currentToken == Token.EOF) {
      throw(new InternalError());
    } else {
      switch (nextChar) {
        case '+':
          currentToken = Token.PLUS;
          readNextChar();
          return;
        case '-':
          currentToken = Token.MINUS;
          readNextChar();
          return;
        case '*':
          currentToken = Token.ASTERISK;
          readNextChar();
          return;
        case '/':
          currentToken = Token.SLASH;
          readNextChar();
          return;
        case -1:
          currentToken = Token.EOF;
          return;
        default:
          if (Character.isDigit(nextChar)) {
            currentToken = Token.INTEGER;
            readIntegerValue();
            return;
          } else {
            System.out.println((char) nextChar);
            throw(new InternalError());
          }
      }
    }
  }

  private void readNextChar() throws InternalError {
    try {
      nextChar = input.read();
    } catch (IOException e) {
      throw(new InternalError());
    }
  }

  private void readIntegerValue() {
    integerValue = Character.digit(nextChar, 10);
    readNextChar();
    if (integerValue != 0) {
      while (Character.isDigit(nextChar)) {
        integerValue = integerValue * 10 + Character.digit(nextChar, 10);
        readNextChar();
      }
    }
  }
}

class Parser {
  private LexicalAnalyzer lex;

  Parser(Reader input) throws InternalError {
    lex = new LexicalAnalyzer(input);
    lex.readNextToken();
  }

  private Node parseFactor() throws InternalError {
    if (lex.currentToken() == Token.INTEGER) {
      Node result = new ConstantNode(lex.integerValue());
      lex.readNextToken();
      return (result);
    } else {
      throw(new InternalError());
    }
  }

  private Node parseTerm() throws InternalError {
    Node result = parseFactor();
    while (lex.currentToken().isAsteriskOrSlash()) {
      Token asteriskOrSlash = lex.currentToken();
      lex.readNextToken();
      result = asteriskOrSlash.createBinaryOperator(result, parseFactor());
    }
    return (result);
  }

  private Node parseExpression() throws InternalError {
    Node result = parseTerm();
    while (lex.currentToken().isPlusOrMinus()) {
      Token plusOrMinus = lex.currentToken();
      lex.readNextToken();
      result = plusOrMinus.createBinaryOperator(result, parseTerm());
    }
    return (result);
  }

  Node parse() throws InternalError {
    return (parseExpression());
  }
}
