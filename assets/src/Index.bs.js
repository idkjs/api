'use strict';

var React = require("react");
var ReactDOMRe = require("reason-react/src/legacy/ReactDOMRe.bs.js");
var App$ReactTemplate = require("./App.bs.js");

((require("../css/app.scss")));

ReactDOMRe.renderToElementWithId(React.createElement(App$ReactTemplate.make, {}), "app");

/*  Not a pure module */
