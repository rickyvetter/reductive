const path = require('path');

module.exports = {
  entry: {
    basic: './lib/js/examples/basic/basicSubscription.js',
    react: './lib/js/examples/react/component.js',
  },
  output: {
    path: path.join(__dirname, "bundledOutputs"),
    filename: '[name].js',
  },
};
