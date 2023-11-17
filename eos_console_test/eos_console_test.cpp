// eos_console_test.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include "../credentials.h"
#include <eos_sdk.h>
#include <malloc.h>
#include <cassert>
#include <windows.h>
#include "eos_error.h"
#include "eos_handle.h"

#pragma comment(lib, "EOSSDK-Win64-Shipping.lib")

class Memory
{
public:
    static void* AllocateFunc(size_t SizeInBytes, size_t Alignment);
    static void* ReallocateFunc(void* p, size_t SizeInBytes, size_t Alignment);
    static void  ReleaseFunc(void* p);
};

const char* GetTempDirectory();

int main()
{
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

    platform.Release();
    {
        const eos::Error r = EOS_Shutdown();
        assert(r.IsSuccess());
    }

    return 0;
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
