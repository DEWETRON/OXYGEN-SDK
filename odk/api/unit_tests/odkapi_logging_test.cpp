// Copyright DEWETRON GmbH 2018

#include "odkapi_logging.h"
#include "odkapi_error_codes.h"
#include "odkapi_message_ids.h"
#include "odkbase_basic_values.h"
#include "odkbase_if_host.h"

#include <boost/test/unit_test.hpp>

#include <functional>
#include <string>

using namespace odk;

namespace
{
    class TestStringValue : public odk::IfStringValue
    {
    public:
        const char* PLUGIN_API getValue() const final { return m_value.c_str(); }
        int PLUGIN_API getLength() const final { return static_cast<int>(m_value.size()); }
        void PLUGIN_API set(const char* value) final { m_value.assign(value); }
        Type PLUGIN_API getType() const final { return Type::TYPE_STRING; }
        const char* PLUGIN_API getDebugString() const final { return ""; }
        void PLUGIN_API addRef() const final { ++m_ref; }
        void PLUGIN_API release() const final
        {
            if (--m_ref == 0) {
                delete this;
            }
        }
    private:
        mutable int m_ref = 1;
        std::string m_value;
    };

    class TestHost : public odk::IfHost
    {
    public:
        std::function<uint64_t(MessageId msg_id, std::uint64_t key, const IfValue* param)> fn_messageAsync;

        IfValue* PLUGIN_API createValue(IfValue::Type type) const final
        {
            if (type == IfValue::Type::TYPE_STRING)
            {
                return new TestStringValue;
            }
            return nullptr;
        }

        std::uint64_t PLUGIN_API messageSync(MessageId msg_id, std::uint64_t key, const IfValue* param, const IfValue** ret = nullptr) final
        {
            return odk::error_codes::NOT_IMPLEMENTED;
        }

        std::uint64_t PLUGIN_API messageSyncData(MessageId msg_id, std::uint64_t key, const void* param, std::uint64_t param_size, const IfValue** ret = nullptr) final
        {
            return odk::error_codes::NOT_IMPLEMENTED;
        }

        std::uint64_t PLUGIN_API messageAsync(MessageId msg_id, std::uint64_t key, const IfValue* param) final
        {
            if (fn_messageAsync)
            {
                return fn_messageAsync(msg_id, key, param);
            }
            return odk::error_codes::NOT_IMPLEMENTED;
        }

        const IfValue* PLUGIN_API query(const char* context, const char* item, const IfValue* param) final
        {
            return nullptr;
        }

        const IfValue* PLUGIN_API queryXML(const char* context, const char* item, const char* xml, std::uint64_t xml_size) final
        {
            return nullptr;
        }
    };
}

BOOST_AUTO_TEST_SUITE(logging)

BOOST_AUTO_TEST_CASE(simple_logging_test)
{
    TestHost host;

    host.fn_messageAsync = [=](MessageId msg_id, std::uint64_t key, const IfValue* param) -> uint64_t
        {
            BOOST_CHECK_EQUAL(msg_id, odk::host_msg_async::LOG_MESSAGE);
            BOOST_CHECK_EQUAL(key, static_cast<uint64_t>(odk::LOGLEVEL_INFO));
            BOOST_REQUIRE(param);
            BOOST_REQUIRE(param->getType() == IfValue::Type::TYPE_STRING);
            auto string_param = static_cast<const odk::IfStringValue*>(param);
            BOOST_CHECK_EQUAL(string_param->getValue(), "UnitTest String");
            return odk::error_codes::OK;
        };
    odk::logMessage(&host, odk::LOGLEVEL_INFO, "UnitTest String");
}

BOOST_AUTO_TEST_CASE(simple_format_test)
{
    TestHost host;

    host.fn_messageAsync = [=](MessageId msg_id, std::uint64_t key, const IfValue* param) -> uint64_t
        {
            BOOST_CHECK_EQUAL(msg_id, odk::host_msg_async::LOG_MESSAGE);
            BOOST_CHECK_EQUAL(key, static_cast<uint64_t>(odk::LOGLEVEL_ERROR));
            BOOST_REQUIRE(param);
            BOOST_REQUIRE(param->getType() == IfValue::Type::TYPE_STRING);
            auto string_param = static_cast<const odk::IfStringValue*>(param);
            BOOST_CHECK_EQUAL(string_param->getValue(), "UnitTest 100 string");
            return odk::error_codes::OK;
        };
    odk::logMessage(&host, odk::LOGLEVEL_ERROR, "UnitTest %d %s", 100, "string");
}

BOOST_AUTO_TEST_SUITE_END()
