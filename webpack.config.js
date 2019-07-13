const path = require("path")

module.exports = {
  mode: "development",
  entry: {
    basic: "./lib/js/examples/basic/basicEntry.js",
    todomvc: "./lib/js/examples/todomvc/todomvcEntry.js",
    react: "./lib/js/examples/react/reactEntry.js",
    immutable: "./lib/js/examples/immutable/immutableEntry.js",
    render: "./lib/js/examples/render/renderEntry.js",
    renderv2: "./lib/js/examples/render-v2/renderEntryV2.js"
  },
  output: {
    path: path.join(__dirname, "bundledOutputs"),
    filename: "[name].js"
  }
}
