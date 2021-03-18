module SpecialSocket =
  CreateSocket.Make({});

module SpecialChannel =
  CreateChannel.Make({});

let endPoint = "ws://localhost:4000/socket";

let socketOpts = token => {
  let token = {"token": token};
  {
    "params": {
      token;
    },
    "timeout": 10000,
    "logger": (kind, msg, data) =>
      Js.log(kind ++ ": " ++ msg ++ ", " ++ data),
  };
};

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

let str = React.string;

type presence = Abstract.PresenceData.t;

type state = {systemPresences: list(presence)};

let initialState = {systemPresences: []};

type action =
  | UpdatePresence(string, list(presence));

let reducer = (state, action) =>
  switch (action) {
  | UpdatePresence(topic, loggedInUsers) =>
    switch (topic) {
    | "system" => {systemPresences: loggedInUsers}
    | _ =>
      Js.log("topic not matched : " ++ topic);
      state;
    }
  };

[@react.component]
let make = (~token, ~userName, ~userId, ()) => {
  let (_state, dispatch) = React.useReducer(reducer, initialState);
  let handleSync = (topic, loggedInUsers) => {
    let _ = Js.log(("handleSync:" ++ topic, loggedInUsers));

    dispatch(UpdatePresence(topic, []));
  };

  <div>
    <h2> {str(userName)} </h2>
    <p> {str("Id: " ++ string_of_int(userId))} </p>
    <p> {str("Token: " ++ token)} </p>
    <SpecialSocket
      endPoint
      socketOpts={socketOpts(token)}
      render={socket => {
        Js.log("Socket joined");

        let systemTopic = "system";
        let privateTopic = "user:" ++ string_of_int(userId);
        <div>
          <p> {str("Socket connected!")} </p>
          <SpecialChannel
            topic=systemTopic
            socket
            handleEvent
            handleReceive
            withPresence=true
            handleSync
            render={channel => {
              Js.log("Channel joined with topic : " ++ channel##topic);
              <p>
                {str("Channel connected with topic : " ++ systemTopic)}
              </p>;
            }}
          />
          <SpecialChannel
            topic=privateTopic
            socket
            handleEvent
            handleReceive
            render={channel => {
              Js.log("Channel joined with topic : " ++ channel##topic);
              <p>
                {str("Channel connected with topic : " ++ privateTopic)}
              </p>;
            }}
          />
        </div>;
      }}
    />
  </div>;
};
