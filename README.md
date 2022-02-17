# Arm FastModels Translation Module Framework Library

This repository contains the support framework library to define custom
translatin hardware components for the Arm FastModels Simulator.

## Authors

See AUTHORS file.

## License

See LICENSE file.

## Dependencies

To use this component, you will need to have the Arm FastModels including the
relevant licenses.

## Building

**Environment Setup:**
Before you can build you will need to source the configuration
files as described by the Arm FastModels tools. For example:

```
$ source FastModelsTools_11.15/source_all.sh
```

This should set the following environment variables:

* `PVLIB_HOME`: FastModels Portfolio directory

**Building:**
To build the framework support library, simply type:

```
$ make
```

## Artifacts

The build process above will generate the library with the includes in the `build`
directory.

 * `build/lib/libframework.a` a static library for use in the FastModels simulator
 * `build/include/` the include directory for the framework library

## Using the Library

Add the library and the build directories to the `sgproject` file of the simulated
Arm FastModels platform.

```
config "default" {
    ...
    INCLUDE_DIRS = "build/include"
    ...
}
files {
    path = "build/lib/libframework.a"
}
```