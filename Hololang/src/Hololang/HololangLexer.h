#ifndef HOLOLANGLEXER_H
#define HOLOLANGLEXER_H

#include "SimpleString.h"
#include "List.h"

namespace Hololang {
	
	class Token {
	public:
		enum class Type {
			Unknown,
			Keyword,
			Number,
			String,
			Identifier,
			OpAssign,
			OpPlus,
			OpMinus,
			OpTimes,
			OpDivide,
			OpMod,
			OpPower,
			OpEqual,
			OpNotEqual,
			OpGreater,
			OpGreatEqual,
			OpLesser,
			OpLessEqual,
			OpAssignPlus,
			OpAssignMinus,
			OpAssignTimes,
			OpAssignDivide,
			OpAssignMod,
			OpAssignPower,
			LParen,
			RParen,
			LSquare,
			RSquare,
			LCurly,
			RCurly,
			AddressOf,
			Comma,
			Dot,
			Colon,
			Comment,
			EOL
		};
		
		Token() : type(Type::Unknown) {}
		Token(Type t) : type(t) {}
		Token(Type t, String s) : type(t), text(s) {}

		String ToString();
		
		Type type;
		String text;
		bool afterSpace;
		
		static Token EOL;
	};
	
	class LexerStorage  {
	private:
		LexerStorage(String s) : refCount(1), lineNum(1), input(s), positionB(0) {
			inputLengthB = input.LengthB();
		}
		~LexerStorage() {}
		
		long refCount;
		
		int lineNum;
		String input;
		long inputLengthB;
		long positionB;
		List<Token> pending;
		
		friend class Lexer;
	};

	class Lexer
	{
	public:
		
		Lexer() { ls = nullptr; }
		Lexer(String input) { ls = new LexerStorage(input); }
		Lexer(const Lexer& other) { ls = other.ls; retain(); }
		Lexer& operator= (const Lexer& other) { if (other.ls) other.ls->refCount++; release(); ls = other.ls; return *this; }

		
		~Lexer() { release(); }
		
		
		bool isNull() { return ls == nullptr; }
		bool atEnd() { return isNull() or (ls->positionB >= ls->inputLengthB and ls->pending.Count() == 0); }
		int lineNum() { return ls == nullptr ? 0 : ls->lineNum; }
		
		bool isAtWhitespace() {
			
			return atEnd() || IsWhitespace(ls->input[ls->positionB]);
		}
		

		
		Token Dequeue();
		Token Peek();
		void Poke(Token tokenToInsert);
		void SkipWhitespaceAndComment();
		
		
		static bool IsNumeric(char c);
		static bool IsIdentifier(char c);
		static bool IsWhitespace(char c);
		static bool IsInStringLiteral(long charPosB, String source, long startPosB);
		static long CommentStartPosB(String source, long startPosB);
		static String TrimComment(String source);
		static Token LastToken(String source);
		
	private:
		Lexer(LexerStorage* storage) : ls(storage) {}  
		void retain() { if (ls) ls->refCount++; }
		void release() { if (ls and --(ls->refCount) == 0) { delete ls; ls = nullptr; } }
		void ensureStorage() { if (!ls) ls = new LexerStorage(""); }
		LexerStorage *ls;

	};
}



#endif 
