#include "HololangInterpreter.h"
#include "HololangParser.h"
#include "SplitJoin.h"

namespace Hololang {
	
	Interpreter::Interpreter() : standardOutput(nullptr), errorOutput(nullptr), implicitOutput(nullptr),
								parser(nullptr), vm(nullptr), hostData(nullptr) {
		
	}

	Interpreter::Interpreter(String source) : standardOutput(nullptr), errorOutput(nullptr), implicitOutput(nullptr),
	parser(nullptr), vm(nullptr), hostData(nullptr) {
		Reset(source);
	}
	
	Interpreter::Interpreter(List<String> source) : standardOutput(nullptr), errorOutput(nullptr), implicitOutput(nullptr),
	parser(nullptr), vm(nullptr), hostData(nullptr) {
		Reset(source);
	}

	Interpreter::~Interpreter() {
		// We own the parser and the VM...
		delete(parser); parser = nullptr;
		delete(vm); vm = nullptr;
		// But we do not own hostData; it's up to the host to deal with that.
	}

	void Interpreter::Reset(List<String> source) {
		Reset(Join("\n", source));
	}

	void Interpreter::Compile() {
		if (vm) return;		// already compiled
		if (not parser) parser = new Parser();
		try {
			parser->Parse(source);
			vm = parser->CreateVM(standardOutput);
			vm->interpreter = this;
		} catch (const HololangException& mse) {
			ReportError(mse);
		}
	}
	
	
	void Interpreter::Step() {
		try {
			Compile();
			if (vm) vm->Step();
		} catch (const HololangException& mse) {
			ReportError(mse);
		}
	}
	

	void Interpreter::RunUntilDone(double timeLimit, bool returnEarly) {
		long startImpResultCount = 0;
		try {
			if (not vm) {
				Compile();
				if (not vm) return;	// (must have been some error)
			}
			startImpResultCount = vm->GetGlobalContext()->implicitResultCounter;
			double startTime = vm->RunTime();
			vm->yielding = false;
			int checkRuntimeIn = 15;		// (because vm->RunTime() is expensive on many machines)
			while (not vm->Done() && !vm->yielding) {
				if (checkRuntimeIn-- == 0) {
					if (vm->RunTime() - startTime > timeLimit) return;	// time's up for now!
					checkRuntimeIn = 15;
				}
				vm->Step();		// update the machine
				if (returnEarly and not vm->GetTopContext()->partialResult.Done()) return;	// waiting for something
			}
		} catch (const HololangException& mse) {
			ReportError(mse);
			vm->GetTopContext()->JumpToEnd();
		}
		CheckImplicitResult(startImpResultCount);
	}

	
	void Interpreter::REPL(String sourceLine, double timeLimit) {
		if (not parser) parser = new Parser();
		if (not vm) {
			vm = parser->CreateVM(standardOutput);
			vm->interpreter = this;
        } else if (vm->Done() && !parser->NeedMoreInput()) {
          
            vm->GetTopContext()->ClearCodeAndTemps();
			parser->PartialReset();
        }
		if (sourceLine == "#DUMP") {
			//ToDo:			vm->DumpTopContext();
			return;
		}
		
		double startTime = vm->RunTime();
		Context *globalContext = vm->GetGlobalContext();
		long startImpResultCount = globalContext->implicitResultCounter;
		vm->storeImplicit = (implicitOutput != nullptr);
		vm->yielding = false;
		
		try {
			if (not sourceLine.empty()) parser->Parse(sourceLine, true);
			if (not parser->NeedMoreInput()) {
				while (not vm->Done() && !vm->yielding) {
					if (vm->RunTime() - startTime > timeLimit) return;	// time's up for now!
					vm->Step();
				}
				CheckImplicitResult(startImpResultCount);
			}
			
		} catch (const HololangException& mse) {
			ReportError(mse);
			
			vm->GetTopContext()->JumpToEnd();
		}
        
        
	}

	
	
	bool Interpreter::NeedMoreInput() {
		return parser and parser->NeedMoreInput();
	}


	
    Value Interpreter::GetGlobalValue(String varName) {
        if (not vm) return Value::null;

		Context* globalContext = vm->GetGlobalContext();
        if (globalContext == nullptr) return Value::null;
        try
        {
            return globalContext->GetVar(varName);

		} catch (const HololangException& mse) {
            ReportError(mse);
            return Value::null;
        }
	}


  
	void Interpreter::SetGlobalValue(String varName, Value value)
    {
        if (vm) vm->GetGlobalContext()->SetVar(varName, value);
	}

	void Interpreter::CheckImplicitResult(long previousImpResultCount) {
		if (!implicitOutput) return;
		Context* globalContext = vm->GetGlobalContext();
		if (implicitOutput and globalContext->implicitResultCounter > previousImpResultCount) {
			Value result = globalContext->GetVar("_");
			if (!result.IsNull()) (*implicitOutput)(result.ToString(vm), true);
		}
	}
	
	void Interpreter::ReportError(const HololangException& mse) {
		if (errorOutput) (*errorOutput)(mse.Description(), true);
	}

}
