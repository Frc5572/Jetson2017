#ifndef ENET_CLIENT_H_
#define ENET_CLIENT_H_

#include <enet/enet.h>
#include <string>

namespace client {
	void init(std::string, int, bool = true);
	void send(std::string, ENetPeer*);
	void send(enet_uint8*, unsigned int, ENetPeer*);
	void connect(void (*)(ENetPeer*, ENetHost*, ENetEvent));
	void recieve(void (*)(ENetPeer*, ENetHost*, ENetEvent));
	void disconnect(void (*)(ENetPeer*, ENetHost*, ENetEvent));
	void quit();
}

namespace server {
	void init(int port);
	void send(std::string, ENetPeer*);
	void connect(void (*)(ENetPeer*, ENetHost*, ENetEvent));
	void recieve(void (*)(ENetPeer*, ENetHost*, ENetEvent));
	void disconnect(void (*)(ENetPeer*, ENetHost*, ENetEvent));
	void quit();
}

#endif
