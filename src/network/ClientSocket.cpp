#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include "ClientSocket.h"
#include "Log.h"

ClientSocket::ClientSocket(rxInt fd, string ip, rxUShort port) : mSocket(fd),
																	mIP(ip), mPort(port)
{
	if(fd == -1)
		mConnected = false;
	else
		mConnected = true;
}

ClientSocket::~ClientSocket()
{
}

rxInt ClientSocket::Connect()
{
	INFO(Log::eNetwork, "Attempting to connect %s:%d", mIP.c_str(), mPort);
	struct sockaddr_in peer;

	if(mSocket == -1 || !isConnected())
	{
		mSocket = socket(AF_INET, SOCK_STREAM, 0);
		if(mSocket == -1) {
			mConnected = false;
			return -1;
		}

		/*Socket has been created*/
		else {
			if(inet_addr(mIP.c_str()) == -1) {
				struct hostent* he;
				struct in_addr **addr_list;

				//resolve the hostname, its not an ip address
				if ( (he = gethostbyname( mIP.c_str() ) ) == NULL)
				{
					ERROR(Log::eNetwork, "Failed to get host by name.");
					mConnected = false;
					return -1;
				}

			   //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
				addr_list = (struct in_addr **) he->h_addr_list;

				for(int i = 0; addr_list[i] != NULL; i++)
				{
					//strcpy(ip , inet_ntoa(*addr_list[i]) );
					peer.sin_addr = *addr_list[i];
					INFO(Log::eNetwork, "address %s resolved to %s", mIP.c_str(), inet_ntoa(*addr_list[i]));

					break;
				}
			}

			/*we already have an ip adress*/
			else {
				peer.sin_addr.s_addr = inet_addr( mIP.c_str() );
			}

			peer.sin_family = AF_INET;
			peer.sin_port = htons(mPort);

			/*
			 * Connect to the peer
			 */
		    //Connect to remote server
		    if (connect(mSocket , (struct sockaddr *)&peer , sizeof(peer)) < 0)
		    {
		        ERROR(Log::eNetwork, "Failed to connect to %s", mIP.c_str());
		        mConnected = false;
		        return -1;
		    }

		    INFO(Log::eNetwork, "Connected to Pushfyi at %s:%d", mIP.c_str(), mPort);
		    mConnected = true;

		    return 0;
		}
	}
	return 0;
}

void ClientSocket::Close()
{
	close(mSocket);
	mSocket = -1;

	mConnected = false;
}
