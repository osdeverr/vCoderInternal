#pragma once

#include <exception>
#include <memory>
#include <vector>
#include <tuple>

#define CXSTRINGIFY(x) #x

// Macros for convenience
#define CXXREFLECT_INTERNAL_PLISTNAME CXProperties // Name of the class static field

/*
 --- 'properties' macro ---
 Declares a property block: can be used outside of a class declaration
 but should be kept in it for clarity
 Input:
 [ClassType] T - Class type
 Usage example:
 class ReflectedClass
 {
 public:
 // Class fields go here...
 
 // Define a property block for ReflectedClass
 properties(ReflectedClass) (
 // Properties go here...
 );
 };
 */
#define CXPROPS(T) \
typedef T CXBaseClass; \
bool HasCXProps = true; \
constexpr static auto CXXREFLECT_INTERNAL_PLISTNAME() { return std::make_tuple

#define CXPROPS_END ;}

/*
 --- 'property' macro ---
 Declares a property: must be used in a property block
 
 Input:
 [ClassMember] member - Class/struct field to bind
 [Optional] [String] name - Name of the property: using the in-class
 field name if not specified
 Usage example:
 // Class fields
 std::string mStringMember;
 int mIntMember;
 bool mBoolMember;
 
 // Define a property block for ReflectedClass
 properties(ReflectedClass) (
 // Define a property automatically named 'mStringMember'
 property(mStringMember),
 // Define a property with a custom name
 property(mIntMember, "customPropertyName")
 );
 */
#define CXPROP(...) CXPROP_VA(__VA_ARGS__, CXPROP_NAMED, CXPROP_AUTONAME)(__VA_ARGS__)

// Internal macros for 'property'
#define CXPROP_AUTONAME(member) CX::Reflection::Internal::DefineProperty(&CXBaseClass::member, CXSTRINGIFY(member))
#define CXPROP_NAMED(member, name) CX::Reflection::Internal::DefineProperty(&CXBaseClass::member, name)
#define CXPROP_VA(_1,_2,NAME,...) NAME

/*
 --- 'enumerate' macro ---
 Uses a lambda to iterate through the properties of a class.
 Input:
 [TypeName] T - Class/struct typename; must have a defined property block
 [T] obj - T instance to be iterated
 Usage example:
 ReflectedClass cls;
 cls.mIntMember = 5;
 cls.mBoolMember = true;
 cls.mStringMember = "CXXRefl test";
 
 enumerate(ReflectedClass, cls) {
 std::cout << "Value " << name << " = " << value << std::endl;
 } cxxenum_end;
 */
#define CXENUM(T, obj) \
constexpr auto nbProperties = std::tuple_size<decltype(T::CXXREFLECT_INTERNAL_PLISTNAME())>::value; \
CX::Reflection::Internal::ForIntSequence(std::make_index_sequence<nbProperties>{}, [&](auto i) { CXENUM_INIT(T, obj);

#define CXENUM_CONST(T, obj) \
constexpr auto nbProperties = std::tuple_size<decltype(T::CXXREFLECT_INTERNAL_PLISTNAME())>::value; \
CX::Reflection::Internal::ForIntSequence(std::make_index_sequence<nbProperties>{}, [&](auto i) { CXENUM_INIT(T, obj, const);

// Internal macros for 'enumerate'
#define CXENUM_INIT(T, obj, ...) \
constexpr auto __property = std::get<i>(T::CXXREFLECT_INTERNAL_PLISTNAME()); \
__VA_ARGS__ T& __object = obj; \
const std::string name = __property.name; \
__VA_ARGS__ auto& value = __object.*(__property.member); \
using type = typename decltype(__property)::Type
#define CXENUM_END })

namespace CX
{
    namespace Reflection
    {
        namespace Internal
        {
            template<class Class, typename MemberType>
            struct PropertyImpl
            {
                constexpr PropertyImpl(MemberType Class::* aMember, const char* aName) : member{ aMember }, name{ aName } {}
                MemberType Class::* member;
                const char* name;
                using Type = MemberType;
            };
            
            template <typename T, T... S, typename F>
            constexpr void ForIntSequence(std::integer_sequence<T, S...>, F&& f) {
                using unpack_t = int[];
                (void)unpack_t {
                    (static_cast<void>((void)f(std::integral_constant<T, S>{})), 0)..., 0
                };
            }
            
            template<typename Class, typename T>
            constexpr auto DefineProperty(T Class::* member, const char* name) {
                return PropertyImpl<Class, T>{member, name};
            }
            
            template <class T, typename = std::void_t<>>
            struct IsCXReflectable : std::false_type {
            };
            template <class T>
            struct IsCXReflectable<T, std::void_t<decltype(T::HasCXProps)>> : std::true_type {};
            
            template <class T, typename = std::void_t<>>
            struct IsCXReference : std::false_type {
            };
            template <class T>
            struct IsCXReference<T, std::void_t<decltype(T::CXReference)>> : std::true_type {};
            
            template <class T, typename = std::void_t<>>
            struct IsCXOptional : std::false_type {
            };
            template <class T>
            struct IsCXOptional<T, std::void_t<decltype(T::CXOptional)>> : std::true_type {};
            
            template <class T, typename = std::void_t<>>
            struct IsCXCustom : std::false_type {
            };
            template <class T>
            struct IsCXCustom<T, std::void_t<decltype(T::CXCustom)>> : std::true_type {};
            
            template <class T, typename = std::void_t<>>
            struct IsStdVector : std::false_type {
            };
            template<typename T, typename A>
            struct IsStdVector<std::vector<T, A>> : std::true_type {};
        }
        
        class NullReferenceException : public std::exception
        {
        public:
            const char* what() { return "CX::Reflection::Reference used without being set"; };
        };
        
        // Used for incomplete/mutual-reference types: do NOT set those manually and check whether they exist before reading
        template<class T>
        class Reference
        {
        public:
            bool CXReference = true; // ref marker
            using ValueType = T;
            
            Reference() : mPointer(nullptr) {}
            Reference(const Reference& ref) { mPointer = new T; if (ref.mPointer) *mPointer = *ref.mPointer; }
            // ~Reference() { if(mPointer) delete mPointer; }
            // FIX MEMORY LEAKS HERE!
            
            void Create() { mPointer = new T; }
            bool Exists() const { return (mPointer != nullptr); }
            
            T& operator()() { if (mPointer) return *mPointer; else throw NullReferenceException(); }
            operator T& () { if (mPointer) return *mPointer; else throw NullReferenceException(); }
            operator const T& () const { if (mPointer) return *mPointer; else throw NullReferenceException(); }
            void Set(const T& val) { Create(); *mPointer = val; }
        private:
            T* mPointer;
        };
        
        // Optional fields for serialization: can be used as an ordinary value. operator() to get the value explicitly
        template<class T>
        class Optional
        {
        public:
            bool CXOptional = true; // opt marker
            using ValueType = T;
            
            Optional() : mExists(false), mValue() {}
            Optional(const T& val) : mExists(true), mValue(val) {}
            bool Exists() const { return mExists; }
            Optional& operator=(T& val) { mExists = true; mValue = val; return *this; }
            Optional& operator=(const T& val) { mExists = true; mValue = val; return *this; }
            
            void Set(const T& val) { mValue = val; mExists = true; }
            
            T& operator()() { return mValue; }
            const T& operator()() const { return mValue; }
            operator T& () { return mValue; }
            operator const T& () const { return mValue; }
        private:
            bool mExists;
            T mValue;
        };
        
        template<typename Type>
        class CustomSerializable
        {
        public:
            bool CXCustom = true; // cust marker
            
            virtual void Deserialize(const std::string& str) = 0;
            virtual std::string Serialize() const = 0;
            
            Type& operator()() { return mValue; }
        protected:
            Type mValue;
        };
    }
    
    template<class Format, class T>
    Format SerializeObject(const T& obj)
    {
        Format j;
        CXENUM_CONST(T, obj) {
            if constexpr (Reflection::Internal::IsCXCustom<type>::value)
            {
                j[name] = SerializeObject<Format>(value);
            }
            else
            {
                if constexpr (Reflection::Internal::IsStdVector<type>::value)
                {
                    using vectype = typename type::value_type;
                    
                    if constexpr (Reflection::Internal::IsCXReflectable<vectype>::value)
                    {
                        for (auto& val : value)
                            j[name].push_back(SerializeObject<Format>(val));
                    }
                    else
                    {
                        for (auto& val : value)
                            j[name].push_back((vectype)val);
                    }
                }
                else
                {
                    if constexpr (Reflection::Internal::IsCXReflectable<type>::value)
                        j[name] = SerializeObject<Format>(value);
                    else
                        j[name] = value;
                }
            }
        } CXENUM_END;
        return j;
    }
        
    template<class T, class Format>
    static T DeserializeObject(const Format& j)
    {
        T obj;
        DeserializeObject<T, Format>(j, obj);
        return obj;
    }
    
    template<class T, class Format>
    static void DeserializeObject(const Format& j, T& obj)
    {
        CXENUM(T, obj)
        {
            try {
                
                if constexpr (Reflection::Internal::IsCXCustom<type>::value)
                {
                    DeserializeObject<Format>(j.at(name));
                }
                else if constexpr (Reflection::Internal::IsCXReference<type>::value)
                {
                    j.at(name);
                    value.Create();
                    using vtype = typename type::ValueType;
                    
                    if constexpr (Reflection::Internal::IsCXReflectable<vtype>::value)
                        value.Set(DeserializeObject<vtype>(j.at(name)));
                    else
                        value = j.at(name).template get<vtype>();
                }
                else if constexpr (Reflection::Internal::IsCXOptional<type>::value)
                {
                    using vtype = typename type::ValueType;
                    
                    if constexpr (Reflection::Internal::IsStdVector<vtype>::value)
                    {
                        using vectype = typename vtype::value_type;
                        
                        if constexpr (Reflection::Internal::IsCXReflectable<vectype>::value)
                        {
                            for (auto& val : j.at(name))
                                value().push_back(DeserializeObject<vectype>(val));
                        }
                        else
                        {
                            for (auto& val : j.at(name))
                                value().push_back(val.template get<vectype>());
                        }
                    }
                    else
                    {
                        if constexpr (Reflection::Internal::IsCXReflectable<vtype>::value)
                            value = DeserializeObject<vtype>(j.at(name));
                        else
                            value = j.at(name).template get<vtype>();
                    }
                }
                else
                {
                    if constexpr (Reflection::Internal::IsStdVector<type>::value)
                    {
                        using vectype = typename type::value_type;
                        
                        if constexpr (Reflection::Internal::IsCXReflectable<vectype>::value)
                        {
                            for (auto& val : j.at(name))
                                value.push_back(DeserializeObject<vectype>(val));
                        }
                        else
                        {
                            for (auto& val : j.at(name))
                                value.push_back(val.template get<vectype>());
                        }
                    }
                    else
                    {
                        if constexpr (Reflection::Internal::IsCXReflectable<type>::value)
                            value = DeserializeObject<type>(j.at(name));
                        else
                            value = j.at(name).template get<type>();
                    }
                }
            }
            catch (...)
            {
                /* ignore non existent fields */
            }
        } CXENUM_END;
    }
}

template<class T>
using cxref = CX::Reflection::Reference<T>;
template<class T>
using cxopt = CX::Reflection::Optional<T>;
