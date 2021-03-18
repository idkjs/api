type user = {
  id: int,
  name: string,
  email: string,
};

type state =
  | NotAsked
  | Loading
  | Failure
  | Success(list(user));

type action =
  | LoadUsers
  | LoadedUsers(list(user))
  | LoadUsersFailed;

module Decode = {
  let user = json =>
    Json.Decode.{
      id: field("id", int, json),
      name: field("name", string, json),
      email: field("email", string, json),
    };
  let users = json => Json.Decode.(field("data", list(user), json));
};

let url = "http://localhost:4000/api/v1/users";

let fetchUsers = () =>
  Js.Promise.(
    Axios.get(url)
    |> then_(response =>
         response##data
         |> Decode.users
         |> (users => Some(users) |> Js.Promise.resolve)
       )
    |> catch(_err => resolve(None))
  );



[@react.component]
let make = () => {
  let (state, dispatch) =
    React.useReducer(
      (_state, action) =>
        switch (action) {
        | LoadUsers => Loading
        | LoadedUsers(users) => Success(users)
        | LoadUsersFailed => Failure
        },
      NotAsked,
    );

  let loadUsers = () => {
    dispatch(LoadUsers);
    Js.Promise.(
      fetchUsers()
      |> then_(result =>
           switch (result) {
           | Some(users) => resolve(dispatch(LoadedUsers(users)))
           | None => resolve(dispatch(LoadUsersFailed))
           }
         )
      |> ignore
    );
  };

  switch (state) {
  | NotAsked =>
    <div>
      {React.string("Click to start load Users")}
      <button onClick={_event => loadUsers()}> {React.string("Load Users")} </button>
    </div>
  | Loading => <div> {React.string("Loading...")} </div>
  | Failure => <div> {React.string("Something went wrong!")} </div>
  | Success(users) =>
    <div>
      <h2> {React.string("Users")} </h2>
      <ul>
        {users
         |> List.map(user =>
              <li key={string_of_int(user.id)}> {React.string(user.name)} </li>
            )
         |> Array.of_list
         |> React.array}
      </ul>
    </div>
  };
};
