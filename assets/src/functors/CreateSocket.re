type socket = Phoenix.Socket.t;

module RR = ReasonReact;
let str = RR.string;

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
  let make = (~endPoint, ~socketOpts, ~render, ()) =>
    ReactCompat.useRecordApi({
      ...ReactCompat.component,

      initialState: () => Initialized,
      reducer: (action, _state) =>
        switch (action) {
        | DisconnectSocket(socket) =>
          UpdateWithSideEffects(
            Disconnected,
            _self => {
              let _ = Phoenix.Socket.disconnect(socket);
              ();
            },
          )
        | SocketConnected(socket) => Update(Connected(socket))
        | SocketFailed => Update(Disconnected)
        | SocketClosed => Update(Disconnected)
        },
      didMount: ({send, onUnmount}) => {
        let mySocket = Phoenix.Socket.init(~opts=socketOpts, endPoint);
        let _ = Phoenix.Socket.connect(mySocket);

        mySocket##onOpen(() => {
          Js.log(
            "Socket connected with status : "
            ++ string_of_bool(mySocket##isConnected()),
          );
          send(SocketConnected(mySocket));
        });
        mySocket##onError(() => {
          Js.log("Socket in error!");
          send(DisconnectSocket(mySocket));
        });
        mySocket##onClose(() => {
          Js.log("Socket closed!");
          send(DisconnectSocket(mySocket));
        });
        onUnmount(() => {
          Js.log("Disconnect socket!");
          let _ = Phoenix.Socket.disconnect(mySocket);
          ();
        });
      },
      render: ({state}) =>
        switch (state) {
        | Initialized => <div> {str("Initialized...")} </div>
        | Disconnected => <div> {str("Disconnected...")} </div>
        | Connected(socket) => render(socket)
        },
    });

};
