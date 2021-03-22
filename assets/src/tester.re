[%bs.raw {|require("../css/app.scss")|}];
[%bs.raw {|require("phoenix")|}];


let () =
  {
    let socket =
      Socket.make("/api/v1", [%obj {
                                     params: {
                                       "userToken": "123",
                                     },
                                   }]);
    let channel =
      Socket.channel("room:123", ~chanParams=[%obj {token: "123"}], socket);

    Channel.on("new_msg", Js.log2("Got message"), channel);

    channel
    |> Channel.push("new_msg", [%obj {body: "a"}], ~timeoutMs=10_000.0)
    |> Push.receive("ok", Js.log2("Created message"))
    |> Push.receive("error", Js.log2("Create failed"))
    |> Push.receive("timeout", _ => Js.log("Network issue"))
    |> ignore;

    channel
    |> Channel.join(_)
    |> Push.receive("ok", Js.log2("Catching up"))
    |> Push.receive("error", payload =>
         Js.log2("Failed to join", payload##reason)
       )
    |> Push.receive("timeout", _ =>
         Js.log("Network issue. Still waiting...")
       )
    |> ignore;
  };
