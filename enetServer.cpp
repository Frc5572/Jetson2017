#include "enet.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <sstream>
#include <cstring>
#include <thread>

using namespace std;

volatile bool quitting = false;

void (*cevent)(ENetPeer*, ENetHost*, ENetEvent) = nullptr;
void (*revent)(ENetPeer*, ENetHost*, ENetEvent) = nullptr;
void (*devent)(ENetPeer*, ENetHost*, ENetEvent) = nullptr;

void server::connect(void (*a)(ENetPeer*, ENetHost*, ENetEvent)){cevent = a;}
void server::recieve(void (*a)(ENetPeer*, ENetHost*, ENetEvent)){revent = a;}
void server::disconnect(void (*a)(ENetPeer*, ENetHost*, ENetEvent)){devent = a;}

void server::send(std::string data, ENetPeer* peer){
	ENetPacket *packet = enet_packet_create(data.c_str(), data.size()+1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer,0,packet);
}

void runThread(int port){
	ENetHost *server;
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = port;
	server = enet_host_create(&address, 32, 2, 0, 0);
	ENetEvent talking;
	while(!quitting){
		enet_host_service(server, &talking, 1000);
		switch(talking.type){
		case ENET_EVENT_TYPE_CONNECT:
			if(cevent != nullptr)
				cevent(talking.peer, server, talking);
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			if(revent != nullptr)
				revent(talking.peer, server, talking);		
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			if(devent != nullptr)
				devent(talking.peer, server, talking);
			break;
		default: break;
		}
		enet_host_flush(server);
	}
	enet_host_destroy(server);
	enet_deinitialize();
}

std::thread thr;

void server::init(int port) {
	thr = std::thread(runThread, port);
}

void server::quit(){
	quitting = true;
	thr.join();
}
