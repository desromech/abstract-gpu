(#(windows osx linux) includes: self platform name) ifTrue: [
    ^ self importPhanapiBintrayLibrary: 'Agpu' fromUser: 'ronsaldo' repository: 'abstract-gpu' package: 'lib'.
].

self platform name selectCase: {
    #uwp -> [
        self externalLibrary: 'Agpu' buildWith: [:builder |
            builder
                sourceDirectory: MbndAbstractGPUAssets location;
                useSeparateBuildDirectory;
                configureWithCMake;
                buildWithCMake;
                artifactDirectory: 'dist/$(WBT_CMAKE_BUILD_MODE)'
        ].
    ].
}.
