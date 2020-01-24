// Copyright DEWETRON GmbH 2019
#pragma once

#include "odkbase_if_plugin.h"
#include "odkapi_error_codes.h"
#include "odkapi_message_ids.h"

#include "odkfw_channels.h"
#include "odkfw_if_message_handler.h"

#include <memory>
#include <set>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif


#ifdef WIN32
# ifdef __GNUC__
#  define DLL_EXPORT __attribute__ ((dllexport))
#  define EXTERN_C extern "C"
# else
#  ifndef EXTERN_C
#    define EXTERN_C WINAPI
#  endif
#  define DLL_EXPORT __declspec(dllexport)
# endif
#else
# define DLL_EXPORT __attribute__ ((visibility ("default")))
# define EXTERN_C extern "C"
#endif


#define OXY_REGISTER_PLUGIN1(PLUGIN_NAME, PLUGIN_MANIFEST, PLUGIN_CLASS)                                                \
            extern "C" {                                                                                                \
                DLL_EXPORT const char* dwGetPluginName() { return PLUGIN_NAME; }                                        \
                DLL_EXPORT const char* dwGetPluginManifest() { return PLUGIN_MANIFEST; }                                \
                DLL_EXPORT int dwCreatePlugin(odk::UuidType uuid, void** plugin) {                                      \
                    if (uuid == odk::Oxygen_Plugin_Uuid) {                                                              \
                        if (plugin) {                                                                                   \
                            auto plugin_inst = new PLUGIN_CLASS();                                                      \
                            *plugin = plugin_inst;                                                                      \
                        }                                                                                               \
                        return true;                                                                                    \
                    }                                                                                                   \
                    return false;                                                                                       \
                }                                                                                                       \
                DLL_EXPORT void dwDeletePlugin(odk::UuidType uuid, void* plugin) {                                      \
                    if (uuid == odk::Oxygen_Plugin_Uuid) {                                                              \
                        auto plugin_inst = static_cast<PLUGIN_CLASS*>(plugin);                                          \
                        delete plugin_inst;                                                                             \
                    }                                                                                                   \
                }                                                                                                       \
            }

namespace odk
{
namespace framework
{

    class PluginBase : public odk::IfPlugin
    {
    public:
        PluginBase()
            : m_host(nullptr)
            , m_plugin_channels(std::make_shared<odk::framework::PluginChannels>())
            , m_registered(false)
        {
            m_message_handlers.insert(m_plugin_channels);
        }

        // only override for special message handling; normal stuff should register handlers
        virtual bool handleMessage(odk::PluginMessageId id, std::uint64_t key, const odk::IfValue* param, const odk::IfValue** ret, std::uint64_t& ret_code)
        {
            return false;
        }

        virtual std::uint64_t init(std::string& error_message) = 0;
        virtual bool deinit() = 0;

        virtual ~PluginBase()
        {
        }

    protected:
        odk::IfHost* getHost()
        {
            return m_host;
        }

        const odk::IfHost* getHost() const
        {
            return m_host;
        }

        PluginChannelsPtr getPluginChannels()
        {
            return m_plugin_channels;
        }

    private:
        void PLUGIN_API setPluginHost(odk::IfHost* host) final
        {
            m_host = host;
            m_plugin_channels->setPluginHost(m_host);
        }

        std::uint64_t PLUGIN_API pluginMessage(
            odk::PluginMessageId id, std::uint64_t key,
            const odk::IfValue* param, const odk::IfValue** ret) final;

    private:
        odk::IfHost* m_host;
        std::set<std::shared_ptr<oxy::IfMessageHandler>> m_message_handlers;
        PluginChannelsPtr m_plugin_channels;
        bool m_registered;
    };


}
}
