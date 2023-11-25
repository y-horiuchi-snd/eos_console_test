// eos_console_test.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include "../credentials.h"
#include <malloc.h>
#include <cassert>
#include <string>
#include <windows.h>

#include <eos_sdk.h>
#include <eos_auth.h>
#include "eos_error.h"
#include "eos_handle.h"

#include <iostream>

#pragma comment(lib, "EOSSDK-Win64-Shipping.lib")

class Memory
{
public:
    static void* AllocateFunc(size_t SizeInBytes, size_t Alignment);
    static void* ReallocateFunc(void* p, size_t SizeInBytes, size_t Alignment);
    static void  ReleaseFunc(void* p);
};

const char* GetTempDirectory();

std::string input(const std::string_view& ask)
{
    std::cout << ask << std::endl;

    std::string r;
    std::cin >> r;
    return r;
}

EOS_ELoginCredentialType     GetCredentialType();
eos::Handle<EOS_Auth_Token*> CopyUserAuthToken(EOS_HAuth auth, EOS_EpicAccountId id);

::eos::Handle<EOS_HPlatform> g_platform;

// 簡易の非同期完了待ちとデータの受け取りをするためのクラス
template <typename T> class Async
{
    T          m_storage; // 受け渡し用のストレージ
    eos::Error m_error;   // 動作とエラー状態

public:
    void              SetComplete(eos::Error e) { m_error = e; }
    const eos::Error& GetError() const { return m_error; }

    void     SetStorage(T storage) { m_storage = storage; }
    const T& GetStorage() const { return m_storage; }

    // 動作完了待機
    void Wait()
    {
        while (true)
        {
            if (!g_platform)
            {
                // エラー
                return;
            }

            // eosの内部進行を進めるために呼び出す必要がある
            EOS_Platform_Tick(g_platform);

            if (m_error.IsComplete())
            {
                // 完了フラグが立てられたら抜ける
                return;
            }
            ::Sleep(10);
        }
    }
};

int main()
{
    // プラットフォームハンドルを作成する
    ::eos::Handle<EOS_HPlatform> platform;
    {
        EOS_InitializeOptions options    = {};
        options.ApiVersion               = EOS_INITIALIZE_API_LATEST;
        options.AllocateMemoryFunction   = Memory::AllocateFunc;
        options.ReallocateMemoryFunction = Memory::ReallocateFunc;
        options.ReleaseMemoryFunction    = Memory::ReleaseFunc;
        options.ProductName              = SampleConstants::GameName;
        options.ProductVersion           = "1.0";
        options.Reserved                 = nullptr;
        options.SystemInitializeOptions  = nullptr;
        options.OverrideThreadAffinity   = nullptr;

        const eos::Error r = EOS_Initialize(&options);
        assert(r.IsSuccess());
    }

    {
        EOS_Platform_Options options = {};
        options.ApiVersion           = EOS_PLATFORM_OPTIONS_API_LATEST;
        options.bIsServer            = EOS_FALSE;
        options.EncryptionKey        = SampleConstants::EncryptionKey;
        options.OverrideCountryCode  = nullptr;
        options.OverrideLocaleCode   = nullptr;
        options.Flags                = 0;
        options.CacheDirectory       = GetTempDirectory();

        options.ProductId    = SampleConstants::ProductId;
        options.SandboxId    = SampleConstants::SandboxId;
        options.DeploymentId = SampleConstants::DeploymentId;

        options.ClientCredentials.ClientId     = SampleConstants::ClientCredentialsId;
        options.ClientCredentials.ClientSecret = SampleConstants::ClientCredentialsSecret;

        platform.Initialize(EOS_Platform_Create(&options), EOS_Platform_Release);
        assert((bool)platform);
    }
    g_platform = platform;

    EOS_Auth_Credentials auth_credentials = {};

    std::string _auth_id, _auth_token;

    auth_credentials.ApiVersion = EOS_AUTH_CREDENTIALS_API_LATEST;

    // 利用したい認証を取得する
    auth_credentials.Type = GetCredentialType();

    switch (auth_credentials.Type)
    {
        case EOS_ELoginCredentialType::EOS_LCT_Password:
            _auth_id               = input("ユーザー名を入力してください");
            _auth_token            = input("パスワードを入力してください");
            auth_credentials.Id    = _auth_id.c_str();
            auth_credentials.Token = _auth_token.c_str();
            break;
        case EOS_ELoginCredentialType::EOS_LCT_Developer:
            _auth_id            = input("DevNameを入力してください");
            auth_credentials.Id = _auth_id.c_str();
            break;
        case EOS_ELoginCredentialType::EOS_LCT_AccountPortal:
            break;
        default:
            return 255;
    }

    // ログインを行い認証情報を得る
    eos::Handle<EOS_Auth_Token*> auth_token;

    {
        EOS_HAuth auth = EOS_Platform_GetAuthInterface(platform);

        EOS_Auth_LoginOptions options = {};

        options.ApiVersion  = EOS_AUTH_LOGIN_API_LATEST;
        options.ScopeFlags  = EOS_EAuthScopeFlags::EOS_AS_NoFlags;
        options.Credentials = &auth_credentials;

        Async<EOS_Auth_LoginCallbackInfo> async;

        EOS_Auth_Login(auth,
                       &options,
                       &async,
                       [](const EOS_Auth_LoginCallbackInfo* data)
                       {
                           auto async = (Async<EOS_Auth_LoginCallbackInfo>*)data->ClientData;

                           if (!EOS_EResult_IsOperationComplete(data->ResultCode))
                           {
                               return;
                           }
                           async->SetStorage(*data);
                           async->SetComplete(eos::Error(data->ResultCode));
                       });

        async.Wait();

        assert(async.GetError().IsSuccess());

        // 認証用のトークンを保存する
        auth_token = CopyUserAuthToken(auth, async.GetStorage().LocalUserId);
    }

    // 認証情報を利用して接続を行う
    {
        auto connect = EOS_Platform_GetConnectInterface(platform);

        EOS_Connect_Credentials  credentials = {};
        EOS_Connect_LoginOptions options     = {};

        credentials.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
        credentials.Token      = auth_token->AccessToken;
        credentials.Type       = EOS_EExternalCredentialType::EOS_ECT_EPIC;

        options.ApiVersion    = EOS_CONNECT_LOGIN_API_LATEST;
        options.Credentials   = &credentials;
        options.UserLoginInfo = nullptr;

        Async<EOS_Connect_LoginCallbackInfo> async;
        EOS_Connect_Login(connect,
                          &options,
                          &async,
                          [](const EOS_Connect_LoginCallbackInfo* data)
                          {
                              auto async = (Async<EOS_Connect_LoginCallbackInfo>*)data->ClientData;

                              if (!EOS_EResult_IsOperationComplete(data->ResultCode))
                              {
                                  return;
                              }
                              async->SetStorage(*data);
                              async->SetComplete(eos::Error(data->ResultCode));
                          });
        async.Wait();

        assert(async.GetError().IsSuccess());
    }

    // ロビーを作成する
    // 属性を設定する

    // 検索
    // ロビー参加

    platform.Release();
    g_platform.Release();
    {
        const eos::Error r = EOS_Shutdown();
        assert(r.IsSuccess());
    }

    return 0;
}

/// @brief 認証タイプを取得する
EOS_ELoginCredentialType GetCredentialType()
{
    // ログインのための情報を受け取る
    puts("ログインタイプを選択");
    do
    {
        auto _login_type = input("0:EOS_LCT_Password , 1:EOS_LCT_Developer , 2:EOS_LCT_AccountPortal");

        char* endptr = nullptr;

        auto v = strtol(_login_type.c_str(), &endptr, 10);
        if (strlen(endptr) == 0)
        {
            switch (v)
            {
                case 0:
                    return EOS_ELoginCredentialType::EOS_LCT_Password;
                case 1:
                    return EOS_ELoginCredentialType::EOS_LCT_Developer;
                case 2:
                    return EOS_ELoginCredentialType::EOS_LCT_AccountPortal;
                default:
                    break;
            }
        }
    } while (true);
    assert(false);
    return EOS_ELoginCredentialType::EOS_LCT_AccountPortal;
}

eos::Handle<EOS_Auth_Token*> CopyUserAuthToken(EOS_HAuth auth, EOS_EpicAccountId id)
{
    EOS_Auth_CopyUserAuthTokenOptions CopyTokenOptions = {0};
    CopyTokenOptions.ApiVersion                        = EOS_AUTH_COPYUSERAUTHTOKEN_API_LATEST;

    EOS_Auth_Token* UserAuthToken = nullptr;
    EOS_Auth_CopyUserAuthToken(auth, &CopyTokenOptions, id, &UserAuthToken);

    return eos::Handle<EOS_Auth_Token*>(UserAuthToken, EOS_Auth_Token_Release);
}

void* Memory::AllocateFunc(size_t SizeInBytes, size_t Alignment)
{
#if defined(_WIN32)
    return _aligned_malloc(SizeInBytes, Alignment);
#else
    return aligned_alloc(Alignment, SizeInBytes);
#endif
}
void* Memory::ReallocateFunc(void* p, size_t SizeInBytes, size_t Alignment)
{
#if defined(_WIN32)
    return _aligned_realloc(p, SizeInBytes, Alignment);
#else
    return reallocalign(p, SizeInBytes, Alignment);
#endif
}
void Memory::ReleaseFunc(void* p)
{
#if defined(_WIN32)
    _aligned_free(p);
#else
    free(p);
#endif
}

const char* GetTempDirectory()
{
#ifdef _WIN32
    static char Buffer[1024] = {0};
    if (Buffer[0] == 0)
    {
        GetTempPathA(sizeof(Buffer), Buffer);
    }

    return Buffer;

#elif defined(__APPLE__)
    return "/private/var/tmp";
#else
    return "/var/tmp";
#endif
}
