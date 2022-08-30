Class {
	#name : #BaselineOfAbstractGPU,
	#superclass : #BaselineOf,
	#category : #BaselineOfAbstractGPU
}

{ #category : #baselines }
BaselineOfAbstractGPU >> baseline: spec [
    <baseline>

	spec for: #pharo do: [ self baselineForPharo: spec ].
	spec for: #squeak do: [ self baselineForSqueak: spec ].
]

{ #category : #baselines }
BaselineOfAbstractGPU >> baselineForPharo: spec [
	spec postLoadDoIt: #postLoadActionsForPharo.
	spec
		baseline: #'PhaNAPI' with: [
			spec repository: 'github://desromech/phanapi'
		];
		package: #'AbstractGPU-CorePharo' with: [
			spec requires: #(#'PhaNAPI' ).
		];
		package: #'AbstractGPU-GeneratedPharo' with: [
			spec requires: #(#'AbstractGPU-CorePharo' )
		];
		package: #'AbstractGPU-Samples' with: [
			spec requires: #(#'AbstractGPU-Window' #'AbstractGPU-CorePharo' #'AbstractGPU-GeneratedPharo' )
		];
		package: #'AbstractGPU-Utility' with: [
			spec requires: #(#'AbstractGPU-GeneratedPharo' #'AbstractGPU-CorePharo' )
		];
		package: #'AbstractGPU-Window' with: [
			spec requires: #(#'AbstractGPU-GeneratedPharo' #'AbstractGPU-CorePharo' #'AbstractGPU-Utility').
		]

]

{ #category : #baselines }
BaselineOfAbstractGPU >> baselineForSqueak: spec [
	spec postLoadDoIt: #postLoadActionsForSqueak.
	spec
		project: 'FFI' with: [
			spec
				className: 'ConfigurationOfFFI';
				repository: 'http://www.squeaksource.com/MetacelloRepository';
				versionString: #development;
				loads: #('Core')
		];
		baseline: #'PhaNAPI' with: [
			spec repository: 'github://desromech/phanapi'
		];
		package: #'AbstractGPU-CoreSqueak' with: [
			spec requires: #(#'PhaNAPI' )
		];
		package: #'AbstractGPU-GeneratedSqueak' with: [
			spec requires: #(#'AbstractGPU-CoreSqueak' )
		];
		package: #'AbstractGPU-Samples' with: [
			spec requires: #(#'AbstractGPU-Window' #'AbstractGPU-CoreSqueak' #'AbstractGPU-GeneratedSqueak')
		];
		package: #'AbstractGPU-Utility' with: [
			spec requires: #(#'AbstractGPU-CoreSqueak' #'AbstractGPU-GeneratedSqueak')
		];
		package: #'AbstractGPU-Window' with: [
			spec requires: #(#'AbstractGPU-CoreSqueak' #'AbstractGPU-GeneratedSqueak' #'AbstractGPU-Utility').
		]

]

{ #category : #baselines }
BaselineOfAbstractGPU >> postLoadActionsForPharo [
	(Smalltalk globals at: #AGPUCBindings) fetchLibraryInPostLoadAction.
	(Smalltalk globals at: #AGPUGeneratedDoIt) initializeBindings.
]

{ #category : #baselines }
BaselineOfAbstractGPU >> postLoadActionsForSqueak [
	(Smalltalk globals at: #AGPUGeneratedDoIt) initializeBindings.
]
