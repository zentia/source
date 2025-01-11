#pragma once

#define DECLARE_CLASS( TClass, TSuperClass, TStaticFlags, TStaticCastFlags, TPackage, TRequiredAPI ) \
private: \
    TClass& operator=(TClass&&); \
    TClass& operator=(const TClass&); \