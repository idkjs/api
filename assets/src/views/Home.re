let str = React.string;

[@react.component]
let make = () =>
  ReactCompat.useRecordApi({
    ...ReactCompat.component,

    render: _self => <div> <h2> {str("Home")} </h2> </div>,
  });
