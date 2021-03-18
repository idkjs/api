/* FUNCTOR */



module Make = (Config: {type data;}) => {
  type state =
    | Loading
    | Failure
    | Success(Config.data);
  type action =
    | Load
    | Loaded(Config.data)
    | Failed;

  [@react.component]
  let make = (~fetch, ~failedMessage, ~render, ()) => {
    let (state, dispatch) =
      React.useReducer(
        (_state, action) =>
          switch (action) {
          | Load => Loading
          | Loaded(data) => Success(data)
          | Failed => Failure
          },
        Loading,
      );
    let load = () => {
      dispatch(Load);
      Js.Promise.(
        fetch()
        |> then_(result =>
             switch (result) {
             | Some(data) => resolve(dispatch(Loaded(data)))
             | None => resolve(dispatch(Failed))
             }
           )
        |> ignore
      );
    };

    React.useEffect0(() => {
      load();
      Some(() => ());
    });

    switch (state) {
    | Loading => <div> {React.string("Loading...")} </div>
    | Failure => <div> {React.string(failedMessage)} </div>
    | Success(data) => render(data)
    };
  };
};
