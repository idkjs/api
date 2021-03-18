let str = React.string;

[@react.component]
let make = () =>
  ReactCompat.useRecordApi({
    ...ReactCompat.component,

    render: _self => <div> <h2> {str("NotFound")} </h2> </div>,
  });
