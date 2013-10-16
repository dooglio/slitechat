#ifndef __LLCHATIBEXPORTS_H__
#define __LLCHATIBEXPORTS_H__

// Right now, we are not building a DLL
//
#if 0 //defined(WIN32)
#  if defined(LLChatLib_EXPORTS)
#     define	LLCHATLIBEXP	__declspec( dllexport )
#  else
#     define	LLCHATLIBEXP	__declspec( dllimport )
#  endif
#else
#     define   LLCHATLIBEXP
#endif

#endif //__LLCHATIBEXPORTS_H__
