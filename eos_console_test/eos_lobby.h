#include <string>

namespace eos {

    class Lobby {
        EOS_HLobby     m_handle;
        EOS_ProductUserId m_local_user;
        std::string m_id;

    public:
        Lobby(EOS_ProductUserId local_user,EOS_LobbyId id);
        ~Lobby();

        void Leave();
    };

}
