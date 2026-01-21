// Copyright DEWETRON GmbH 2017
#ifndef _ODKBASE_IF_API_OBJECT_H_
#define _ODKBASE_IF_API_OBJECT_H_


#ifdef WIN32
#  define PLUGIN_API __stdcall
#else
#  define PLUGIN_API
#endif

namespace odk
{

    /**
     * ApiObject instances are created within the plugin runtime and are reference counted.
     * All references should be released as soon as they are no longer required, and have to be released on shutdown of the plugin.
     */
    class IfApiObject
    {
    public:

        /**
         * Increment reference counter of the service
         */
        virtual void PLUGIN_API addRef() const = 0;
        /**
         * Decrease reference counter of the service
         * Object may no longer be accessed through this pointer after reference counter becomes 0
         */
        virtual void PLUGIN_API release() const = 0;

    protected:
        IfApiObject() = default;

        // instances may not be deleted
        // (only implementations may do that once the reference counter is 0)
        // therefore the destructor is _not_ virtual so it's not in the vtable
        ~IfApiObject() = default;

    private:
        IfApiObject(const IfApiObject&) = default;
        IfApiObject& operator=(const IfApiObject&)
        { return *this;}
    };

    /**
     * helper function for ApiObjectPtr
     */
    inline void intrusive_ptr_add_ref(const IfApiObject* service)
    {
        service->addRef();
    }

    /**
     * helper function for ApiObjectPtr
     */
    inline void intrusive_ptr_release(const IfApiObject* service)
    {
        service->release();
    }

}

#endif //_ODKBASE_IF_API_OBJECT_H_

