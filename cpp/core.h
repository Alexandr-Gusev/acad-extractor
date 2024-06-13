#ifndef CORE_H
#define CORE_H

#ifdef AE_EXPORT
#define AE_EXTERN_C extern "C"
#define AE_DECL_SPEC __declspec(dllexport) __stdcall
#endif

#ifdef AE_IMPORT
#define AE_EXTERN_C extern "C"
#define AE_DECL_SPEC __declspec(dllimport) __stdcall
#endif

#ifndef AE_DECL_SPEC
#define AE_EXTERN_C
#define AE_DECL_SPEC
#endif

#include <vector>
#include <functional>
#include <unknwn.h>

AE_EXTERN_C long AE_DECL_SPEC select_on_screen
(
    std::vector<BSTR>* const names,
    std::vector<std::vector<std::pair<BSTR, BSTR>>>* const attrs_list,
    std::vector<std::vector<std::pair<BSTR, VARIANT>>>* const props_list,
    std::vector<BSTR>* const handles = 0,
    std::function<void(int processed, int total)> callback = 0
);

#endif // CORE_H
