import std.stdio;
import std.getopt;
import std.file;
import std.regex;
import std.conv;
import std.uni;
import std.ascii;
import std.algorithm;

/+
 `scan` returns the next character
 `tokenise` returns the next token and its type
 `parse` generates the abstract syntax tree
 `interpret` evaluates the abstract syntax tree
 `generate` writes the abstract syntax tree to x86-64 assembler for yasm

  Tokenise, parse and interpret or generate operations have their own grammatical rules.

  The tokeniser needs a rule for every specific identifier it is to recognise.
  These rules can be constructed from finite-state machines or regular expressions,
  e.g. a named variable must start with [a-zA-Z] and may be followed by [a-zA-Z0-9],
       a string must start and end with '"'
  i.e. a recognised intentifier (a token) has a type and name.

  http://parsingintro.sourceforge.net/
 +/

enum Mode { scan, tokenise, parse, interpret, generate, }
Mode mode = Mode.scan;


void main(string[] args)
{
  getopt(args, "mode", &mode);

  if (args.length < 2)
    {
      writeln("no files provided");
      return;
    }

  string[] inputs;
  foreach (ref arg; args[1..$])
    {
      if (exists(arg))
        {
          ++inputs.length;
          inputs[$-1] = arg;
        }
    }

  foreach (input; inputs)
    {
      auto scanner = Scanner(input);
      scanner.evaluate();

      auto lexer = Lexer(scanner);
      lexer.evaluate();

      auto parser = Parser(lexer);
      parser.evaluate();
    }
}


struct Scanner
{
  this(string fileName)
  {
    this.text = readText(fileName);
  }

  void evaluate()
  {
    writeln("Running the scanner...");
    char e;
    while (getNext(e))
      {
        writeln(e);
      }
  }

  bool getNext(out char e)
  {
    bool success = false;
    if (offset < this.text.length)
      {
        success = true;
        e = this.text[offset];
        ++offset;
      }

    return success;
  }

  bool peekNext(out char e)
  {
    bool success = false;
    if (offset < this.text.length)
      {
        success = true;
        e = this.text[offset];
      }

    return success;
  }

  void increment()
  {
    ++offset;
  }

  void clear()
  {
    offset = 0;
    line = 0;
    col = 0;
  }

  string text;
  int offset = 0;
  int line = 0;
  int col = 0;
}


struct Token
{
  string type = "";
  string name = "";
  int offset = 0;
  int length = 0;
}

struct IdentifierToken
{
  bool hasFirstChar(char e) { return std.ascii.isAlpha(e); }
  bool hasNextChar(char e) { return std.ascii.isAlphaNum(e); }
  string[] keyword = ["import", "return"];
  string[] primative = ["void", "int", "string"];
}

struct SymbolToken
{
  bool isUnary(char e) { return find(unary, e).length > 0; }
  bool isBinary(char e, char ep) { return find(binary, [e, ep]).length > 0; }
  char[] unary = ['=', '!', '(', ')', '[', ']', '{', '}', ';', '.', ',', '+', '-', '*', '/'];
  string[] binary = ["==", "!="];
}

struct NumberToken
{
  bool hasFirstChar(char e) { return std.ascii.isDigit(e); }
  bool hasNextChar(char e) { return hasFirstChar(e); }
}

struct StringToken
{
  bool hasFirstChar(char e) { return e == '"'; }
  bool hasLastChar(char e) { return hasFirstChar(e); }
}


struct Lexer
{
  this(ref Scanner scanner)
  {
    this.scanner = scanner;
    this.scanner.clear();
  }

  void evaluate()
  {
    writeln("Running the lexer...");
    Token token;
    while (getNext(token))
      {
        writeln(to!string(token.type), ": ", token.name);
      }
  }

  bool getNext(out Token token)
  {
    bool success = false;
    char e, ep;

    while (this.scanner.getNext(e))
      {
        // ignore whitespace
        while (std.ascii.isWhite(e))
          {
            this.scanner.getNext(e);
          }

        // ignore comments
        while (e == '/' && this.scanner.peekNext(ep) && ep == '*')
          {
            this.scanner.increment();
            while (this.scanner.getNext(e))
              {
                if (e == '*' && this.scanner.peekNext(ep) && ep == '/')
                  {
                    this.scanner.increment();
                    break;
                  }
              }
          }

        auto identifierToken = IdentifierToken();
        if (identifierToken.hasFirstChar(e))
          {
            token.offset = this.scanner.offset - 1;
            token.length = 1;
            while (this.scanner.peekNext(ep) && identifierToken.hasNextChar(ep))
              {
                this.scanner.increment();
                ++token.length;
              }
            token.type = typeof(identifierToken).stringof;
            token.name = this.scanner.text[token.offset..token.offset + token.length];
            return true;
          }

        auto numberToken = NumberToken();
        if (numberToken.hasFirstChar(e))
          {
            token.offset = this.scanner.offset - 1;
            token.length = 1;
            while (this.scanner.peekNext(ep) && numberToken.hasNextChar(ep))
              {
                this.scanner.increment();
                ++token.length;
              }
            token.type = typeof(numberToken).stringof;
            token.name = this.scanner.text[token.offset..token.offset + token.length];
            return true;
          }

        auto stringToken = StringToken();
        if (stringToken.hasFirstChar(e))
          {
            token.offset = this.scanner.offset - 1;
            token.length = 1;
            while (this.scanner.getNext(e))
              {
                ++token.length;
                if (this.scanner.peekNext(ep) && stringToken.hasLastChar(ep))
                  {
                    this.scanner.increment();
                    ++token.length;
                    token.type = typeof(stringToken).stringof;
                    token.name = this.scanner.text[token.offset..token.offset + token.length];
                    return true;
                  }
              }
          }

        auto symbolToken = SymbolToken();
        if (this.scanner.peekNext(ep) && symbolToken.isBinary(e, ep))
          {
            this.scanner.increment();
            token.length = 2;
            token.type = typeof(symbolToken).stringof;
          }
        else if (symbolToken.isUnary(e))
          {
            token.length = 1;
            token.type = typeof(symbolToken).stringof;
          }
        if (token.type == typeof(symbolToken).stringof)
          {
            token.offset = this.scanner.offset - token.length;
            token.name = this.scanner.text[token.offset..token.offset + token.length];
            return true;
          }
      }

    return success;
  }

  Scanner scanner;
}


struct Node
{
  int level = 0;
  Token token;
  Token[] children;
}

struct Terminal
{
  char opAssign = '=';
}

struct Parser
{
  this(ref Lexer lexer)
  {
    this.lexer = lexer;
    this.lexer.scanner.clear();
  }

  void evaluate()
  {
    writeln("Running the parser...");
    Node node;
    buildTree(node);
  }

  void buildTree(out Node node)
  {
    // statement(node);
  }

  // void statement(out Node node)
  // {
  //   Token t0, t1;
  //   if (lexer.getNext(t0) && token.type == IndentifierToken.stringof)
  //     {
  //       lexer.getNext(t1);
  //       if (expect(t1, Terminal().opAssign))
  //         {
  //           expression(t1);
  //         }
  //     }
  // }

  // bool expect(ref Token token, char e)
  // {
  //   return token.name == e;
  // }

  Lexer lexer;
}
