#ifndef REFCOUNTEDSTORAGE_H
#define REFCOUNTEDSTORAGE_H

#include <stdio.h>

namespace Hololang {

#if DEBUG
extern long _stringInstanceCount();
#endif

	class RefCountedStorage {
	public:
		void retain() { refCount++; }
		void release() { if (--refCount == 0) delete this; }
		
	protected:
		RefCountedStorage() : refCount(1) {
#if(DEBUG)
			instanceCount++;
			printf("+++ %ld instances (%ld strings)\n", instanceCount, _stringInstanceCount());
#endif
		}
		virtual ~RefCountedStorage() {
#if(DEBUG)
			instanceCount--;
			printf("--- %ld instances (%ld strings)\n", instanceCount, _stringInstanceCount());
#endif
		}
		
		long refCount;
		
#if(DEBUG)
	public:
		static long instanceCount;
#endif
	};
	

}
#endif 
