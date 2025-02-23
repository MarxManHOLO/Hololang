#ifndef HOLOLANGPARSER_H
#define HOLOLANGPARSER_H

#include "SimpleString.h"
#include "List.h"
#include "Dictionary.h"
#include "HololangTAC.h"
#include "HololangLexer.h"

namespace Hololang {
	
	
	class BackPatch {
	public:
		long lineNum;			
		String waitingFor;		
		
		BackPatch() {}
		BackPatch(long lineNum, String waitFor) : lineNum(lineNum), waitingFor(waitFor) {}
	};
	
	
	class JumpPoint {
	public:
		long lineNum;			
		String keyword;			// jump type, by keyword: "while", "for", etc.
		
		JumpPoint() {}
		JumpPoint(long lineNum, String keyword) : lineNum(lineNum), keyword(keyword) {}
	};

	class ParseState {
	public:
		List<TACLine> code;
		List<BackPatch> backpatches;
		List<JumpPoint> jumpPoints;
		int nextTempNum;
		String localOnlyIdentifier;		
		bool localOnlyStrict;			
		
		bool empty() { return code.Count() == 0; }
		
		ParseState() { Clear(); }
		
		void Clear() {
			code = List<TACLine>();
			backpatches = List<BackPatch>();
			jumpPoints = List<JumpPoint>();
			nextTempNum = 0;
			localOnlyIdentifier = "";
			localOnlyStrict = false;
		}
		
		void Add(TACLine line) { code.Add(line); }
		
		
		void AddBackpatch(String waitFor) {
			backpatches.Add(BackPatch(code.Count()-1, waitFor));
		}

		void AddJumpPoint(String jumpKeyword) {
			jumpPoints.Add(JumpPoint(code.Count(), jumpKeyword));
		}
		
		JumpPoint CloseJumpPoint(String keyword);
		
		bool IsJumpTarget(long lineNum);
		
		
		void Patch(String keywordFound, long reservingLines=0) {
			Patch(keywordFound, false, reservingLines);
		}
		
	
		void Patch(String keywordFound, bool alsoBreak, long reservingLines=0);

		void PatchIfBlock(bool singleLineIf);
	};
	
	class Parser {
	public:
		String errorContext;	// name of file, etc., used for error reporting
		
	
		List<ParseState> outputStack;
		
		// Partial input, in the case where line continuation has been used.
		String partialInput;
		
		
		ParseState* output;
		
		
		ParseState pendingState;
		bool pending;
		
		Parser() { Reset(); }
		
		
		void Reset() {
			outputStack.Clear();
			outputStack.Add(ParseState());
			output = &outputStack[0];
			pendingState.Clear();
			pending = false;
		}

		
		void PartialReset() {
			while (outputStack.Count() > 1) outputStack.Pop();
			output = &outputStack[0];
			output->backpatches.Clear();
			output->jumpPoints.Clear();
			output->nextTempNum = 0;
		}

		bool NeedMoreInput() {
			return (not partialInput.empty() or outputStack.Count() > 1 or output->backpatches.Count() > 0);
		}

		static bool EndsWithLineContinuation(String sourceCode);
		
		void Parse(String sourceCode, bool replMode=false);

		Machine *CreateVM(TextOutputMethod standardOutput);
		
		FunctionStorage *CreateImport();
		
	private:
		static ParseState nullState;
		
	
		void ParseMultipleLines(Lexer tokens);

		void ParseStatement(Lexer tokens, bool allowExtra=false);
		void ParseAssignment(Lexer tokens, bool allowExtra=false);
		Value ParseExpr(Lexer tokens, bool asLval=false, bool statementStart=false);
		Value ParseFunction(Lexer tokens, bool asLval=false, bool statementStart=false);
		Value ParseOr(Lexer tokens, bool asLval=false, bool statementStart=false);
		Value ParseAnd(Lexer tokens, bool asLval=false, bool statementStart=false);
		Value ParseNot(Lexer tokens, bool asLval=false, bool statementStart=false);
		Value ParseIsA(Lexer tokens, bool asLval=false, bool statementStart=false);
		Value ParseComparisons(Lexer tokens, bool asLval=false, bool statementStart=false);
		Value ParseAddSub(Lexer tokens, bool asLval=false, bool statementStart=false);
		Value ParseMultDiv(Lexer tokens, bool asLval=false, bool statementStart=false);
		Value ParseUnaryMinus(Lexer tokens, bool asLval=false, bool statementStart=false);
		Value ParseNew(Lexer tokens, bool asLval=false, bool statementStart=false);
		Value ParseAddressOf(Lexer tokens, bool asLval=false, bool statementStart=false);
		Value ParsePower(Lexer tokens, bool asLval=false, bool statementStart=false);
		Value ParseDotExpr(Lexer tokens, bool asLval=false, bool statementStart=false);
		Value ParseCallExpr(Lexer tokens, bool asLval=false, bool statementStart=false);
		Value ParseSeqLookup(Lexer tokens, bool asLval=false, bool statementStart=false);
		Value ParseMap(Lexer tokens, bool asLval=false, bool statementStart=false);
		Value ParseList(Lexer tokens, bool asLval=false, bool statementStart=false);
		Value ParseQuantity(Lexer tokens, bool asLval=false, bool statementStart=false);
		Value ParseCallArgs(Value funcRef, Lexer tokens);
		Value ParseAtom(Lexer tokens, bool asLval=false, bool statementStart=false);

		void CheckForOpenBackpatches(int sourceLineNum);
		Value FullyEvaluate(Value val, LocalOnlyMode localOnlyMode=LocalOnlyMode::Off);
		void StartElseClause();
		Token RequireToken(Lexer tokens, Token::Type type, String text=String());
		Token RequireEitherToken(Lexer tokens, Token::Type type1, String text1, Token::Type type2, String text2=String());
		Token RequireEitherToken(Lexer tokens, Token::Type type1, Token::Type type2, String text2=String()) {
			return RequireEitherToken(tokens, type1, String(), type2, text2);
		}

	};
	
}

#endif 
