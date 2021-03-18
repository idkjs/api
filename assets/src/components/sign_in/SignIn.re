type formTypes =
  | Name
  | Password;

type formState = {
  name: string,
  password: string,
};

module Configuration = {
  type state = formState;
  type field = formTypes;
  let update = (field, value, state) =>
    switch (field) {
    | Name => {...state, name: value}
    | Password => {...state, password: value}
    };
  let get = (field, state) =>
    switch (field) {
    | Name => state.name
    | Password => state.password
    };
};

let minLengthPassord = (value, _values) => String.length(value) >= 6;

let emptyMsg = "Field is required";

let rules = [
  (Name, [(CreateForm.NotEmpty, emptyMsg)]),
  (
    Password,
    [
      (CreateForm.NotEmpty, emptyMsg),
      (
        CreateForm.Custom(minLengthPassord),
        "Password length must be equal or larger than 6",
      ),
    ],
  ),
];

module SpecialForm = CreateForm.Make(Configuration);



let first = list => List.length(list) > 0 ? Some(List.hd(list)) : None;

let getError = (field, errors) =>
  List.filter(((name, _)) => name === field, errors)
  |> first
  |> (
    errors =>
      switch (errors) {
      | Some((_, msgs)) => React.string(List.hd(msgs))
      | None => React.null
      }
  );

let getInputClass = (field, errors): string =>
  List.filter(((name, _)) => name === field, errors)
  |> first
  |> (
    errors =>
      switch (errors) {
      | Some((_, _msgs)) => "form-control is-invalid"
      | None => "form-control"
      }
  );

let getValue = event => ReactEvent.Form.target(event)##value;

let preventDefault = event => ReactEvent.Synthetic.preventDefault(event);

type user = Abstract.User.t;
type sessionData = Abstract.SessionData.t;

type state =
  | Display
  | DisplayWithErrors(string)
  | Loading;

type action =
  | Load
  | Loaded(sessionData)
  | Failed(string);

[@react.component]
let make = (~handleSubmit, ()) => {
  let (state, dispatch) =
    React.useReducer(
      (_state, action) =>
        switch (action) {
        | Load => Loading

        | Loaded(sessionData) =>
          handleSubmit(sessionData);
          Display;
        | Failed(error) => DisplayWithErrors(error)
        },
      Display,
    );

  let load = (data: SpecialForm.state) => {
    let {name, password} = data.values;
    dispatch(Load);
    Js.Promise.(
      Api.signIn({
        "session": {
          "name": name,
          "password": password,
        },
      })
      |> then_(result =>
           switch (result) {
           | Belt.Result.Ok(sessionData) =>
             resolve(dispatch(Loaded(sessionData)))
           | Belt.Result.Error(errorMsg) =>
             resolve(dispatch(Failed(errorMsg)))
           }
         )
      |> ignore
    );
  };
  <div>
    {switch (state) {
     | Display => React.null
     | DisplayWithErrors(error) => <p> {React.string(error)} </p>
     | Loading => <p> {React.string("Loading...")} </p>
     }}
    <SpecialForm
      initialState={name: "", password: ""}
      rules
      render={(formState: SpecialForm.form) => {
        let {form, handleChange} = formState;
        <form
          onSubmit={e => {
            preventDefault(e);
            load(form);
          }}>
          <div className="form-group">
            <label htmlFor="userName"> {React.string("Name: ")} </label>
            <input
              id="userName"
              placeholder="Name"
              value={form.values.name}
              onChange={e => getValue(e) |> handleChange(Name)}
              className={getInputClass(Name, form.errors)}
              required=true
            />
            <div className="invalid-feedback">
              {getError(Name, form.errors)}
            </div>
          </div>
          <div className="form-group">
            <label htmlFor="userPassword"> {React.string("Password: ")} </label>
            <input
              id="userPassword"
              placeholder="Password"
              type_="password"
              value={form.values.password}
              onChange={e => getValue(e) |> handleChange(Password)}
              className={getInputClass(Password, form.errors)}
              required=true
            />
            <div className="invalid-feedback">
              {getError(Password, form.errors)}
            </div>
          </div>
          <button className="btn btn-primary"> {React.string("Sign In")} </button>
        </form>;
      }}
    />
  </div>;
};
