# Reductive

A reimplementation of [redux](https://github.com/reactjs/redux) in [Reason](https://github.com/facebook/reason).

## Word of Caution

**[You might not need this library](https://medium.com/@dan_abramov/you-might-not-need-redux-be46360cf367)**, especially so in a language which provides good enough construction blocks out of the box.

However, in the spirit of unifying the Reason community around an authoritative Redux implementation and to avoid churn, we'll polish Reductive to make it production-ready for those who do wish to use it anyway.

## Comparisons with Redux

### Actions
Redux [actions](http://redux.js.org/docs/basics/Actions.html) are implemented as plain JS objects. JS objects can be a bit too flexible, and many Redux users rely on standardized shapes for their actions to make sure that middlewares and third party libraries work with them. Reason has language-level support for composing a set of data types which many functions can operate over. They are called [variants](https://reasonml.github.io/guide/language/data-types#variants) and you can see how they are used [here](https://github.com/reasonml/reductive/blob/master/examples/basic/simpleStore.re).

### Action Creators
Redux [action creators](http://redux.js.org/docs/basics/Actions.html#action-creators) are used as composable ways of generating plain JS objects that match the schema of a specific action. In Reason, the type system does a lot of this work for you, so you can reliably put action types directly in your code and know that things will Just Work™. This has an added performance advantage of shifting that function call to compile time.

### Enhancer Composition
Much of the power of redux comes from having a unified API for writing and using middleware or enhancers. They're typically wired together behind the scenes in the [applyMiddlewares function](http://redux.js.org/docs/api/applyMiddleware.html). This function doesn't exist in Reductive, because Reason has language-level support for function composition with operators like [`|>` and `@@`](https://caml.inria.fr/pub/docs/manual-ocaml/libref/Pervasives.html#6_Compositionoperators). You can see some [example usage here](https://github.com/reasonml/reductive/blob/master/examples/immutable/timeTravelStore.re#L86-L87) and [some of the deprecation messages](https://github.com/reasonml/reductive/blob/master/src/reductive.rei#L39-L112) to get a better understanding of typical usage.

### The Source
The code behind Reductive is incredibly simple. The [first 40 lines](https://github.com/reasonml/reductive/blob/master/src/reductive.re#L1-L40) Include the entire reimplementation of [redux](https://github.com/reactjs/redux). The [next ~40 lines](https://github.com/reasonml/reductive/blob/master/src/reductive.re#L42-L83) are a reimplementation of the [react-redux](https://github.com/reactjs/react-redux) library (without the higher-order component connect style implementation). The code is short and relatively simple to follow.

## Alternative to Reductive

Hopefully, the above comparisons demonstrated that **you might not need an extra-react state management technique at all**; passing props down works well in [Reason-React](https://github.com/reasonml/reason-react), and since props are well-typed, adding/remove them takes seconds. Keep state/props management simple so that you can spend the learning budget elsewhere!

## Contributing
Current needs:

* Potential bug fixes
* Better examples usages
* More docs on differences from/similarities with Redux
* Different examples of/links to simpler state management in Reason
