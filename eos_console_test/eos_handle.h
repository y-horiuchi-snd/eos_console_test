#pragma once

#include <memory>

namespace eos
{

    /// @brief EOSハンドルを安全に管理するためのオブジェクト
    template <typename TValue> class Handle
    {
    public:
        /// @brief ハンドル開放用のメソッド
        using RELEASE_METHOD = void (*)(TValue);

    private:
        /// @brief ハンドルを実際に管理するためのオブジェクト
        class Object
        {
            TValue         m_handle;
            RELEASE_METHOD m_release;

        public:
            Object(TValue handle, RELEASE_METHOD release) : m_handle(handle), m_release(release) {}
            ~Object() { Release(); }

            void Release()
            {
                if (m_handle)
                {
                    m_release(m_handle);
                    m_handle = nullptr;
                }
            }

            TValue GetHandle() { return m_handle; }
        };

        std::shared_ptr<Object> m_object;

    public:
        Handle() {}
        Handle(TValue handle, RELEASE_METHOD release) { Initialize(handle, release); }

        /// @brief 初期化するハンドルと解放方法を指定する
        /// @param handle 登録するハンドル
        /// @param release 解放処理
        void Initialize(TValue handle, RELEASE_METHOD release) { m_object = std::make_shared<Object>(handle, release); }

        /// @brief 不要になったので開放する
        void Release() { m_object.reset(); }

        /// @brief 中身が存在するかどうか
        operator bool() const noexcept { return (bool)m_object; }
        bool operator!() const noexcept { return !m_object; }

        /// @brief ハンドルを参照する
        TValue operator*() const { return m_object->GetHandle(); }
        TValue operator->() const { return m_object->GetHandle(); }
        operator TValue() const { return m_object->GetHandle(); }
    };

} // namespace eos
