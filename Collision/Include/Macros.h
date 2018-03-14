#ifndef _MACROS_H_
#define _MACROS_H_

#define XMALIGN __declspec(align(16))

#define XMNEW void* operator new(size_t i)	\
		{									\
			return _mm_malloc(i, 16);		\
		}										

#define XMDELETE void operator delete(void* p) \
		{										\
			_mm_free(p);						\
		}							


#endif
