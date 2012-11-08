#ifndef __CLASSINDLL_H__
#define __CLASSINDLL_H__


#ifdef __unix__
#define CLASSINDLL_CLASS_DECL
#else

#pragma warning(disable: 4251)

#ifdef _CLASSINDLL
	#define CLASSINDLL_CLASS_DECL       __declspec(dllimport)
#else
	#define CLASSINDLL_CLASS_DECL       __declspec(dllexport)
#endif

#endif

#endif // ClassInDLL_H
