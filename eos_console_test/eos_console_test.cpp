// eos_console_test.cpp
//

#include "../credentials.h"
#include "my_eos.h"

#pragma comment(lib, "EOSSDK-Win64-Shipping.lib")

int main()
{
    EOS eos;

    // 初期化とプラットフォームハンドルを作成する
    eos.Initialize();
    {
        // 認証
        auto auth = eos.Authorize();
        eos.Connect(auth);
    }

    std::vector<std::shared_ptr<EOS::Lobby>> lobbies;

    // ロビーを数個作成する
    for (int i = 0; i < 5; i++)
    {
        auto lobby = eos.LobbyCreate();
        eos.LobbySetAttributes(lobby, i, 1);
        lobbies.push_back(lobby);
    }

    // 反映されるまで少し時間がかかるようなので適当に待機します
    puts("wait");
    ::Sleep(10 * 1000);

    // 属性"test"が1のものを探す
    puts("search 1");
    {
        auto search = eos.LobbySearchCreate(5);

        EOS_Lobby_AttributeData attr;
        search->AddParameter(EOS::MakeAttribute(attr, "test", 1), EOS_EComparisonOp::EOS_CO_EQUAL);
        eos.LobbySearchExecute(search);

        eos.LobbySearchDump(search);
    }

    // 属性"test"が2以上、４未満のものを探す
    puts("search 2");
    {
        auto search = eos.LobbySearchCreate(5);

        EOS_Lobby_AttributeData attr;
        search->AddParameter(EOS::MakeAttribute(attr, "number", 2), EOS_EComparisonOp::EOS_CO_GREATERTHANOREQUAL);
        search->AddParameter(EOS::MakeAttribute(attr, "number", 4), EOS_EComparisonOp::EOS_CO_LESSTHAN);
        eos.LobbySearchExecute(search);

        eos.LobbySearchDump(search);
    }

    for (auto l : lobbies)
    {
        eos.LobbyLeave(l);
    }
    //eos.LobbyDestroy(lobby);

    std::cout << "Hello World!\n";

    eos.Finalize();

    return 0;
}
