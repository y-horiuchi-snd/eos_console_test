#pragma once

#include <eos_sdk.h>
#include <string>

namespace eos
{

    class AccountHelper
    {
    public:
        /// <summary>
        /// 指定のepicアカウントを文字列に変換する
        /// </summary>
        static std::string EpicAccountIDToString(EOS_EpicAccountId InAccountId);
        /// <summary>
        /// 指定のIDを文字列に変換する
        /// </summary>
        static std::string ProductUserIDToString(EOS_ProductUserId InAccountId);

        /// <summary>
        /// 文字列からepicアカウントへ変換する
        /// </summary>
        static EOS_EpicAccountId EpicAccountIDFromString(const char* AccountString);
    };

    /// <summary>
    /// IDユーティリティクラス
    /// </summary>
    template <typename T> struct Account
    {
    };

    /// <summary>
    /// ProductUserID用
    /// </summary>
    template <> struct Account<EOS_ProductUserId>
    {
        static bool IsValid(const EOS_ProductUserId ProductUserId) { return EOS_ProductUserId_IsValid(ProductUserId); }

        static std::string ToString(const EOS_ProductUserId ProductUserId)
        {
            return AccountHelper::ProductUserIDToString(ProductUserId);
        }
    };

    /// <summary>
    /// Epicアカウント用
    /// </summary>
    template <> struct Account<EOS_EpicAccountId>
    {
        static bool IsValid(const EOS_EpicAccountId AccountId) { return EOS_EpicAccountId_IsValid(AccountId); }

        static std::string ToString(const EOS_EpicAccountId AccountId)
        {
            return AccountHelper::EpicAccountIDToString(AccountId);
        }
    };

    /// <summary>
    /// ID管理用のユーティリティクラス
    /// </summary>
    template <class AccountType> struct EpicAccount
    {
    private:
        AccountType m_account_id = nullptr;

    public:
        EpicAccount(AccountType InAccountId) : m_account_id(InAccountId) {}
        EpicAccount()                              = default;
        EpicAccount(const EpicAccount&)            = default;
        EpicAccount& operator=(const EpicAccount&) = default;

        /// <summary>
        /// アカウントが同一であるか？
        /// </summary>
        bool operator==(const EpicAccount& Other) const { return m_account_id == Other.m_account_id; }

        /// <summary>
        /// アカウントが異なっているか？
        /// </summary>
        bool operator!=(const EpicAccount& Other) const { return !(this->operator==(Other)); }

        bool operator<(const EpicAccount& Other) const { return m_account_id < Other.m_account_id; }

        /// <summary>
        /// IDが有効かどうか
        /// </summary>
        operator bool() const { return IsValid(); }

        operator AccountType() const { return m_account_id; }

        /// <summary>
        /// 文字列へ変換する
        /// </summary>
        std::string ToString() const { return Account<AccountType>::ToString(m_account_id); }

        /// <summary>
        /// IDが有効かどうか
        /// </summary>
        bool IsValid() const { return Account<AccountType>::IsValid(m_account_id); };
    };

} // namespace eos
