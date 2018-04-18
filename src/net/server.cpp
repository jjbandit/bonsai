#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write

#include <bonsai_types.h>

inline void
CheckForConnectingClient(socket_t *ListeningSocket, network_connection *ClientConnection)
{
  Assert(ListeningSocket->Type == Socket_NonBlocking);

  u32 AddressSize = sizeof(ClientConnection->Address);
  s32 SocketId =
    accept4(ListeningSocket->Id,
        (sockaddr*)&ClientConnection->Address,
        &AddressSize,
        SOCK_NONBLOCK);

  // The accept() call overwrites this value to let us know how many bytes it
  // wrote to ClientConnection.Address
  Assert(AddressSize == sizeof(ClientConnection->Address));

  if (SocketId < 0)
  {
    switch(errno)
    {
      case EAGAIN:
      {
        // No incoming connections
      } break;

      default:
      {
        Error("Accept Failed");
      } break;
    }
  }
  else
  {
    Debug("Connection accepted");
    ClientConnection->Socket.Id = SocketId;
    ClientConnection->Socket.Type = Socket_NonBlocking;
    ClientConnection->Connected = True;
  }

  return;
}

#define MAX_CLIENTS 2

int
main(int ArgCount, char **Arguments)
{
  network_connection IncomingConnections = { Socket_NonBlocking };

  s32 BindResult =
    bind(IncomingConnections.Socket.Id,
        (sockaddr *)&IncomingConnections.Address,
        sizeof(IncomingConnections.Address));

  if( BindResult < 0)
    { Error("Bind Failed"); return 1; }

  Debug("Bind Successful");

  listen(IncomingConnections.Socket.Id, 3);

  Debug("Listening");

  network_connection ClientList[MAX_CLIENTS] = {};

  server_to_client_message OutputMessage = {};

  for(;;)
  {
    for (u32 ClientIndex = 0;
        ClientIndex < MAX_CLIENTS;
        ++ClientIndex)
    {
      network_connection *Connection = &ClientList[ClientIndex];
      if (IsConnected(Connection))
      {
        if (FlushIncomingMessages(Connection, &OutputMessage)
            == SocketOpResult_CompletedRW)
        {
          Send(Connection, &OutputMessage);
          Print(OutputMessage.P);
          Print(OutputMessage.Id);
        }
      }
      else
      {
        CheckForConnectingClient(&IncomingConnections.Socket, Connection);
      }
    }

  }

  return 0;
}