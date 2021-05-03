|source destination packages goferSource goferDestination|
source := TonelRepository basicFromUrl: 'tonel://tonel'.
destination := MCDirectoryRepository new directory: FileSystem workingDirectory / 'mc'.

packages := #(
    #'AbstractGPU-CorePharo'
    #'AbstractGPU-CoreSqueak'
    #'AbstractGPU-GeneratedPharo'
    #'AbstractGPU-GeneratedSqueak'
    #'AbstractGPU-Utility'
    #'AbstractGPU-Window'
    #'AbstractGPU-Samples'
    #'BaselineOfAbstractGPU'
).

goferSource := Gofer new repository: source.
goferDestination := Gofer new repository: destination.

packages do: [:package |
    goferSource package: package.
    goferDestination package: package.
].

goferSource fetch.
goferDestination push.

"Exit"
Smalltalk quitPrimitive.
