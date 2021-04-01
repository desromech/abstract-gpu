# AbstractGPU
The Abstract GPU is a cross platform low-level 3D graphics API.

## Building instruction for Linux and Max OS/X
For building the abstraction layer for Linux and Max OS/X, CMake is required. In
Linux the Vulkan headers and libraries has to be installed. The following commands
can be used for building:

```bash
mkdir build
cd build
cmake ..
make
```

The built files will be available at the dist folder. The samples will not be
built by default. For building the samples, SDL2 has to be installed and the
AGPU_BUILD_SAMPLES option has to be set. Or for simplicity, you have to use
the following commands for building:

```bash
mkdir build
cd build
cmake -DAGPU_BUILD_SAMPLES=True ..
make
```

# Installing bindings
### Pharo
The Pharo bindings can be installed by running the following script in a
playground:

```smalltalk
Metacello new
   baseline: 'AbstractGPU';
   repository: 'github://ronsaldo/abstract-gpu/tonel';
   load
```

This installation script takes care of automatically downloading a version of
the platform specific library binary that is automatically being built on the CI
server.

### Squeak
Since Squeak does not support repositories in the Tonel format, the installation
requires a manual Tonel to Monticello conversion step that uses Pharo. For
converting the Tonel sources, the following script must be executed:

```bash
./tonelToMonticello.sh
```

This script will download a Pharo image, and run the conversion script at
scripts/tonelToMonticello.st which will convert the packages under tonel into
Monticello packages under the mc folder. These converted packages can be
loaded in Squeak by running the following script:

```smalltalk
(Installer repository: 'http://source.squeak.org/FFI')
    install: 'FFI-Pools';
    install: 'FFI-Kernel'.
(Smalltalk at: #ExternalType) initialize.

"Replace with the path to the converted MC files, or make a symlink to this folder ;)"
Installer monticello directory: 'mc';
    install: 'AbstractGPU-CoreSqueak';
    install: 'AbstractGPU-GeneratedSqueak';
    install: 'AbstractGPU-Window';
    install: 'AbstractGPU-Samples'.

(Smalltalk at: #AGPUGeneratedDoIt) initializeBindings.
```
