

#ifndef HOLOLANGINTERPRETER_H
#define HOLOLANGINTERPRETER_H

#include "SimpleString.h"
#include "HololangTAC.h"

namespace Hololang {

	
	class Parser;
	
	class Interpreter {
		
	public:
		
		TextOutputMethod standardOutput;

		
		TextOutputMethod implicitOutput;
		
		
		TextOutputMethod errorOutput;

	
		void* hostData;
		
		
		Machine *vm;
		
		
		Interpreter();
		Interpreter(String source);
		Interpreter(List<String> source);
		
		
		~Interpreter();
		
		
		bool Done() { return vm == nullptr or vm->Done(); }

		
		void Stop() { if (vm) vm->Stop(); }

		
		void Reset(String source="") {
			this->source = source;
			parser = nullptr;
			vm = nullptr;
		}
		
		void Reset(List<String> source);

		void Restart() { if (vm) vm->Reset();	}

		
		void Compile();

		
		void Step();
		
		
		void RunUntilDone(double timeLimit=60, bool returnEarly=true);

		
		
		void REPL(String sourceLine, double timeLimit=60);
		
		
		bool NeedMoreInput();

		
       
		Value GetGlobalValue(String varName);

		
        void SetGlobalValue(String varName, Value value);

	protected:
		void CheckImplicitResult(long previousImpResultCount);

		virtual void ReportError(const HololangException& mse);

	private:
		String source;
		Parser *parser;
	};
}

#endif 
