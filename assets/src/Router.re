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
  let toRoute = (url: ReasonReactRouter.url) =>
    switch (url.path) {
    | [] => Home
    | ["users"] => Users
    | ["channels"] => Channels
    | ["sign_in"] => SignIn
    | ["forms"] => Forms
    | ["about"] => About
    | _ => NotFound
    };

  let toUrl = route =>
    switch (route) {
    | Home => "/"
    | Users => "/users"
    | Channels => "channels"
    | SignIn => "/sign_in"
    | Forms => "/forms"
    | About => "/about"
    | _ => "/not_found"
    };
};

module Router = CreateRouter.Make(Config);



[@react.component]
let make = () => {
  <div>
    <header>
      <nav
        role="navigation" className="navbar navbar-expand navbar-dark bg-dark">
        <a className="navbar-brand" href="#">
          {React.string("Phoenix ReasonML client")}
        </a>
        <div>
          <ul className="navbar-nav">
            <li className="nav-item">
              <Link href={Config.toUrl(Home)} label={() => React.string("Home")} />
            </li>
            <li className="nav-item">
              <Link href={Config.toUrl(Users)} label={() => React.string("Users")} />
            </li>
            <li className="nav-item">
              <Link
                href={Config.toUrl(Channels)}
                label={() => React.string("Channels")}
              />
            </li>
            <li className="nav-item">
              <Link
                href={Config.toUrl(SignIn)}
                label={() => React.string("SignIn")}
              />
            </li>
            <li className="nav-item">
              <Link href={Config.toUrl(Forms)} label={() => React.string("Forms")} />
            </li>
            <li className="nav-item">
              <Link href={Config.toUrl(About)} label={() => React.string("About")} />
            </li>
          </ul>
        </div>
      </nav>
    </header>
    <main role="main" className="container">
      <Router
        render={(router: Router.renderProps) => {
     Js.log2("Router.renderProps: ",router);
          let {route} = router;
          switch (route) {
          | Home => <Home />
          | Users => <Users />
          | Channels => <Channels />
          | SignIn => <SignIn handleSubmit={values => Js.log(values)} />
          | Forms => <Forms handleSubmit={values => Js.log(values)} />
          | About => <About />
          | NotFound => <NotFound />
          };
        }}
      />
    </main>
  </div>;
};
