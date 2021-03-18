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

let str = React.string;

[@react.component]
let make = () =>
  ReactCompat.useRecordApi({
    ...ReactCompat.component,

    initialState: () => NotAsked,
    reducer: (action, _state) =>
      switch (action) {
      | LoadUsers =>
        UpdateWithSideEffects(
          Loading,
          self =>
            Js.Promise.(
              fetchUsers()
              |> then_(result =>
                   switch (result) {
                   | Some(users) => resolve(self.send(LoadedUsers(users)))
                   | None => resolve(self.send(LoadUsersFailed))
                   }
                 )
              |> ignore
            ),
        )
      | LoadedUsers(users) => Update(Success(users))
      | LoadUsersFailed => Update(Failure)
      },
    render: self =>
      switch (self.state) {
      | NotAsked =>
        <div>
          {str("Click to start load Users")}
          <button onClick={_event => self.send(LoadUsers)}>
            {str("Load Users")}
          </button>
        </div>
      | Loading => <div> {str("Loading...")} </div>
      | Failure => <div> {str("Something went wrong!")} </div>
      | Success(users) =>
        <div>
          <h2> {str("Users")} </h2>
          <ul>
            {users
             |> List.map(user =>
                  <li key={string_of_int(user.id)}> {str(user.name)} </li>
                )
             |> Array.of_list
             |> React.array}
          </ul>
        </div>
      },
  });
