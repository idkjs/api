//

// [@react.component]
// let make = () => <div> <h2> {React.string("Home")} </h2> </div>;
type sessionData = Abstract.SessionData.t;

type signMode =
  | SignInMode
  | SignUpMode;

let toggleLabelOfSignMode = signMode =>
  switch (signMode) {
  | SignInMode => "or Sign Up"
  | SignUpMode => "or Sign In"
  };

type state = {
  bootupTime: option(float),
  isAuthenticated: bool,
  sessionData: option(sessionData),
  loading: bool,
  signMode,
};

type action =
  | AppBootup
  | Login(sessionData)
  | Logout
  | RefreshToken(string)
  | RefreshSucceed(sessionData)
  | RefreshFailed
  | ToggleSignMode;

let initialState = {
  bootupTime: None,
  isAuthenticated: false,
  sessionData: None,
  loading: false,
  signMode: SignInMode,
};



let reducer = (state, action) =>
  switch (action) {
  | AppBootup => {...state, bootupTime: Some(Js.Date.now())}
  | Login(sessionData) =>
    AuthService.saveToken(sessionData.token);

    {...state, isAuthenticated: true, sessionData: Some(sessionData)};

  | Logout =>
    AuthService.removeToken();
    {...state, isAuthenticated: false, sessionData: None};
  | RefreshToken(token) =>
    Js.log("Token to reload is: " ++ token);
    {...state, loading: true};

  | RefreshSucceed(sessionData) =>
    AuthService.saveToken(sessionData.token);
    {
      ...state,
      loading: false,
      isAuthenticated: true,
      sessionData: Some(sessionData),
    };
  | RefreshFailed =>
    AuthService.removeToken();
    {...state, loading: false, isAuthenticated: false, sessionData: None};

  | ToggleSignMode =>
    let newSignMode =
      switch (state.signMode) {
      | SignInMode => SignUpMode
      | SignUpMode => SignInMode
      };
    {...state, signMode: newSignMode};
  };

[@react.component]
let make = () => {
  let (state, dispatch) = React.useReducer(reducer, initialState);
  let refreshToken = token => {
    dispatch(RefreshToken(token));

    Js.Promise.(
      Api.refreshToken(token)
      |> then_(result =>
           switch (result) {
           | Belt.Result.Ok(sessionData) =>
             resolve(dispatch(RefreshSucceed(sessionData)))
           | Belt.Result.Error(error) =>
             let _ = Belt.Result.Error(["error'", ...error]);
             let _ = Belt.Result.Error(Js.log2(["error'", ...error]));
             resolve(dispatch(RefreshFailed));
           }
         )
      |> ignore
    );
  };
  React.useEffect0(() => {
    dispatch(AppBootup);
    switch (AuthService.loadToken()) {
    | Some(token) => refreshToken(token)
    | None => Js.log("No token to reload from...")
    };
    None;
  });

  if (state.loading) {
    <p> {React.string("Loading...")} </p>;
  } else {
    <div>
      <header>
        {switch (state.bootupTime) {
         | Some(bootupTime) =>
           <p> {React.string(Tools.formatTimestamp(bootupTime))} </p>
         | None => <p> {React.string("Bootup time is not set!")} </p>
         }}
      </header>
      <main role="main" className="container">
        {state.isAuthenticated
           ? <div>
               <button
                 onClick={_event => dispatch(Logout)}
                 className="btn btn-link">
                 {React.string("Log Out")}
               </button>
               {switch (state.sessionData) {
                | Some(sessionData) =>
                  <Member
                    token={sessionData.token}
                    userName={sessionData.currentUser.name}
                    userId={sessionData.currentUser.id}
                  />
                | None => React.null
                }}
             </div>
           : <div>
               {switch (state.signMode) {
                | SignInMode =>
                  <SignIn handleSubmit={values => dispatch(Login(values))} />
                | SignUpMode =>
                  <SignUp handleSubmit={values => dispatch(Login(values))} />
                }}
               <button
                 onClick={_event => dispatch(ToggleSignMode)}
                 className="btn btn-link">
                 {React.string(toggleLabelOfSignMode(state.signMode))}
               </button>
             </div>}
      </main>
    </div>;
  };
};
