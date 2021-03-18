module Push = {
  type t = {
    .
    [@meth] "resend": float => unit,
    [@meth] "send": unit => unit,
    [@meth] "receive": (string, Js.Json.t => unit) => t,
  };
};

module Channel = {
  type nonrec ref = int;
  type nonrec t = {
    .
    [@get] "topic": string,
    [@get] "state": string,
    [@meth] "rejoinUntilConnected": unit => unit,
    [@meth] "onClose": unit => unit,
    [@meth] "onError": unit => unit,
    [@meth] "on": (string, Js.Json.t => unit) => ref,
    [@meth] "off": (string, ref) => unit,
    [@meth] "onMessage": (string, Js.Json.t, Js.Json.t) => Js.Json.t,
  };

  [@bs.send] external join: (t, ~timeout: float=?, unit) => Push.t = "join";
  let join = (~timeout: option(float)=?, channel) =>
    join(channel, ~timeout?, ());

  [@bs.send] external leave: (t, ~timeout: float=?, unit) => Push.t = "leave";
  let leave = (~timeout: option(float)=?, channel) =>
    leave(channel, ~timeout?, ());

  [@bs.send]
  external push:
    (t, string, Js.t('payload), ~timeout: float=?, unit) => Push.t =
    "push";
  let push = (event, payload, ~timeout=?, channel) =>
    push(channel, event, payload, ~timeout?, ());
};

module Socket = {
  type nonrec t = {
    .
    [@meth] "protocol": unit => string,
    [@meth] "endPointURL": unit => string,
    [@meth] "log": (string, string, Js.Json.t) => unit,
    [@meth] "hasLogger": unit => bool,
    [@meth] "onOpen": (unit => unit) => unit,
    [@meth] "onClose": (unit => unit) => unit,
    [@meth] "onError": (unit => unit) => unit,
    [@meth] "onMessage": (unit => unit) => unit,
    [@meth] "connectionState": unit => unit,
    [@meth] "isConnected": unit => bool,
  };

  [@module "phoenix"] [@new]
  external init: (string, ~opts: Js.t('opts)=?, unit) => t = "Socket";
  let init = (~opts: option(Js.t('opts))=?, endPoint) =>
    init(endPoint, ~opts?, ());

  [@bs.send]
  external connect: (t, ~params: Js.t('params)=?, unit) => unit = "connect";
  let connect = (~params: option(Js.t('params))=?, socket) =>
    [@ns.braces]
    {
      let _ = connect(~params?, socket, ());
      socket;
    };

  [@bs.send]
  external disconnect:
    (
      t,
      ~callback: unit => unit=?,
      ~code: string=?,
      ~reason: Js.t('reason)=?,
      unit
    ) =>
    unit =
    "disconnect";
  let disconnect =
      (
        ~callback: option(unit => unit)=?,
        ~code: option(string)=?,
        ~reason: option(Js.t('reason))=?,
        socket,
      ) =>
    [@ns.braces]
    {
      let _ = disconnect(~callback?, ~code?, ~reason?, socket, ());
      socket;
    };

  [@bs.send]
  external channel: (t, string, ~params: Js.t('params)=?, unit) => Channel.t =
    "channel";
  let channel = (~params: option(Js.t('params))=?, topic, socket) =>
    channel(socket, topic, ~params?, ());
};

module Presence = {
  // open Abstract;
  type nonrec t = {
    .
    [@meth] "onJoin": (unit => unit) => unit,
    [@meth] "onLeave": (unit => unit) => unit,
    [@meth] "onSync": (unit => unit) => unit,
    [@meth] "list": (unit => unit) => Js.Json.t,
    [@meth] "inPendingSyncState": unit => bool,
  };

  [@module "phoenix"] [@new]
  external init: (Channel.t, ~opts: Js.t('opts)=?, unit) => t = "Presence";
  let init = (~opts: option(Js.t('opts))=?, channel) =>
    init(channel, ~opts?, ());
};
