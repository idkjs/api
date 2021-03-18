[@react.component]
let make = (~href, ~label, ()) => {
  let onClick = e => {
    ReactEvent.Mouse.preventDefault(e);
    ReasonReactRouter.push(href);
  };
  <a href onClick className="nav-link"> {label()} </a>;
};
