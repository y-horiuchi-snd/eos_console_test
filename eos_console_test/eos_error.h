#pragma once

#include <stdint.h>
#include <eos_common.h>

namespace eos
{
    /// @brief EOSで発生したエラーを一元管理するための簡易クラス追加
    struct Error
    {
        enum Type
        {
            NONE,   // 未設定
            EOS,    // EOSの実行結果が格納されている
            APP,    // アプリケーション起因のエラー
            WINDOWS // OS起因のエラー
        };

        // アプリケーション内固有のエラー状態
        enum AppError
        {
            APPERROR_NONE, // 未定義のエラー
        };

        Type     m_type;  ///!< エラー種別
        uint32_t m_error; ///!< 詳細なエラー情報

        Error() : m_type(NONE), m_error(0) {}
        Error(Type t, uint32_t e) : m_type(t), m_error(e) {}
        Error(EOS_EResult e) : m_type(EOS), m_error((uint32_t)e) {}
        Error(AppError e) : m_type(APP), m_error((uint32_t)e) {}

        void Clear()
        {
            m_type  = NONE;
            m_error = 0;
        }

        /// @brief 結果は格納されているか
        /// @retval true 格納されている
        /// @retval false 格納されていない
        bool IsComplete() const { return m_type != NONE; }

        /// @brief 成功したか
        /// @retval true 成功
        /// @retval false 失敗（未実行もこちら）
        bool IsSuccess() const
        {
            switch (m_type)
            {
                case EOS:
                    return (EOS_EResult)m_error == EOS_EResult::EOS_Success;
                case APP:
                    // APPになった場合は必ず失敗している
                    return false;
                case WINDOWS:
                    // OSエラーが設定されているので失敗している
                    return false;
                default:
                    break;
            }
            return false;
        }

        static Error MakeEOS(EOS_EResult e) { return Error(e); }
        static Error MakeAPP(AppError e) { return Error(e); }
        static Error MakeWin32(unsigned long e) { return Error(Type::WINDOWS, e); }
    };
} // namespace eos
