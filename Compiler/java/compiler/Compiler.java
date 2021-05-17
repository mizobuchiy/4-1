import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.io.Reader;


// こんどは解答です
//   前回のインタプリタと中間表現のところを修正してコンパイラにしました．
//   構文解析系や字句解析系はいじっていません

/*-------------------------- コンパイラ --------------------------------------*/
/// コンパイラ
class Compiler {
  /// メインメソッド
  public static void main(String[] args) {
    // インタプリタを新規生成し，標準入力(System.in)から式を受け取って，
    // 受け取った式を翻訳(process)し，結果を標準出力(System.out)に出力します
    (new Compiler()).process(new InputStreamReader(System.in), System.out);
  }

  /// 文字列を式と認識して計算し，出力する処理
  void process(Reader input, PrintStream output) throws InternalError {
    try {
      // 構文解析系(Parser)に頼んでinputから受け取る文字列を中間表現Nodeに
      // してもらいます
      Node expression = (new Parser(input)).parse();
      // 中間表現をコンパイルして結果をoutputに印字します
      expression.emitCode(output);
      output.println("pop eax"); // 最後に棚からレジスタeaxに結果を下ろす命令
    } catch (InternalError e) {
      e.printStackTrace();
    }
  }
}

/*--------------------------- 中間表現 ---------------------------------------*/
/// 全中間表現のスーパクラス
abstract class Node {
  /// 中間表現に対応する命令列を印字する
  //  ★このメソッドがコンパイラの本体です
  abstract void emitCode(PrintStream output);
}

/// 2項演算の中間表現のスーパクラス
abstract class BinaryOperationNode extends Node {
  /// 左オペランド
  Node _leftHandSide;
  /// 右オペランド
  Node _rightHandSide;

  BinaryOperationNode(Node leftHandSide, Node rightHandSide) {
    _leftHandSide = leftHandSide;
    _rightHandSide = rightHandSide;
  }

  /// 中間表現に対応する命令列を印字する
  //  ★このメソッドがコンパイラの本体です
  void emitCode(PrintStream output) {
    _leftHandSide.emitCode(output); // 左オペランドに対応する命令列を生成
    _rightHandSide.emitCode(output); // 右オペランドに対応する命令列を生成
    output.println("pop ecx"); // 棚からレジスタeaxに右オペランドを下ろす命令
    output.println("pop eax"); // 棚からレジスタeaxに左オペランドを下ろす命令
    output.println(opcode() + " ecx, eax"); // 演算し結果をeaxに格納する命令
    output.println("push eax"); // 結果を棚に積む命令
  }

  abstract String opcode(); // 具象クラスで実装します
}

/// 加算の中間表現
class AddNode extends BinaryOperationNode {
  AddNode(Node lhs, Node rhs) {
    super(lhs, rhs);
  }

  String opcode() {
    return "add";
  }
}

/// 減算の中間表現
class SubNode extends BinaryOperationNode {
  SubNode(Node lhs, Node rhs) {
    super(lhs, rhs);
  }

  String opcode() {
    return "sub";
  }
}

/// 乗算の中間表現
class MulNode extends BinaryOperationNode {
  MulNode(Node lhs, Node rhs) {
    super(lhs, rhs);
  }

  String opcode() {
    return "mul";
  }
}

/// 除算の中間表現
class DivNode extends BinaryOperationNode {
  DivNode(Node lhs, Node rhs) {
    super(lhs, rhs);
  }

  String opcode() {
    return "div";
  }
}

/// 定数の中間表現
class ConstantNode extends Node {
  // 定数値
  int _value;

  ConstantNode(int value) {
    _value = value;
  }

  /// 中間表現に対応する命令列を印字する
  //  ★このメソッドがコンパイラの本体です
  void emitCode(PrintStream output) {
    output.println("push " + _value); // 結果を棚に積む命令
  }
}

/*---------------------------- 字句解析系 ------------------------------------*/
class InternalError extends Error {}

/// 字句
// 字句解析系による文字列の分析結果の構成要素が字句です．たとえば字句解析系は
// "1+2"という文字列を分析して{ INTEGER, PLUS, INTEGER }にします
enum Token { // enumって何？と思ったら検索してみましょう
  PLUS { //このプログラムの字句解析系が認識する字句は，プラス，
    boolean isPlusOrMinus() {
      return (true);
    }

    Node createBinaryOperator(Node lhs, Node rhs) {
      return (new AddNode(lhs, rhs));
    }
  },
  MINUS { //マイナス，
    boolean isPlusOrMinus() {
      return (true);
    }

    Node createBinaryOperator(Node lhs, Node rhs) {
      return (new SubNode(lhs, rhs));
    }
  },
  ASTERISK { //アスタリスク，
    boolean isAsteriskOrSlash() {
      return (true);
    }

    Node createBinaryOperator(Node lhs, Node rhs) {
      return (new MulNode(lhs, rhs));
    }
  },
  SLASH { //スラッシュ，
    boolean isAsteriskOrSlash() {
      return (true);
    }

    Node createBinaryOperator(Node lhs, Node rhs) {
      return (new DivNode(lhs, rhs));
    }
  },
  INTEGER, //整数，
  EOF; // end of file の6種類です

  // このプログラムの字句には，字句がプラスあるいはマイナスか，
  boolean isPlusOrMinus() {
    return (false);
  }

  // アスタリスクあるいはスラッシュか問合せられるように，
  boolean isAsteriskOrSlash() {
    return (false);
  }

  // 更には対応する中間表現の作成も依頼できるようにしてあります
  Node createBinaryOperator(Node lhs, Node rhs) {
    throw(new InternalError());
  }
}

/// 字句解析系
class LexicalAnalyzer {
  private Reader _input;
  private Token _currentToken;
  private int _integerValue;
  private int _nextChar;

  LexicalAnalyzer(Reader input) throws InternalError {
    _input = input;
    readNextChar();
  }

  /// 今読み込んだ字句を返します
  Token currentToken() {
    return (_currentToken);
  }

  /// 今読み込んだ字句がINTEGERのときだけ使えるメソッドで，読み込んだ整数の
  /// 値を返します
  int integerValue() {
    return (_integerValue);
  }

  /// 次の字句を読み込みます
  /// 構文解析系は字句の処理が終わるとこのメソッドを読んで字句解析系に文字列
  /// の解釈を再開させます．字句解析系は1つ字句を読み終わった時点で停止し，
  /// 構文解析系から次の指示がくるのを待ちます
  void readNextToken() throws InternalError {
    if (_currentToken == Token.EOF) {
      throw(new InternalError());
    } else {
      switch (_nextChar) {
        case '+':
          _currentToken = Token.PLUS;
          readNextChar();
          return;
        case '-':
          _currentToken = Token.MINUS;
          readNextChar();
          return;
        case '*':
          _currentToken = Token.ASTERISK;
          readNextChar();
          return;
        case '/':
          _currentToken = Token.SLASH;
          readNextChar();
          return;
        case -1:
          _currentToken = Token.EOF;
          return;
        default:
          if (Character.isDigit(_nextChar)) {
            _currentToken = Token.INTEGER;
            readIntegerValue();
            return;
          } else {
            System.out.println((char) _nextChar);
            throw(new InternalError());
          }
      }
    }
  }

  /// _inputから次の文字を読み込む
  private void readNextChar() throws InternalError {
    try {
      _nextChar = _input.read();
    } catch (IOException e) {
      throw(new InternalError());
    }
  }

  /// _inputから整数を読み込む
  private void readIntegerValue() {
    _integerValue = Character.digit(_nextChar, 10);
    readNextChar();
    if (_integerValue != 0) {
      while (Character.isDigit(_nextChar)) {
        _integerValue = _integerValue * 10 + Character.digit(_nextChar, 10);
        readNextChar();
      }
    }
  }
}

/*---------------------------- 構文解析系 ------------------------------------*/
class Parser {
  /// 字句解析系
  /// 構文解析系は字句解析系から字句を順次受け取って中間表現に組み上げてゆきます
  private LexicalAnalyzer _lex;

  Parser(Reader input) throws InternalError {
    _lex = new LexicalAnalyzer(input);
    _lex.readNextToken();
  }

  /// 因子の構文解析
  /// 因子の構文規則「因子 ::= 整数」の実装です．今回演習で作るコンパイラの
  /// 構文規則では因子は整数にしかならなので実装は単純です
  private Node parseFactor() throws InternalError {
    if (_lex.currentToken() == Token.INTEGER) {
      Node result = new ConstantNode(_lex.integerValue());
      _lex.readNextToken();
      return (result);
    } else {
      throw(new InternalError());
    }
  }

  /// 項の構文解析
  /// 項の構文規則「項 := 因子((*|/)因子)*」の実装です．構文規則通り
  private Node parseTerm() throws InternalError {
    Node result = parseFactor(); // 最初に因子を読んで
    while (_lex.currentToken().isAsteriskOrSlash()) { // 次が* or /である限り
      Token ateriskOrSlash = _lex.currentToken(); // * or /を読んで
      _lex.readNextToken(); // 因子を読んで，を繰返して
      result = ateriskOrSlash.createBinaryOperator(result, parseFactor());
    } // 中間表現を組み上げます
    return (result);
  }

  /// 式の構文解析
  /// 式の構文規則「式 := 項((+|-)項)*」の実装です．こちらも構文規則通りです
  private Node parseExpression() throws InternalError {
    Node result = parseTerm();
    while (_lex.currentToken().isPlusOrMinus()) {
      Token plusOrMinus = _lex.currentToken();
      _lex.readNextToken();
      result = plusOrMinus.createBinaryOperator(result, parseTerm());
    }
    return (result);
  }

  /// プログラム全体の構文解析
  /// 実習で作るコンパイラでは「プログラム := 式」なので式の構文解析を
  /// おこなって結果を返します
  Node parse() throws InternalError {
    return (parseExpression());
  }
}
