'use strict';

var React = require("react");
var ReactDOMRe = require("reason-react/src/legacy/ReactDOMRe.bs.js");
var Router$ReactTemplate = require("./Router.bs.js");

((require("../css/app.scss")));

ReactDOMRe.renderToElementWithId(React.createElement(Router$ReactTemplate.make, {}), "app");

/*  Not a pure module */
