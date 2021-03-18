let str = React.string;

[@react.component]
let make = () =>
  ReactCompat.useRecordApi({
    ...ReactCompat.component,

    render: _self => <div> <h2> {str("About")} </h2> </div>,
  });
