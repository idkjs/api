module Abstract = {
  type nonrec void = unit;
  type nonrec function_ = unit => unit;
  type nonrec any = Js_json.t;
};

module Push = {
  open Abstract;
  type t = {
    .
    [@meth] "resend": float => void,
    [@meth] "send": unit => void,
    [@meth] "receive": (string, any => void) => t,
  };
};

module Channel = {
  open Abstract;
  type nonrec ref = int;
  type nonrec t = {
    .
    [@get] "topic": string,
    [@get] "state": string,
    [@meth] "rejoinUntilConnected": unit => unit,
    [@meth] "onClose": unit => unit,
    [@meth] "onError": unit => unit,
    [@meth] "on": (string, any => unit) => ref,
    [@meth] "off": (string, ref) => unit,
    [@meth] "onMessage": (string, any, any) => any,
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
  open Abstract;
  type nonrec t = {
    .
    [@meth] "protocol": unit => string,
    [@meth] "endPointURL": unit => string,
    [@meth] "log": (string, string, any) => void,
    [@meth] "hasLogger": unit => bool,
    [@meth] "onOpen": function_ => void,
    [@meth] "onClose": function_ => void,
    [@meth] "onError": function_ => void,
    [@meth] "onMessage": function_ => void,
    [@meth] "connectionState": unit => void,
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
      ~callback: function_=?,
      ~code: string=?,
      ~reason: Js.t('reason)=?,
      unit
    ) =>
    unit =
    "disconnect";
  let disconnect =
      (
        ~callback: option(function_)=?,
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
  open Abstract;
  type nonrec t = {
    .
    [@meth] "onJoin": function_ => void,
    [@meth] "onLeave": function_ => void,
    [@meth] "onSync": function_ => void,
    [@meth] "list": function_ => any,
    [@meth] "inPendingSyncState": unit => bool,
  };

  [@module "phoenix"] [@new]
  external init: (Channel.t, ~opts: Js.t('opts)=?, unit) => t = "Presence";
  let init = (~opts: option(Js.t('opts))=?, channel) =>
    init(channel, ~opts?, ());
};
