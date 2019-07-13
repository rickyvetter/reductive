module type Config = {
  type state;
  type action;
  let store: Reductive.Store.t(action, state);
};
module Make:
  (Config: Config) =>
   {
    module Context: {
      type t = Reductive.Store.t(Config.action, Config.state);
      let context: React.Context.t(t);
    };
    module Provider: {
      [@bs.obj]
      external makeProps:
        (~children: 'children, ~key: string=?, unit) =>
        {. "children": 'children} =
        "";
      let make: {. "children": React.element} => React.element;
    };
    let useSelector: (Config.state => 'a) => 'a;
    let useDispatch: (unit, Config.action) => unit;
  };
