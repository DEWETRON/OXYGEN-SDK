// Copyright DEWETRON GmbH 2020
#pragma once

#ifdef _MSVC_LANG
#  define ODK_CPLUSPLUS _MSVC_LANG
#else
#  define ODK_CPLUSPLUS __cplusplus
#endif

#define ODK_UNUSED(expr) do { (void)(expr); } while (0)

#if ODK_CPLUSPLUS >= 201703L
#  define ODK_NODISCARD [[nodiscard]]
#else
#  define ODK_NODISCARD
#endif
