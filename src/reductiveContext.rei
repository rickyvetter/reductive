module type Config = {
  type state;
  type action;
  let store: Reductive.Store.t(action, state);
};
module Make:
  (Config: Config) =>
   {
    module Provider: {
      [@bs.obj]
      external makeProps:
        (~children: 'children, ~key: string=?, unit) =>
        {. "children": 'children} =
        "";
      let make: {. "children": React.element} => React.element;
    };
    let useSelector: (Config.state => 'selectedState) => 'selectedState;
    let useDispatch: (unit, Config.action) => unit;
  };
