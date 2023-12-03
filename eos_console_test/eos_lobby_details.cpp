#include "eos_lobby_details.h"
#include <cassert>
#include <eos_lobby.h>

namespace eos
{
    namespace lobby
    {
        Attribute::Attribute(_tagEOS_Lobby_Attribute* attr) : m_attr(attr, EOS_Lobby_Attribute_Release) {}
        void Attribute::Release() { m_attr.Release(); }

        EOS_ELobbyAttributeType Attribute::GetType() const { return (*m_attr)->Data->ValueType; }

        const char* Attribute::GetName() const { return (*m_attr)->Data->Key; }

        int64_t Attribute::AsInt64() const
        {
            assert((*m_attr)->Data->ValueType == EOS_ELobbyAttributeType::EOS_AT_INT64);
            return (*m_attr)->Data->Value.AsInt64;
        }
        double Attribute::AsDouble() const
        {
            assert((*m_attr)->Data->ValueType == EOS_ELobbyAttributeType::EOS_AT_DOUBLE);
            return (*m_attr)->Data->Value.AsDouble;
        }
        bool Attribute::AsBool() const
        {
            assert((*m_attr)->Data->ValueType == EOS_ELobbyAttributeType::EOS_AT_BOOLEAN);
            return (*m_attr)->Data->Value.AsBool == EOS_TRUE;
        }
        const char* Attribute::AsUtf8() const
        {
            assert((*m_attr)->Data->ValueType == EOS_ELobbyAttributeType::EOS_AT_STRING);
            return (*m_attr)->Data->Value.AsUtf8;
        }

        std::string Attribute::Dump() const
        {
            std::string r;

            switch (GetType())
            {
                case EOS_ELobbyAttributeType::EOS_AT_BOOLEAN:
                    r = AsBool() ? "true" : "false";
                    break;
                case EOS_ELobbyAttributeType::EOS_AT_DOUBLE:
                    r = std::to_string(AsDouble());
                    break;
                case EOS_ELobbyAttributeType::EOS_AT_INT64:
                    r = std::to_string(AsInt64());
                    break;
                case EOS_ELobbyAttributeType::EOS_AT_STRING:
                    r = AsUtf8();
                    break;
            }

            return r;
        }

        Details::Details(EOS_LobbyDetailsHandle* details) : m_details(details, EOS_LobbyDetails_Release)
        {
            internal_MakeAttributes(*m_details, m_attributes);
        }
        Details::Details(eos::Handle<EOS_LobbyDetailsHandle*> details) : m_details(details) {}
        void Details::Release() { m_details.Release(); }

        uint32_t Details::GetMemberCount() const
        {
            EOS_LobbyDetails_GetMemberCountOptions count_options;
            count_options.ApiVersion = EOS_LOBBYDETAILS_GETMEMBERCOUNT_API_LATEST;

            return EOS_LobbyDetails_GetMemberCount(*m_details, &count_options);
        }

        std::shared_ptr<Attribute> Details::GetAttribute(const std::string& key) const
        {
            auto iter = m_attributes.find(key);
            if (iter == m_attributes.end())
            {
                return nullptr;
            }

            return iter->second;
        }
        std::shared_ptr<Attribute> Details::GetAttribute(const std::string& key, EOS_ELobbyAttributeType type) const
        {
            auto a = GetAttribute(key);
            if (!a)
            {
                return nullptr;
            }
            if (a->GetType() != type)
            {
                return nullptr;
            }

            return a;
        }

        EOS_ProductUserId Details::GetOwnerId() const
        {
            EOS_LobbyDetails_GetLobbyOwnerOptions options;
            options.ApiVersion = EOS_LOBBYDETAILS_GETLOBBYOWNER_API_LATEST;

            return EOS_LobbyDetails_GetLobbyOwner(*m_details, &options);
        }

        EOS_ProductUserId Details::GetMemberId(uint32_t index) const
        {
            EOS_LobbyDetails_GetMemberByIndexOptions options;
            options.ApiVersion  = EOS_LOBBYDETAILS_GETMEMBERBYINDEX_API_LATEST;
            options.MemberIndex = index;

            return EOS_LobbyDetails_GetMemberByIndex(*m_details, &options);
        }

        void Details::internal_MakeAttributes(EOS_LobbyDetailsHandle* details_handle, Attributes& r)
        {

            EOS_LobbyDetails_GetAttributeCountOptions count_options;
            count_options.ApiVersion = EOS_LOBBYDETAILS_GETATTRIBUTECOUNT_API_LATEST;
            const auto count         = EOS_LobbyDetails_GetAttributeCount(details_handle, &count_options);

            EOS_LobbyDetails_CopyAttributeByIndexOptions copy_index_options;
            copy_index_options.ApiVersion = EOS_LOBBYDETAILS_COPYATTRIBUTEBYINDEX_API_LATEST;
            for (uint32_t i = 0; i < count; i++)
            {
                copy_index_options.AttrIndex = i;

                EOS_Lobby_Attribute* attr;
                EOS_LobbyDetails_CopyAttributeByIndex(details_handle, &copy_index_options, &attr);

                auto p          = std::make_shared<Attribute>(attr);
                r[p->GetName()] = p;
            }
        }

    } // namespace lobby
} // namespace eos
