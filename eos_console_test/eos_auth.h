#pragma once

#include "eos_account.h"

struct AuthInfo
{
    EpicAccount<EOS_ProductUserId> m_product_user_id;
    PlatformId                     m_platform_id;

    std::string m_token; // 現在有効なトークン情報

    bool IsValid() const { return m_product_user_id.IsValid() && m_platform_id.IsValid(); };
};
