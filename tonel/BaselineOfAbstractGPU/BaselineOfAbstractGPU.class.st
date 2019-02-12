Class {
	#name : #BaselineOfAbstractGPU,
	#superclass : #BaselineOf,
	#category : #BaselineOfAbstractGPU
}

{ #category : #baselines }
BaselineOfAbstractGPU >> baseline: spec [
    <baseline>
	spec for: #'common' do: [
		spec blessing: #'baseline'.
		spec 
			package: 'AbstractGPU-Core';
			package: #'AbstractGPU-Generated' with: [
				spec requires: #('AbstractGPU-Core' ). ];
			package: 'AbstractGPU-Samples' with: [
				spec requires: #('AbstractGPU-Window' 'AbstractGPU-Core' #'AbstractGPU-Generated' ). ];
			package: 'AbstractGPU-Window' with: [
				spec requires: #(#'AbstractGPU-Generated' 'AbstractGPU-Core' ). ]. ].

]
