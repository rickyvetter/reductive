# Reductive

Reductive, like [redux](https://github.com/reactjs/redux), is a predictable
state container for [Reason](https://github.com/facebook/reason) applications.
Its scope goes beyond that of managing state for a single component, and
concerns the application as a whole. While reducer components are fine for
managing short-lived state, the lifespan of reductive state is that of the
entire application, making it suitable for high-level business logic.

## Requirements

A recent release of [Node](https://nodejs.org/en/) LTS should be sufficient.

## Installation

Installation of this module can be achieved using the `npm` command like so:

```shell
$ npm install reasonml-community/reductive
```

## Examples

See the `examples` directory for several working examples using reductive. The
`basic` example is console logging only. The `immutable` example may be broken
due to an API incompatibility. The `render` example demonstrates the
effectiveness of the `Lens`, in that only the components whose state has changed
will be re-rendered; turn on the "highlight updates" option in React Devtools to
see the effect. The `todomvc` example shows the use of reducer components along
with reductive. While the todomvc example looks a lot like those of the [todomvc
project](http://todomvc.com/), it does not conform to the todomvc application
specification, instead focusing on demonstrating the usefulness of reductive.

### Running the Examples

Start by cloning this repository, then get everything installed and built:

```shell
$ npm install
$ npm run build
$ npx webpack
```

You can then open any of the HTML files in the `test` folder within your browser.

## Word of Caution

**[You might not need this library](https://medium.com/@dan_abramov/you-might-not-need-redux-be46360cf367)**, especially so in a language which provides good enough construction blocks out of the box. **ReasonReact [already comes with reducers](https://reasonml.github.io/reason-react/blog/2017/09/01/reducers.html)**!

However, in the spirit of unifying the Reason community around an authoritative Redux implementation and to avoid churn, we'll polish Reductive to make it production-ready for those who do wish to use it anyway.

## Comparisons with Redux

### Actions

Redux [actions](http://redux.js.org/docs/basics/Actions.html) are implemented as plain JS objects. JS objects can be a bit too flexible, and many Redux users rely on standardized shapes for their actions to make sure that middlewares and third party libraries work with them. Reason has language-level support for composing a set of data types which many functions can operate over. They are called [variants](https://reasonml.github.io/docs/en/variant.html) and you can see how they are used [here](https://github.com/reasonml-community/reductive/blob/master/examples/basic/simpleStore.re).

### Action Creators

Redux [action creators](http://redux.js.org/docs/basics/Actions.html#action-creators) are used as composable ways of generating plain JS objects that match the schema of a specific action. In Reason, the type system does a lot of this work for you, so you can reliably put action types directly in your code and know that things will Just Work™. This has an added performance advantage of shifting that function call to compile time.

### Enhancer Composition

Much of the power of redux comes from having a unified API for writing and using middleware or enhancers. They're typically wired together behind the scenes in the [applyMiddlewares function](http://redux.js.org/docs/api/applyMiddleware.html). This function doesn't exist in Reductive, because Reason has language-level support for function composition with operators like [`|>` and `@@`](https://caml.inria.fr/pub/docs/manual-ocaml/libref/Pervasives.html#6_Compositionoperators). You can see some [example usage here](https://github.com/reasonml-community/reductive/blob/master/examples/immutable/timeTravelStore.re#L86-L87) and [some of the deprecation messages](https://github.com/reasonml-community/reductive/blob/master/src/reductive.rei#L39-L112) to get a better understanding of typical usage.

### The Source

The code behind Reductive is incredibly simple. The [first 40 lines](https://github.com/reasonml-community/reductive/blob/master/src/reductive.re#L1-L40) include the entire re-implementation of [redux](https://github.com/reactjs/redux). The [next ~40 lines](https://github.com/reasonml-community/reductive/blob/master/src/reductive.re#L42-L83) are a re-implementation of the [react-redux](https://github.com/reactjs/react-redux) library (without the higher-order component connect style implementation). The code is short and relatively simple to follow.
