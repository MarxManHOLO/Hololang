#ifndef HOLOLANGINTRINSICS_H
#define HOLOLANGINTRINSICS_H

#include "HololangTypes.h"

namespace Hololang {

	class Context;

	
	extern String hostName;
	extern String hostInfo;
	extern double hostVersion;
	
	class Intrinsics {
	public:
		static void InitIfNeeded();
		
		
		static void CompileSlice(List<TACLine> code, Value list, Value fromIdx, Value toIdx, int resultTempNum);
		
		
		static ValueList StackList(Machine *vm);
		
		
		static Value FunctionType();
		static Value ListType();
		static Value MapType();
		static Value NumberType();
		static Value StringType();
	private:
		static bool initialized;
	};
	
	class IntrinsicResultStorage : public RefCountedStorage {
	public:
		bool done;			
		Value result;		
		
	private:
		IntrinsicResultStorage() : done(true) {}
		virtual ~IntrinsicResultStorage() {}
		friend class IntrinsicResult;
	};
	
	class IntrinsicResult {
	public:
		IntrinsicResult() : rs(nullptr) {}
		IntrinsicResult(Value value, bool done=true) : rs(nullptr) {
			ensureStorage();
			rs->result = value;
			rs->done = done;
		}
		IntrinsicResult(const IntrinsicResult& other) {	((IntrinsicResult&)other).ensureStorage(); rs = other.rs; retain(); }
		IntrinsicResult& operator= (const IntrinsicResult& other) {	((IntrinsicResult&)other).ensureStorage(); other.rs->refCount++; release(); rs = other.rs; return *this; }

		~IntrinsicResult() { release(); }
		
		bool Done() { return not rs or rs->done; }
		Value Result() { return rs ? rs->result : Value::null; }

		static IntrinsicResult Null;		
		static IntrinsicResult EmptyString;	
		
	private:
		IntrinsicResult(IntrinsicResultStorage* storage) : rs(storage) {} 
		void forget() { rs = nullptr; }
		
		void retain() { if (rs) rs->refCount++; }
		void release() { if (rs and --(rs->refCount) == 0) { delete rs; rs = nullptr; } }
		void ensureStorage() { if (!rs) rs = new IntrinsicResultStorage(); }
		IntrinsicResultStorage *rs;
	};

	class Intrinsic {
	public:
		
		String name;

		
		IntrinsicResult (*code)(Context *context, IntrinsicResult partialResult);
		
		
		long id() { return numericID; }
		
		void AddParam(String name, Value defaultValue) { function->parameters.Add(FuncParam(name, defaultValue)); }
		void AddParam(String name, double defaultValue);
		void AddParam(String name) { AddParam(name, Value::null); }

		
		Value GetFunc();

		
		static Intrinsic *GetByID(long id) { return all[id]; }
		
		
		static Intrinsic *GetByName(String name) {
			Intrinsics::InitIfNeeded();
			Intrinsic* result = nullptr;
			nameMap.Get(name, &result);
			return result;
		}
		
		
		static Intrinsic* Create(String name);
		
	
		static IntrinsicResult Execute(long id, Context *context, IntrinsicResult partialResult);

		
		static List<Intrinsic*> all;

	private:
		Intrinsic() {}		

		FunctionStorage* function;
		Value valFunction;		
		long numericID;			

		static Dictionary<String, Intrinsic*, hashString> nameMap;
	};

	void InitRand(unsigned int seed);
}



#endif 
