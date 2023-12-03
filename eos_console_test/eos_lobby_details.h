#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "eos_handle.h"
#include <eos_sdk.h>

struct _tagEOS_Lobby_Attribute;
struct EOS_LobbyDetailsHandle;

namespace eos
{
    namespace lobby
    {

        /// <summary>
        /// ロビーで利用される属性情報を管理する
        /// </summary>
        class Attribute
        {
        private:
            Attribute(const Attribute&)            = delete;
            Attribute& operator=(const Attribute&) = delete;

        private:
            Handle<_tagEOS_Lobby_Attribute*> m_attr;

        public:
            Attribute(_tagEOS_Lobby_Attribute* attr);
            ~Attribute() { Release(); }

            /// <summary>
            /// 中身が有効かどうか
            /// </summary>
            operator bool() const { return (bool)m_attr; }

            /// <summary>
            /// 中身を開放する
            /// </summary>
            void Release();

            /// <summary>
            /// 中身の種類
            /// </summary>
            EOS_ELobbyAttributeType GetType() const;

            /// <summary>
            /// 属性の名前を取得する
            /// </summary>
            const char* GetName() const;

            /// <summary>
            ///
            /// </summary>
            int64_t     AsInt64() const;
            double      AsDouble() const;
            bool        AsBool() const;
            const char* AsUtf8() const;

            std::string Dump() const;
        };

        using Attributes = std::unordered_map<std::string, std::shared_ptr<Attribute>>;

        class Details
        {
            Handle<EOS_LobbyDetailsHandle*> m_details;
            Attributes                      m_attributes;

        private:
            static void internal_MakeAttributes(EOS_LobbyDetailsHandle* details_handle, Attributes& r);

        public:
            Details() {}
            Details(EOS_LobbyDetailsHandle* details);
            Details(Handle<EOS_LobbyDetailsHandle*> details);
            ~Details() { Release(); }
            void Release();

            bool IsValid() const { return (bool)m_details; }

            operator bool() const noexcept { return IsValid(); }

        public:
            /// <summary>
            /// クエリ発行時のメンバー数を取得する
            /// </summary>
            /// <returns>メンバー数</returns>
            uint32_t GetMemberCount() const;

            EOS_ProductUserId GetOwnerId() const;
            EOS_ProductUserId GetMemberId(uint32_t index) const;

            const Attributes&          GetAttributes() const { return m_attributes; }
            std::shared_ptr<Attribute> GetAttribute(const std::string& key) const;
            std::shared_ptr<Attribute> GetAttribute(const std::string& key, EOS_ELobbyAttributeType type) const;

            Handle<EOS_LobbyDetailsHandle*> Handle() const { return m_details; }

            template <typename T> T GetValue(const std::string& key, const T& default_value)
            {
                // static_assert(false, "not implement.");
            }

            template <> int64_t GetValue<int64_t>(const std::string& key, const int64_t& default_value)
            {
                if (auto p = GetAttribute(key))
                {
                    return p->AsInt64();
                }
                return default_value;
            }

            template <> bool GetValue<bool>(const std::string& key, const bool& default_value)
            {
                if (auto p = GetAttribute(key))
                {
                    return p->AsBool();
                }
                return default_value;
            }

            template <> std::string GetValue<std::string>(const std::string& key, const std::string& default_value)
            {
                if (auto p = GetAttribute(key))
                {
                    return std::string(p->AsUtf8());
                }
                return default_value;
            }
        };
    } // namespace lobby
} // namespace eos
