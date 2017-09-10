# pytwogeom : another set of `lib2geom` python bindings

## Why not `py2geom`?

`lib2geom` has some existing python bindings, both with `cython` and `boost::python`,
but they look out of date if not broken.

I looked into fixing/updating the `boost::python` bindings [here](http://github.com/snoyer/lib2geom)
but ran into some issues:
- I'm not familiar enough with lib2geom to fix/maintain complete bindings
- I'm not a fan of the build process (compiles along the core lib, install to `site-packages` from `cmake`)

So I'm starting a new set of bindings that:
- build/install with `setuptools` on top of an existing `lib2geom` install
- have no claim of being complete


## Features

As mentioned above, I'm not writing a complete, one-to-one binding of all the `lib2geom` primitives.
Instead I'm aiming to expose a few higher-level operations, mapping the necessary primitives as I go.
I may eventually go for a more complete coverage as I gain familiarity with the library, but in the meantime I'm looking at:

- [x] SVG string IO
- [ ] boolean operations
- [x] adaptive subdivision
- [x] polyline fitting
- [ ] path offset
- [ ] outline stroke
