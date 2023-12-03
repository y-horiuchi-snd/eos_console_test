#include "eos_account.h"
#include <cassert>

namespace eos
{
    /// <summary>
    /// 指定のepicアカウントを文字列に変換する
    /// </summary>
    std::string AccountHelper::EpicAccountIDToString(EOS_EpicAccountId InAccountId)
    {
        [[maybe_unused]] EOS_EResult result;

        std::string r;
        r.resize(EOS_EPICACCOUNTID_MAX_LENGTH);

        int32_t s = (int32_t)(r.size() + 1);
        result    = EOS_EpicAccountId_ToString(InAccountId, &r[0], &s);
        assert(result == EOS_EResult::EOS_Success);
        r.resize(s - 1);

        return r;
    }

    /// <summary>
    /// 指定のIDを文字列に変換する
    /// </summary>
    std::string AccountHelper::ProductUserIDToString(EOS_ProductUserId InAccountId)
    {
        [[maybe_unused]] EOS_EResult result;

        std::string r;
        r.resize(EOS_PRODUCTUSERID_MAX_LENGTH);

        int32_t s = (int32_t)(r.size() + 1);
        result    = EOS_ProductUserId_ToString(InAccountId, &r[0], &s);
        assert(result == EOS_EResult::EOS_Success);
        r.resize(s - 1);

        return r;
    }

    /// <summary>
    /// 文字列からepicアカウントへ変換する
    /// </summary>
    EOS_EpicAccountId AccountHelper::EpicAccountIDFromString(const char* AccountString)
    {
        return EOS_EpicAccountId_FromString(AccountString);
    }

} // namespace eos
