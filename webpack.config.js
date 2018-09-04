const path = require('path');

module.exports = {
  entry: {
    basic: './lib/js/examples/basic/basicEntry.js',
    react: './lib/js/examples/react/reactEntry.js',
    immutable: './lib/js/examples/immutable/immutableEntry.js',
    render: './lib/js/examples/render/renderEntry.js',
  },
  output: {
    path: path.join(__dirname, "bundledOutputs"),
    filename: '[name].js',
  },
};
