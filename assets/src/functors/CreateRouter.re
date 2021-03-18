/* FUNCTOR */

module type Config = {
  type route;
  let toUrl: route => string;
  let toRoute: ReasonReactRouter.url => route;
};

module Make = (Config: Config) => {
  type route = Config.route;
  type renderProps = {
    updateRoute: route => unit,
    route,
  };
  type state = {route};
  type action =
    | UpdateRoute(route);
  let initialState = {
    route: ReasonReactRouter.dangerouslyGetInitialUrl() |> Config.toRoute,
  };
  [@react.component]
  let make = (~render, ()) => {
    let (state, dispatch) =
      React.useReducer(
        (_state, action) =>
          switch (action) {
          | UpdateRoute(route) => {route: route}
          },
        initialState,
      );

    React.useEffect0(() => {
      let watcherID =
        ReasonReactRouter.watchUrl(url =>
          dispatch(UpdateRoute(Config.toRoute(url)))
        );
      Some(() => ReasonReactRouter.unwatchUrl(watcherID));
    });

    render({
      updateRoute: route => dispatch(UpdateRoute(route)),
      route: state.route,
    });
  };
};
