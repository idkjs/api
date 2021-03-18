/* https://medium.com/@sharifsbeat/reasonable-form-handling-d117d52ebc8a */

type formTypes =
  | UserName
  | RepeatUserName;

type formState = {
  userName: string,
  repeatUserName: string,
};

module Configuration = {
  type state = formState;
  type field = formTypes;
  let update = (field, value, state) =>
    switch (field) {
    | UserName => {...state, userName: value}
    | RepeatUserName => {...state, repeatUserName: value}
    };
  let get = (field, state) =>
    switch (field) {
    | UserName => state.userName
    | RepeatUserName => state.repeatUserName
    };
};

let equalUserName = (value, values) => value === values.userName;

let emptyMsg = "Field is required";

let rules = [
  (UserName, [(CreateForm.NotEmpty, emptyMsg)]),
  (
    RepeatUserName,
    [
      (CreateForm.NotEmpty, emptyMsg),
      (
        CreateForm.Custom(equalUserName),
        "UserName and RepeatUserName have to be same",
      ),
    ],
  ),
];

module SpecialForm = CreateForm.Make(Configuration);

let str = React.string;

let first = list => List.length(list) > 0 ? Some(List.hd(list)) : None;

let getError = (field, errors) =>
  List.filter(((name, _)) => name === field, errors)
  |> first
  |> (
    errors =>
      switch (errors) {
      | Some((_, msgs)) => str(List.hd(msgs))
      | None => React.null
      }
  );

let getValue = event => ReactEvent.Form.target(event)##value;

let preventDefault = event => ReactEvent.Synthetic.preventDefault(event);

[@react.component]
let make = (~handleSubmit) => {
  <SpecialForm
    initialState={userName: "", repeatUserName: ""}
    rules
    render={(formState: SpecialForm.form) => {
      let {form, handleChange} = formState;
      <form
        onSubmit={e => {
          preventDefault(e);
          handleSubmit(form.values);
        }}>
        <label>
          {str("UserName: ")}
          <br />
          <input
            value={form.values.userName}
            onChange={e => getValue(e) |> handleChange(UserName)}
          />
        </label>
        <p> {getError(UserName, form.errors)} </p>
        <label>
          {str("Repeat UserName: ")}
          <br />
          <input
            value={form.values.repeatUserName}
            onChange={e => getValue(e) |> handleChange(RepeatUserName)}
          />
        </label>
        <p> {getError(RepeatUserName, form.errors)} </p>
        <br />
        <button> {str("Submit")} </button>
      </form>;
    }}
  />;
};
