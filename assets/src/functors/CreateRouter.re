/* FUNCTOR */

module type Config = {
  type route;
  let toUrl: route => string;
  let toRoute: ReasonReact.Router.url => route;
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

  [@react.component]
  let make = (~render, ()) =>
    ReactCompat.useRecordApi({
      ...ReactCompat.component,

      initialState: () => {
        route: ReasonReact.Router.dangerouslyGetInitialUrl() |> Config.toRoute,
      },
      reducer: (action, _state) =>
        switch (action) {
        | UpdateRoute(route) => Update({route: route})
        },
      didMount: self => {
        let watcherID =
          ReasonReact.Router.watchUrl(url =>
            self.send(UpdateRoute(Config.toRoute(url)))
          );
        self.onUnmount(() => ReasonReact.Router.unwatchUrl(watcherID));
      },
      render: self =>
        render({
          updateRoute: route => self.send(UpdateRoute(route)),
          route: self.state.route,
        }),
    });
};
