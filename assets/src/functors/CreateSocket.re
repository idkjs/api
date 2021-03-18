type socket = Phoenix.Socket.t;


module Make = (()) => {
  type state =
    | Initialized
    | Connected(socket)
    | Disconnected;

  type action =
    | DisconnectSocket(socket)
    | SocketConnected(socket)
    | SocketFailed
    | SocketClosed;

  [@react.component]
  let make = (~endPoint, ~socketOpts, ~render) => {
    let (state, dispatch) =
      React.useReducer(
        (_state, action) =>
          switch (action) {
          | DisconnectSocket(socket) =>
            let _ = Phoenix.Socket.disconnect(socket);
            Disconnected;

          | SocketConnected(socket) => Connected(socket)
          | SocketFailed => Disconnected
          | SocketClosed => Disconnected
          },
        Initialized,
      );
    React.useEffect0(() => {
      let mySocket = Phoenix.Socket.init(~opts=socketOpts, endPoint);
      let _ = Phoenix.Socket.connect(mySocket);
      mySocket##onOpen(() => {
        Js.log(
          "Socket connected with status : "
          ++ string_of_bool(mySocket##isConnected()),
        );
        dispatch(SocketConnected(mySocket));
      });
      mySocket##onError(() => {
        Js.log("Socket in error!");
        dispatch(DisconnectSocket(mySocket));
      });
      mySocket##onClose(() => {
        Js.log("Socket closed!");
        dispatch(DisconnectSocket(mySocket));
      });
      Some(
        () => {
          Js.log("Disconnect socket!");
          let _ = Phoenix.Socket.disconnect(mySocket);
          ();
        },
      );
    });

    switch (state) {
    | Initialized => <div> {React.string("Initialized...")} </div>
    | Disconnected => <div> {React.string("Disconnected...")} </div>
    | Connected(socket) => render(socket)
    };
  };
};
