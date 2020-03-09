//#define SERVER_IP "34.214.168.166"
#define SERVER_IP "127.0.0.1"

#define REMOTE_PORT 1337

#define MAX_CLIENTS 2

struct client_state
{
  s32 Id;
  u32 Counter;
  canonical_position P;
};

struct server_state
{
  client_state Clients[MAX_CLIENTS];
};
typedef server_state server_to_client_message;

struct client_to_server_message
{
  client_state Client;
};

struct handshake_message
{
  s32 ClientId;
};

enum socket_type
{
  Socket_Blocking,
  Socket_NonBlocking
};

struct socket_t
{
  s32 Id;
  socket_type Type;

  socket_t(socket_type Type_in)
  {
    Clear(this);
    this->Type = Type_in;
  }

  socket_t() {
    Clear(this);
  }
};

enum connection_state
{
  ConnectionState_Disconnected,
  ConnectionState_AwaitingHandshake,
  ConnectionState_Connected
};

struct network_connection
{
  client_state *Client;

  sockaddr_in Address;
  socket_t Socket;
  connection_state State;

  network_connection(socket_type Type, const char* IP)
  {
    Clear(this);
    this->Socket = CreateSocket(Type);
    this->Address = CreateAddress(IP);
  }

};

enum socket_op_result
{
  SocketOpResult_Noop,
  SocketOpResult_CompletedRW,
  SocketOpResult_Fail
};

enum socket_op
{
  SocketOp_Null,
  SocketOp_Read,
  SocketOp_Write,
  SocketOp_Count
};

inline b32
IsDisconnected(network_connection *Conn)
{
  b32 Result = (Conn->State == ConnectionState_Disconnected);
  return Result;
}

inline b32
IsConnected(network_connection *Conn)
{
  b32 Result = (Conn->State == ConnectionState_Connected);
  return Result;
}

inline void
Disconnect(network_connection *Connection)
{
  Info("Disconnecting");
  Assert(Connection->Socket.Id);

  close(Connection->Socket.Id);
  Clear(&Connection->Socket);
  Connection->State = ConnectionState_Disconnected;

  // Clients aren't required to have this pointer hooked up so we've got to
  // check for it before overwriting.
  if (Connection->Client)
    Connection->Client->Id = -1;

  Assert(!IsConnected(Connection));

  return;
}

socket_op_result
NetworkOp(network_connection *Connection, void *Message, u32 MessageSize, socket_op SocketOp)
{
  Assert(Connection);
  Assert(Message);

  socket_op_result OpResult = SocketOpResult_Fail;
  s64 SocketReturnValue = 0;
  u64 BytesAvailable = 0;

  // We may have disconnected on a previous attempt to read/write this frame
  // therefore this check must be here
  if (!IsDisconnected(Connection))
  {
    switch(SocketOp)
    {
      case SocketOp_Read:
      {
        s32 Flags = 0;
        BytesAvailable = (u64)recv(Connection->Socket.Id, (void*)Message, MessageSize, MSG_PEEK);

        if (BytesAvailable >= MessageSize)
        {
          SocketReturnValue = recv(Connection->Socket.Id, (void*)Message, MessageSize, Flags);
        }

      } break;

      case SocketOp_Write:
      {
        s32 Flags = MSG_NOSIGNAL;
        // FIXME(Jesse): This can fail if the underlying TCP implementations
        // internal buffer is unable to send MessageSize bytes.  In that case
        // the Assert below will fire.  This is insufficient as it will result
        // in a corrupt message stream.
        SocketReturnValue = send(Connection->Socket.Id, (void*)Message, MessageSize , Flags);
        Assert(MessageSize == SocketReturnValue);
      } break;

      InvalidDefaultCase;
    }

    const s32 SocketError = -1;
    const s32 SocketClosed = 0;
    switch(SocketReturnValue)
    {
      case SocketError:
      {
        switch (errno)
        {
          case EAGAIN:
          {
            // Non-blocking socket would block
            Assert(Connection->Socket.Type == Socket_NonBlocking);
            OpResult = SocketOpResult_Noop;
          } break;

          default:
          {
            Error("SocketOp failed : %d", errno);
          } break;
        }
      } break;

      case SocketClosed:
      {
        Info("Network peer closed connection gracefully");
      } break;

      default:
      {
        // s64 BytesRead = SocketReturnValue;
        OpResult = SocketOpResult_CompletedRW;
        Assert(MessageSize == SocketReturnValue);
      } break;
    }

    if (OpResult == SocketOpResult_Fail) { Disconnect(Connection); }
  }

  return OpResult;
}

template <typename T> void
Send(network_connection *Connection, T *Message)
{
  NetworkOp(Connection, Message, sizeof(T), SocketOp_Write);
  return;
}

template <typename T> socket_op_result
Read(network_connection *Connection, T *Message)
{
  socket_op_result Result = NetworkOp(Connection, Message, sizeof(T), SocketOp_Read);
  return Result;
}

template <typename T> socket_op_result
FlushIncomingMessages(network_connection *Connection, T *Message)
{
  socket_op_result ReadMessage = Read(Connection, Message);
  socket_op_result AnyMessagesRead = ReadMessage;

  while (ReadMessage == SocketOpResult_CompletedRW) {
    ReadMessage = Read(Connection, Message);
  }

  return AnyMessagesRead;
}
