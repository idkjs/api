/* TESTING SOCKET */

let endPoint = "ws://localhost:4000/socket";

let socketOpts = {
  "timeout": 10000,
  "logger": (kind, msg, data) => Js.log(kind ++ ": " ++ msg ++ ", " ++ data),
};

let mySocket = Phoenix.Socket.init(~opts=socketOpts, endPoint);

Phoenix.Socket.connect(mySocket) |> ignore;

mySocket##onOpen(() => Js.log(mySocket##isConnected()));

mySocket##onError(() => Js.log("Socket in error!"));
mySocket##onClose(() => Js.log("Socket closed!"));

Js.log2("SOCKET : ", mySocket);

let putOn = (event, f: Js_json.t => unit, channel) => {
  let _ = channel##on(event, f);
  channel;
};

let putReceive = (event, handleReceive, push) => {
  let _ = push##receive(event, handleReceive);
  push;
};

let joinChannel = Phoenix.Channel.join;

let handleReceive = (event, any) =>
  switch (event) {
  | "ok" => Js.log(("handleReiceive:" ++ event, "Joined"))
  | "error" => Js.log(("handleReiceive:" ++ event, "Failed to join channel"))
  | _ => Js.log(("handleReiceive:" ++ event, any))
  };

let handleEvent = (event, response) => {
  let _ = Js.log(("handleEvent:" ++ event, response));
  ();
};

let myChannel = Phoenix.Socket.channel("system", mySocket);

let myPresence = Phoenix.Presence.init(myChannel);

let listBy = [%bs.raw
  {|
  (id, { metas: [first, ...rest] }) =>
    Object.assign({}, first, { id, count: rest.length + 1 })
|}
];

let _ =
  myPresence##onSync(() => {
    let loggedInUsers = myPresence##list(listBy);
    Js.log(loggedInUsers);
  });

Js.log2("PRESENCE : ", myPresence);

let _ =
  myChannel
  |> putOn("ping", handleEvent("ping"))
  |> joinChannel(_)
  |> putReceive("ok", handleReceive("ok"))
  |> putReceive("error", handleReceive("error"));

Js.log(myChannel##state);

Js.log2("CHANNEL : ", myChannel);

[@bs.val] external setTimeout: (unit => unit, int) => float = "setTimeout";

let _ =
  setTimeout(
    () => {
      Js.log(mySocket##isConnected());
      let _ = Phoenix.Socket.disconnect(mySocket);
      Js.log("Disconnected!");
      Js.log(mySocket##isConnected());
    },
    10000,
  );

type user = {
  name: string,
  role: string,
};

type member = {
  name: string,
  role: string,
};

let koko: user = {name: "koko", role: "user"};

let member: member = {name: "yo!", role: "member"};

Js.log(koko.name);
Js.log(member.name);

let str = React.string;

[@react.component]
let make = () => {
  React.useEffect0(() => {
    Js.log("Mounted!");
    None;
  });
  <div> <h2> {str("Channels")} </h2> </div>;
};
