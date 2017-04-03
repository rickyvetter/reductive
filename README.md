# reductive

## What is this?
Reductive is a minimal reimplementation of [redux](https://github.com/reactjs/redux) in [Reason](https://github.com/facebook/reason). It is intended to be used as a learning tool for Reason, and to show how the different semantics and language features of Reason make much of the library redundant.

## What is this not?
This is not internded to be used as a production state-management solution. This is not intended to be used as a drop-in replacement for redux, or for your JS state management library of choice.

## What are some interesting things to look at?
### Actions
Redux [actions](http://redux.js.org/docs/basics/Actions.html) are implemented as plain JS objects. JS objects can be a little too flexible, and many users of Redux rely on standardized shapes for their actions to make sure that middleware and third party libraries can work with them. Reason has first-class language-level support for composing a set of data types which many functions can operate over. They are called [variants](http://facebook.github.io/reason/#syntax-basics-variants) and you can see how they are used [here](https://github.com/reasonml/reductive/blob/master/examples/basic/simpleStore.re).

### Action Creators
Redux [action creators](http://redux.js.org/docs/basics/Actions.html#action-creators) are used as composable ways of reliably generating plain JS objects which match the schema for a specific action. In Reason, the type system does a lot of this work for you, so you can reliably put action types directly in your code and know that things will Just Work™. This has an added performance advantage of shifting that function call to compile time.

### Enhancer composition
Much of the power of redux comes from having a unified api for writing and using middleware, or enhancers. These middlewares typically get wired together behind the scenes in the [applyMiddlewares function](http://redux.js.org/docs/api/applyMiddleware.html). This function doesn't exist in reductive, because Reason has language-level support for function composition with operators like [`|>` and `@@`](https://caml.inria.fr/pub/docs/manual-ocaml/libref/Pervasives.html#6_Compositionoperators). You can see some [example usage here](https://github.com/reasonml/reductive/blob/master/examples/immutable/timeTravelStore.re#L86-L87) and reason [some of the deprecation messages](https://github.com/reasonml/reductive/blob/master/src/reductive.rei#L39-L112) to get a better understanding of typical usage.

### The Source
The code behind reductive is incredibly simple. The [first 40 lines](https://github.com/reasonml/reductive/blob/master/src/reductive.re#L1-L40) Include the entire reimplementation of [redux](https://github.com/reactjs/redux). The [next ~40 lines](https://github.com/reasonml/reductive/blob/master/src/reductive.re#L42-L83) are a reimplementation of the [react-redux](https://github.com/reactjs/react-redux) library (sans the HOC connect style implementation). The code is very short, and relatively simple to follow. Hopefully it will inform your exploration of Reason and what possiblities there are when it comes to state management using some new language features.

## Contributing
This is an imperfect project with the intention to help learning Reason syntax and to spark imagination for Reason state management. There are a number of things that can still be added to help this process.

* Bug fixes
* Better example usage
* More documentation of differences and similarities from redux
* Different examples of (or links to) state management in Reason
