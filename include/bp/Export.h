#ifndef BP_EXPORT_H
#define BP_EXPORT_H

#ifdef SWIG
#define BP_EXPORT
#endif

#ifndef BP_EXPORT
#ifdef BP_EXPORTS
#define BP_EXPORT __declspec(dllexport)
#else
#define BP_EXPORT __declspec(dllimport)
#endif
#endif

#endif // BP_EXPORT_H