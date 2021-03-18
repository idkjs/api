[@react.component]
let make = (~href, ~label, ()) =>
  ReactCompat.useRecordApi({
    ...ReactCompat.component,

    render: _self => {
      let onClick = e => {
        ReactEvent.Mouse.preventDefault(e);
        ReasonReact.Router.push(href);
      };
      <a href onClick className="nav-link"> {label()} </a>;
    },
  });
