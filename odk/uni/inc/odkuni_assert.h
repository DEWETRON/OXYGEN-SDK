// Copyright DEWETRON GmbH 2013
#pragma once

#include <cassert>

#define ODK_ASSERT(condition) assert(condition)

#define ODK_ASSERT_EQUAL(val1, val2) assert(val1 == val2)

#define ODK_ASSERT_LTE(val1, val2) assert(val1 <= val2)

#define ODK_ASSERT_GTE(val1, val2) assert(val1 >= val2)

#define ODK_ASSERT_MSG(condition, msg, ...) assert(condition)

#define ODK_ASSERT_FAIL(msg, ...) assert(false)

#define ODK_VERIFY(condition) \
    (void)(condition);

#define ODK_VERIFY_MSG(condition, msg, ...) \
    (void)(condition);


