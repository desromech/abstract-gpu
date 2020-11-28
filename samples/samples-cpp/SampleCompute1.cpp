#include "SampleBase.hpp"

class SampleCompute1: public ComputeSampleBase
{
public:
    int run(int argc, const char **argv)
    {
		// Create the programs.
		auto computeShader = compileShaderFromFile("data/shaders/computeAdd.glsl", AGPU_COMPUTE_SHADER);
		if (!computeShader)
			return false;

		// Create the shader signature.
		auto shaderSignatureBuilder = device->createShaderSignatureBuilder();
		shaderSignatureBuilder->beginBindingBank(1);
		shaderSignatureBuilder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER, 1);
		shaderSignatureBuilder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER, 1);
		shaderSignatureBuilder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER, 1);

		auto shaderSignature = shaderSignatureBuilder->build();
		if (!shaderSignature)
			return false;

		// Create the compute pipeline builder
		auto pipelineBuilder = device->createComputePipelineBuilder();
		pipelineBuilder->setShaderSignature(shaderSignature);
		pipelineBuilder->attachShader(computeShader);

		// Build the pipeline
		auto pipeline = pipelineBuilder->build();
		if (!pipeline)
			return false;

		// Create the command allocator and the command list
		auto commandAllocator = device->createCommandAllocator(AGPU_COMMAND_LIST_TYPE_DIRECT, commandQueue);
		auto commandList = device->createCommandList(AGPU_COMMAND_LIST_TYPE_DIRECT, commandAllocator, pipeline);

		// Create different buffers for uploading, computing on the gpu, and reading back the
		agpu_size arraySize = 256;
		agpu_size arrayByteSize = arraySize* sizeof(float);
        agpu_size bufferByteSize = arrayByteSize * 3;
		auto uploadBuffer = createMappableUploadBuffer(bufferByteSize, nullptr);
        auto readbackBuffer = createMappableReadbackBuffer(bufferByteSize, nullptr);
        auto storageBuffer = createStorageBuffer(bufferByteSize, sizeof(float), nullptr);

		// Create the shader bindings
		auto shaderBindings = shaderSignature->createShaderResourceBinding(0);
		shaderBindings->bindStorageBufferRange(0, storageBuffer, 0, arrayByteSize);
		shaderBindings->bindStorageBufferRange(1, storageBuffer, arrayByteSize, arrayByteSize);
		shaderBindings->bindStorageBufferRange(2, storageBuffer, arrayByteSize*2, arrayByteSize);

		// Write the inputs
		auto mappedPointer = (float*)uploadBuffer->mapBuffer(AGPU_WRITE_ONLY);
		float *leftInput = mappedPointer;
		float *rightInput = mappedPointer + arraySize;

		for (agpu_size i = 0; i < arraySize; ++i)
		{
			leftInput[i] = 1.0f + float(i);
			rightInput[i] = 1.0f + float(i) *2;
		}

		// Unmap the buffer.
		uploadBuffer->unmapBuffer();

        // Copy the data from the upload buffer
        {
			commandList->pushBufferTransitionBarrier(storageBuffer, AGPU_COPY_DESTINATION_BUFFER);
			commandList->copyBuffer(uploadBuffer, 0, storageBuffer, 0, bufferByteSize);
			commandList->popBufferTransitionBarrier();
	    }

		// Dispatch the compute
		commandList->setShaderSignature(shaderSignature);
		commandList->useComputeShaderResources(shaderBindings);
		commandList->dispatchCompute(arraySize, 1, 1);

        // Copy the data into the readback buffer.
        {
			commandList->pushBufferTransitionBarrier(storageBuffer, AGPU_COPY_SOURCE_BUFFER);
			commandList->copyBuffer(storageBuffer, 0, readbackBuffer, 0, bufferByteSize);
			commandList->popBufferTransitionBarrier();
        }

		commandList->close();
		commandQueue->addCommandList(commandList);
		commandQueue->finishExecution();

		// Map the readback buffer;
        mappedPointer = (float*)readbackBuffer->mapBuffer(AGPU_READ_ONLY);
        leftInput = mappedPointer;
        rightInput = mappedPointer + arraySize;
		float *output = rightInput + arraySize;

		// Readback the output
		int exitCode = 0;
		for (size_t i = 0; i < arraySize; ++i)
		{
			auto left = leftInput[i];
			auto right = rightInput[i];
			auto result = output[i];
			auto expected = left + right;
			if (expected != result)
			{
				fprintf(stderr, "%d: %f + %f = %f\n", (int)i, left, right, result);
				exitCode = 1;
			}
            else if(left == 0 && right == 0 && expected == 0)
            {
                fprintf(stderr, "%d is completely zero\n", (int)i);
                exitCode = 1;
            }
		}

        readbackBuffer->unmapBuffer();
		if (exitCode == 0)
			printf("Success\n");

        return exitCode;
    }
};

SAMPLE_MAIN(SampleCompute1)
