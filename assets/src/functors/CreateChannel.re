type socket = Phoenix.Socket.t;
type channel = Phoenix.Channel.t;



/* Helpers */

let putOn = (event, f: Js_json.t => unit, channel) => {
  let _ = channel##on(event, f);
  channel;
};

let putReceive = (event, handleReceive, push) => {
  let _ = push##receive(event, handleReceive);
  push;
};

let putOnError = (f: Js_json.t => unit, channel) => {
  let _ = channel##onError(f);
  channel;
};

let putOnClose = (f: Js_json.t => unit, channel) => {
  let _ = channel##onClose(f);
  channel;
};

let joinChannel = Phoenix.Channel.join;

/* let listBy = [%bs.raw {|
     (id, { metas: [first, ...rest] }) =>
       Object.assign({}, first, { id, count: rest.length + 1 })
   |}];  */

let defaultSync = (topic, list) => Js.log2(topic, list);

/* FUNCTOR */

module Make = (()) => {
  type state =
    | Initialized
    | Joined(channel)
    | Left;

  type action =
    | LeaveChannel(channel)
    | ChannelJoined(channel)
    | ChannelFailed
    | ChannelClosed;

  [@react.component]
  let make =
      (
        ~socket,
        ~topic,
        ~render,
        ~withPresence=false,
        ~handleReceive,
        ~handleEvent,
        ~handleSync=defaultSync,
        (),
      ) => {
    let (state, dispatch) =
      React.useReducer(
        (_state, action) =>
          switch (action) {
          | LeaveChannel(channel) =>
            let _ = Phoenix.Channel.leave(channel);
            ();

            Left;
          | ChannelJoined(channel) => Joined(channel)
          | ChannelFailed => Left
          | ChannelClosed => Left
          },
        Initialized,
      );
    // this is unused
    // let leaveChannel = channel => {
    //   dispatch(LeaveChannel(channel));
    //   let _ = Phoenix.Channel.leave(channel);
    //   ();
    // };

    React.useEffect0(() => {
      Js.log("Mounted");
      let myChannel = Phoenix.Socket.channel(topic, socket);

      if (withPresence) {
        let myPresence = Phoenix.Presence.init(myChannel);

        let _ = myPresence##onJoin(() => Js.log("Presence joined!"));

        let _ =
          myPresence##onSync(() => {
            Js.log(myPresence##list);
            Js.log("------------");

            /* let loggedInUsers = myPresence##list(listBy);
               handleSync(topic, loggedInUsers); */

            handleSync(topic, myPresence##list);
          });
        ();
      };

      let _ =
        myChannel
        |> putOn("ping", handleEvent("ping"))
        |> joinChannel(_)
        |> putReceive("ok", handleReceive("ok"))
        |> putReceive("error", handleReceive("error"))
        |> putReceive("timeout", handleReceive("timeout"));

      /* let _ = myChannel
         |> putOnError(handleEvent("error"))
         |> putOnClose();  */

      dispatch(ChannelJoined(myChannel));

      Some(
        () => {
          Js.log("Disconnect channel!");
          let _ = Phoenix.Channel.leave(myChannel);
          ();
        },
      );
    });

    switch (state) {
    | Initialized => <div> {React.string("Initialized...")} </div>
    | Left => <div> {React.string("Left...")} </div>
    | Joined(channel) => render(channel)
    };
  };
};
