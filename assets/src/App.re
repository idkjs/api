
type sessionData = Abstract.SessionData.t;

type state = {
  bootupTime: option(float),
  isAuthenticated: bool,
  sessionData: option(sessionData),
  loading: bool,
}

type action = 
  | AppBootup
  | Login(sessionData)
  | Logout
  | RefreshToken(string)
  | RefreshSucceed(sessionData)
  | RefreshFailed;

let initialState = {
  bootupTime: None,
  isAuthenticated: false,
  sessionData: None,
  loading: false,
}

module RR = ReasonReact;
let str = RR.string;

let reducer = (action, state) => {
  switch (action) {
  | AppBootup => RR.Update({...state, bootupTime: Some(Js.Date.now())})
  | Login(sessionData) => RR.UpdateWithSideEffects(
    {...state, isAuthenticated: true, sessionData: Some(sessionData)},
    _self => AuthService.saveToken(sessionData.token)
  )
  | Logout => RR.UpdateWithSideEffects(
    {...state, isAuthenticated: false, sessionData: None},
    _self => AuthService.removeToken()
  )
  | RefreshToken(token) => 
    RR.UpdateWithSideEffects(
      {...state, loading: true},
      self => Js.Promise.(
        Api.refreshToken(token)
        |> then_(result => 
          switch (result) {
            | Some(sessionData) =>
              resolve(self.send(RefreshSucceed(sessionData)))
            | None => 
              resolve(self.send(RefreshFailed))
            }
        )
        |> ignore
      ) 
    )
  | RefreshSucceed(sessionData) => RR.UpdateWithSideEffects(
    {...state, loading: false, isAuthenticated: true, sessionData: Some(sessionData)},
    _self => AuthService.saveToken(sessionData.token)
  )
  | RefreshFailed => RR.UpdateWithSideEffects(
    {...state, loading: false, isAuthenticated: false, sessionData: None},
    _self => AuthService.removeToken()
  )};
};

let formatTimestamp: float => string = timestamp => 
  timestamp
  -> Js.Date.fromFloat
  -> Js.Date.toLocaleTimeString;

let component = RR.reducerComponent("App");

let make = _children => {
  ...component,
  initialState: () => initialState,
  reducer,
  didMount: ({send}) => {
    send(AppBootup);
    switch (AuthService.loadToken()) {
    | Some(token) => send(RefreshToken(token))
    | None => Js.log("No token to reload from...")
    }
  },
  render: ({send, state}) => {
    <div>
      <header>
        {
          if (state.loading) {
            <p>(str("Loading"))</p>
          } else {
            ReasonReact.null
          }
        }
        {
          switch (state.bootupTime) {
          | Some(bootupTime) => <p>(str(formatTimestamp(bootupTime)))</p>
          | None => <p>(str("Bootup time is not set!"))</p>
          };
        }
      </header>
      <main role="main" className="container">
        {
          switch (state.isAuthenticated) {
          | true => 
            <div>
              <button onClick=(_event => send(Logout)) className="btn btn-primary">
                (str("Log Out"))
              </button>
              <Member />
            </div>
          | false => 
            <SignIn handleSubmit=(values => send(Login(values))) />
          }
        }
      </main>
    </div>
  }
};

/* 
type page =
  | Home
  | Users
  | Channels
  | SignIn
  | Forms
  | About
  | NotFound;

module Config = {
  type route = page;
  let toRoute = (url: ReasonReact.Router.url) =>
    switch (url.path) {
    | []           => Home
    | ["users"]    => Users
    | ["channels"] => Channels
    | ["sign_in"]  => SignIn
    | ["forms"]    => Forms
    | ["about"]    => About
    | _            => NotFound
    };

  let toUrl = route =>
    switch (route) {
    | Home     => "/"
    | Users    => "/users"
    | Channels => "channels"
    | SignIn   => "/sign_in"
    | Forms    => "/forms"
    | About    => "/about"
    | _        => "/not_found"
    };
};

module Router = CreateRouter.Make(Config);

let str = ReasonReact.string;

let component = ReasonReact.statelessComponent("App");

let make = _children => {
  ...component,

  render: _self => {
    <div>
      <header>
        <nav role="navigation" className="navbar navbar-expand navbar-dark bg-dark">
          <a className="navbar-brand" href="#">(str("Phoenix ReasonML client"))</a>
          <div>
            <ul className="navbar-nav">
              <li className="nav-item"><Link href=(Config.toUrl(Home)) label=(() => str("Home")) /></li>
              <li className="nav-item"><Link href=(Config.toUrl(Users)) label=(() => str("Users")) /></li>
              <li className="nav-item"><Link href=(Config.toUrl(Channels)) label=(() => str("Channels")) /></li>
              <li className="nav-item"><Link href=(Config.toUrl(SignIn)) label=(() => str("SignIn")) /></li>
              <li className="nav-item"><Link href=(Config.toUrl(Forms)) label=(() => str("Forms")) /></li>
              <li className="nav-item"><Link href=(Config.toUrl(About)) label=(() => str("About")) /></li>
            </ul>
          </div>
        </nav>
      </header>
      <main role="main" className="container">
        <Router 
          render=(({route}) => 
            switch route {
            | Home => <Home />
            | Users => <Users />
            | Channels => <Channels />
            | SignIn => <SignIn handleSubmit=(values => Js.log(values)) />
            | Forms => <Forms handleSubmit=(values => Js.log(values)) />
            | About => <About />
            | NotFound => <NotFound />
            }
          )
        />
      </main>
    </div>
  },
}; */
