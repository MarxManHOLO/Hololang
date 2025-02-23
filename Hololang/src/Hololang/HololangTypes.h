#ifndef HOLOLANGTYPES_H
#define HOLOLANGTYPES_H

#include "SimpleString.h"
#include "List.h"
#include "Dictionary.h"

#include <cstdint>

namespace Hololang {
	
	extern const String VERSION;
	
	class FuncParam;
	class TACLine;
	class Value;
	class Context;
	class Machine;
	
	unsigned int HashValue(const Value& v);
	
	typedef List<Value> ValueList;
	typedef ListStorage<Value> ValueListStorage;
	typedef DictionaryStorage<Value, Value> ValueDictStorage;
	typedef DictIterator<Value, Value> ValueDictIterator;
	typedef Dictionary<Value, Value, HashValue> ValueDict;

	
	class FunctionStorage : public RefCountedStorage {
	public:
		// Function parameters
		List<FuncParam> parameters;
		
		// Function code (compiled down to TAC form)
		List<TACLine> code;
		
		// Local variables where the function was defined {#8}
		ValueDict outerVars;
		
		FunctionStorage *BindAndCopy(ValueDict contextVariables);
	};

	class SeqElemStorage;

	enum class ValueType : unsigned char {
		Null,
		Number,
		Temp,
		// Ref-counted types:
		String,
		List,
		Map,
		Function,
		Var,
		SeqElem,
		Handle		
	};
	
	enum class LocalOnlyMode : unsigned char {
		Off = 0,
		Warn,
		Strict
	};

	String ToString(ValueType type);

	class Value {
	public:
		static long maxStringSize;
		static long maxListSize;
		static int maxIsaDepth;
		
		ValueType type;
		bool noInvoke;
		LocalOnlyMode localOnly;
		union {
			double number;
			RefCountedStorage *ref;
			int tempNum;
		} data;
		
		// constructors from base types
		Value() : type(ValueType::Null), noInvoke(false), localOnly(LocalOnlyMode::Off) {}
		Value(double number) : type(ValueType::Number), noInvoke(false), localOnly(LocalOnlyMode::Off) { data.number = number; }
		Value(const char *s) : type(ValueType::String), noInvoke(false), localOnly(LocalOnlyMode::Off) { String temp(s); data.ref = temp.ss; temp.forget(); }
		Value(const String& s) : type(ValueType::String), noInvoke(false), localOnly(LocalOnlyMode::Off) { data.ref = (s.ss ? s.ss : emptyString.data.ref);	retain(); }
		Value(const ValueList& l) : type(ValueType::List), noInvoke(false), localOnly(LocalOnlyMode::Off) { ((ValueList&)l).ensureStorage(); data.ref = l.ls; retain(); }
		Value(const ValueDict& d) : type(ValueType::Map), noInvoke(false), localOnly(LocalOnlyMode::Off) { ((ValueDict&)d).ensureStorage(); data.ref = d.ds; retain(); }
		Value(FunctionStorage *s) : type(ValueType::Function), noInvoke(false), localOnly(LocalOnlyMode::Off) { data.ref = s; }
		Value(SeqElemStorage *s);

		// some factory functions to make things clearer
		static Value Temp(const int tempNum) { return Value(tempNum, ValueType::Temp); }
		static Value Var(const String& ident) { return Value(ident, ValueType::Var); }
		static Value SeqElem(const Value& seq, const Value& idx);
		static Value NewHandle(RefCountedStorage* data) { Value v; v.type = ValueType::Handle; v.data.ref = data; return v; }
		static Value Truth(bool b) { return b ? one : zero; }
		static Value Truth(double b);

		static Value GetKeyValuePair(Value map, long index);
		
		// copy-ctor, assignment-op, destructor
		Value(const Value &other) : type(other.type), noInvoke(other.noInvoke), localOnly(other.localOnly) {
			data = other.data;
			if (usesRef()) retain();
		}
		Value& operator= (const Value& other) {
			if (other.usesRef() and other.data.ref) other.data.ref->retain();
			if (usesRef()) release();
			type = other.type;
			noInvoke = other.noInvoke;
			localOnly = other.localOnly;
			data = other.data;
			return *this;
		}
		inline ~Value() { if (usesRef()) release(); }

		// conversions
		String ToString(Machine *vm=nullptr);
		String CodeForm(Machine *vm, int recursionLimit=-1);
		int32_t IntValue() const noexcept;
		uint32_t UIntValue() const noexcept;
		float FloatValue() const noexcept;
		bool BoolValue() const noexcept;
		double DoubleValue() const noexcept { return type == ValueType::Number ? data.number : 0; }
		
		
		String GetString() const { Assert(type == ValueType::String or type == ValueType::Var);
			StringStorage *ss = (StringStorage*)(data.ref);
			if (!data.ref) return String();
			ss->retain();
			return String(ss, false); }
		ValueList GetList() const { Assert(type == ValueType::List); ValueList l((ValueListStorage*)(data.ref), false); return l; }
		ValueDict GetDict() { Assert(type == ValueType::Map); if (not data.ref) data.ref = new ValueDictStorage(); ValueDict d((ValueDictStorage*)(data.ref)); d.retain(); return d; }

		// evaluation
		bool IsNull() const {
			return type == ValueType::Null /* || (usesRef() && data.ref == nullptr) */;
		}

		Value Val(Context *context, ValueDict *outFoundInMap=nullptr) const;
		
		
		Value FullEval(Context *context);
		
	
		Value EvalCopy(Context *context);
		
		
		bool CanSetElem() { return type == ValueType::List or type == ValueType::Map; }
		
		
		void SetElem(Value index, Value value);

		
		Value GetElem(Value index);
		
		
		static Value Resolve(Value sequence, String identifier, Context *context, ValueDict *outFoundInMap);

		
		Value Lookup(Value key) {
			Value result = null;
			Value obj = *this;
			while (obj.type == ValueType::Map) {
				ValueDict d = obj.GetDict();
				if (d.ApplyEvalOverride(key, result)) return result;
				if (d.Get(key, &result)) return result;
				if (!d.Get(Value::magicIsA, &obj)) break;
			}
			return null;
		}

		
		bool IsA(Value type, Machine *vm);
		
		// handy statics (DO NOT MUTATE THESE!)
		static Value zero;			// 0
		static Value one;			// 1
		static Value emptyString;	// ""
		static Value magicIsA;		// "__isa"
		static Value null;			// null
		static Value keyString;		// "key"
		static Value valueString;	// "value"
		static Value implicitResult;	// variable "_"

		inline bool operator==(const Value& rhs) const;
		inline bool operator!=(const Value& rhs) const { return !(*this == rhs); }
		unsigned int Hash() const;
		static double Equality(const Value& lhs, const Value& rhs, int recursionDepth=16);
		inline bool RefEquals(const Value& rhs) const;
		
	private:
		// private constructors used by factory functions
		Value(const int tempNum, ValueType type) : type(type), noInvoke(false), localOnly(LocalOnlyMode::Off) { data.tempNum = tempNum; }	// (type should be ValueType::Temp)
		Value(const String& s, ValueType type) : type(type), noInvoke(false), localOnly(LocalOnlyMode::Off) { data.ref = s.ss; retain(); }

		// reference handling (for types where that applies)
		bool usesRef() const { return type >= ValueType::String; }
		void retain() { if (data.ref) data.ref->retain(); }
		void release() { if (data.ref) { data.ref->release(); data.ref = nullptr; } }

		// equality helpers
		static bool Equal(StringStorage *lhs, StringStorage *rhs);
		static bool Equal(ListStorage<Value> *lhs, ListStorage<Value> *rhs);
		static bool Equal(DictionaryStorage<Value, Value> *lhs, DictionaryStorage<Value, Value> *rhs);
		static bool Equal(SeqElemStorage *lhs, SeqElemStorage *rhs);
		static bool RefEqual(const Value& lhs, const Value& rhs);
		
		// more hashing/equality helpers, for recursive reference types (List and Map)
		bool RecursiveEqual(Value rhs) const;
		unsigned int RecursiveHash() const;
	};
	
	class FuncParam {
	public:
		FuncParam() {}
		
		String name;
		Value defaultValue;
		
		FuncParam(String name, Value defaultValue) : name(name), defaultValue(defaultValue) {}
	};

	inline bool Value::operator==(const Value& rhs) const {
		if (type != rhs.type) return false;
		switch (type) {
			case ValueType::Null:
				return true;		// null values are always equal
				
			case ValueType::Number:
				return data.number == rhs.data.number;
				
			case ValueType::String:
			case ValueType::Var:
			{
				if (data.ref == rhs.data.ref) return true;
				if (!data.ref || !rhs.data.ref) return false;
				return Equal((StringStorage*)data.ref, (StringStorage*)rhs.data.ref);
			}
			case ValueType::List:
			{
				if (data.ref == rhs.data.ref) return true;
				if (!data.ref || !rhs.data.ref) return false;
				return RecursiveEqual(rhs);
			}
			case ValueType::Map:
			{
				if (data.ref == rhs.data.ref) return true;
				if (!data.ref || !rhs.data.ref) return false;
				return RecursiveEqual(rhs);
			}
			case ValueType::Function:
				// Two functions are equal only if they refer to the exact same function
				return (data.ref == rhs.data.ref);

			case ValueType::Temp:
				return (data.tempNum == rhs.data.tempNum);
				
			case ValueType::SeqElem:
				if (data.ref == rhs.data.ref) return true;
				if (!data.ref || !rhs.data.ref) return false;
				return Equal((SeqElemStorage*)data.ref, (SeqElemStorage*)rhs.data.ref);
			
			case ValueType::Handle:
				return (data.ref == rhs.data.ref);
		}
		return false;
	}

	bool Value::RefEquals(const Value& rhs) const {
		if (!usesRef()) return *this == rhs;
		return data.ref == rhs.data.ref;
	}
	

	inline Value Value::Truth(double truthValue) {
		if (truthValue == 0.0) return Value::zero;
		if (truthValue == 1.0) return Value::one;
		return truthValue;
	}


	
	class SeqElemStorage : public RefCountedStorage {
	public:
		Value sequence;
		Value index;

		SeqElemStorage(Value seq, Value idx) : sequence(seq), index(idx) {}
	};

	inline Value::Value(SeqElemStorage *s) : type(ValueType::SeqElem), noInvoke(false) {
		data.ref = s;
	}

	inline Value Value::SeqElem(const Value& seq, const Value& idx) {
		return Value(new SeqElemStorage(seq, idx));
	}

	typedef void (*TextOutputMethod)(String text, bool addLineBreak);

}

#endif 
